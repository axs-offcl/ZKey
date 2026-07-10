// Copyright (c) 2026 ZKey Contributors
// This file is part of ZKey.
// ZKey is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later version.
// ZKey is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with ZKey.
// If not, see <https://www.gnu.org/licenses/>.

#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QShortcut>
#include <QStyle>
#include <QTimer>
#include <QDir>
#include <QFileInfo>
#include <QSet>
#include <QEvent>
#include <QSpinBox>
#include <QComboBox>

#include "inputmanager.h"
#include "overlaywidget.h"
#include "overlaymanager.h"
#include "profilemanager.h"
#include "settingswindow.h"
#include "../core/layoutmanager.h"
#include "../core/config.h"
#include "../core/theme.h"
#include "cli_manager.h"
#include "obsgateway.h"
#include "nativehotkeymanager.h"
#include "themewidget.h"
#include <QElapsedTimer>
#include <cstdio>

class App : public QObject {
    Q_OBJECT
public:
    App()
    {
        QElapsedTimer total;
        total.start();

        QElapsedTimer t;
        t.start();
        m_config = ConfigManager::load();
        fprintf(stderr, "[PERF] Config load: %lld ms\n", t.elapsed());

        if (m_config.keyboardLanguage >= 0) {
            KeyboardLayoutDetector::setOverride(static_cast<KeyboardLayoutDetector::Layout>(m_config.keyboardLanguage));
        }

        t.restart();
        m_input = new InputManager(this);
        m_input->start();
        fprintf(stderr, "[PERF] InputManager start: %lld ms\n", t.elapsed());

        t.restart();
        m_layoutMgr = new LayoutManager(this);
        QString layoutsPath = QCoreApplication::applicationDirPath() + "/layouts";
        m_layoutMgr->setLayoutsDir(layoutsPath);
        m_layoutMgr->scanLayouts();
        fprintf(stderr, "[PERF] Layout scan: %lld ms\n", t.elapsed());

        t.restart();
        m_overlayMgr = new OverlayManager(m_input, this);
        m_overlayMgr->setLayoutManager(m_layoutMgr);
        m_overlayMgr->loadFromJson(m_config.overlays, m_layoutMgr);
        fprintf(stderr, "[PERF] Overlay load: %lld ms\n", t.elapsed());

        if (m_overlayMgr->count() == 0) {
            QJsonObject overlayCfg;
            overlayCfg["id"] = "";
            overlayCfg["type"] = "Combined";
            overlayCfg["name"] = "Overlay";
            overlayCfg["showKeyboard"] = true;
            overlayCfg["showMouse"] = true;
            overlayCfg["enabled"] = true;
            m_overlayMgr->addOverlay(overlayCfg);
        }

        t.restart();
        m_profileMgr = new ProfileManager(m_overlayMgr, this);
        m_settings = new SettingsWindow(m_overlayMgr, m_profileMgr, m_input, m_layoutMgr, nullptr);
        fprintf(stderr, "[PERF] SettingsWindow create: %lld ms\n", t.elapsed());
        m_settings->loadFromConfig(
            m_config.keyboardLayout, m_config.showKeyboard,
            m_config.showMouse, m_config.lockPosition,
            m_config.lockMouseSection, m_config.opacityPercent,
            m_config.themeIndex, m_config.mouseOffset,
            m_config.keyboardOffset, m_config.keyboardOpacityPercent,
            m_config.mouseOpacityPercent, m_config.autoHideGames,
            m_config.osdMode, m_config.osdFontSize,
            m_config.osdFontColor, m_config.osdBgOpacity,
            m_config.osdPosX, m_config.osdPosY,
            m_config.osdAnchor, m_config.osdFormat,
            m_config.osdMaxKeys, m_config.osdHideWhenEmpty,
            m_config.osdSortOrder,
            m_config.useCustomLayout, m_config.customLayoutName,
            m_overlayMgr->primaryOverlay() ? m_overlayMgr->primaryOverlay()->backgroundOpacity() : 0
        );
        m_settings->loadAppearance(m_config.appearance);
        m_settings->move(m_config.settingsPos);
        m_settings->resize(m_config.settingsSize);
        m_settings->syncToOverlay(m_overlayMgr->primaryOverlay());
        fprintf(stderr, "[PERF] Settings loadFromConfig: %lld ms\n", t.elapsed());

        t.restart();
        setupTray();
        fprintf(stderr, "[PERF] Tray setup: %lld ms\n", t.elapsed());

        t.restart();
        setupHotkey();
        fprintf(stderr, "[PERF] Hotkey setup: %lld ms\n", t.elapsed());

        connect(m_overlayMgr, &OverlayManager::overlayAdded, this, [this](OverlayWidget *w) {
            connect(w, &OverlayWidget::positionChanged, this, [this](QPoint) { saveConfig(); });
            connect(w, &OverlayWidget::settingsChanged, this, [this]() { saveConfig(); });
        });
        connect(m_settings, &SettingsWindow::settingsChanged, this, [this]() {
            saveConfig();
        });

        m_hotkeyMgr = new NativeHotkeyManager(this);
        qApp->installNativeEventFilter(m_hotkeyMgr);
        m_hotkeyMgr->registerHotkeys(m_config);

        connect(m_hotkeyMgr, &NativeHotkeyManager::toggleVisibilityTriggered, this, [this]() {
            if (!m_overlayMgr) return;
            for (auto *w : m_overlayMgr->allOverlays())
                w->setOverlayVisible(!w->isVisible());
        });
        connect(m_hotkeyMgr, &NativeHotkeyManager::toggleAutoHideTriggered, this, [this]() {
            m_settings->setAutoHideGames(!m_settings->autoHideGames());
        });
        connect(m_hotkeyMgr, &NativeHotkeyManager::toggleMoveTriggered, this, [this]() {
            m_settings->setLocked(!m_settings->isLocked());
        });
        connect(m_hotkeyMgr, &NativeHotkeyManager::openPreferencesTriggered, this, [this]() {
            m_settings->show();
            m_settings->raise();
            m_settings->activateWindow();
        });
        connect(m_hotkeyMgr, &NativeHotkeyManager::increaseOpacityTriggered, this, [this]() {
            int val = qMin(100, m_settings->opacityPercent() + 5);
            m_settings->setOpacityPercent(val);
        });
        connect(m_hotkeyMgr, &NativeHotkeyManager::decreaseOpacityTriggered, this, [this]() {
            int val = qMax(0, m_settings->opacityPercent() - 5);
            m_settings->setOpacityPercent(val);
        });

        connect(m_settings, &SettingsWindow::hotkeyConfigChanged, this, [this]() {
            m_hotkeyMgr->registerHotkeys(m_config);
        });

        m_obsGateway = new ObsGateway(m_input, this);
        if (m_config.obsEnabled) {
            m_obsGateway->start(m_config.obsPort);
            auto *po = m_overlayMgr->primaryOverlay();
            m_obsGateway->updateConfig(
                po ? po->showKeyboard() : true,
                po ? po->showMouse() : true,
                m_config.themeIndex
            );
        }
        connect(m_settings, &SettingsWindow::settingsChanged, this, [this]() {
            if (m_obsGateway) {
                bool wantOn = m_settings->obsEnabled();
                int port = m_settings->obsPort();
                if (wantOn && !m_obsGateway->isRunning()) {
                    m_obsGateway->start(port);
                } else if (!wantOn && m_obsGateway->isRunning()) {
                    m_obsGateway->stop();
                } else if (wantOn && m_obsGateway->isRunning() && m_obsGateway->port() != port) {
                    m_obsGateway->stop();
                    m_obsGateway->start(port);
                }
                if (m_obsGateway->isRunning()) {
                    auto *po = m_overlayMgr->primaryOverlay();
                    m_obsGateway->updateConfig(
                        po ? po->showKeyboard() : true,
                        po ? po->showMouse() : true,
                        m_config.themeIndex
                    );
                }
            }
        });

        if (m_config.settingsVisible) {
            QTimer::singleShot(200, m_settings, &QWidget::show);
        }

        fprintf(stderr, "[PERF] TOTAL startup: %lld ms\n", total.elapsed());
    }

    ~App()
    {
        saveConfig();
        m_input->stop();
    }

private:
    void setupTray()
    {
        QIcon trayIco(":/icon/zkey.ico");
        if (trayIco.isNull())
            trayIco = qApp->style()->standardIcon(QStyle::SP_ComputerIcon);
        m_trayIcon = new QSystemTrayIcon(trayIco, this);
        m_trayIcon->setToolTip("ZKey Overlay v2.0");

        auto *menu = new QMenu;
        auto *toggleOverlay = menu->addAction("Toggle Overlay");
        auto *showSettings = menu->addAction("Settings");
        menu->addSeparator();
        auto *quit = menu->addAction("Quit");

        m_trayIcon->setContextMenu(menu);
        m_trayIcon->show();

        connect(toggleOverlay, &QAction::triggered, this, [this]() {
            if (!m_overlayMgr) return;
            for (auto *w : m_overlayMgr->allOverlays())
                w->setOverlayVisible(!w->isVisible());
        });
        connect(showSettings, &QAction::triggered, this, [this]() {
            m_settings->show();
            m_settings->raise();
            m_settings->activateWindow();
        });
        connect(quit, &QAction::triggered, qApp, &QApplication::quit);
    }

    void setupHotkey()
    {
        auto connectHotkey = [this](OverlayWidget *w) {
            auto *shortcut = new QShortcut(QKeySequence("Ctrl+Shift+K"), w);
            connect(shortcut, &QShortcut::activated, this, [this]() {
                m_settings->setVisible(!m_settings->isVisible());
                if (m_settings->isVisible()) {
                    m_settings->raise();
                    m_settings->activateWindow();
                }
            });
        };

        auto *primary = m_overlayMgr ? m_overlayMgr->primaryOverlay() : nullptr;
        if (primary) {
            connectHotkey(primary);
        }
        connect(m_overlayMgr, &OverlayManager::overlayAdded, this, [this, connectHotkey](OverlayWidget *w) {
            static bool hotkeyConnected = false;
            if (!hotkeyConnected) {
                connectHotkey(w);
                hotkeyConnected = true;
            }
        });
    }

    void saveConfig()
    {
        m_config.keyboardLayout = m_settings->layoutIndex();
        m_config.keyboardLanguage = m_settings->keyboardLanguage();
        m_config.showKeyboard = m_settings->showKeyboard();
        m_config.showMouse = m_settings->showMouse();
        m_config.lockPosition = m_settings->isLocked();
        m_config.lockMouseSection = m_settings->isMouseSectionLocked();
        m_config.autoHideGames = m_settings->autoHideGames();
        m_config.opacityPercent = m_settings->opacityPercent();
        m_config.keyboardOpacityPercent = m_settings->keyboardOpacityPercent();
        m_config.mouseOpacityPercent = m_settings->mouseOpacityPercent();
        m_config.themeIndex = m_settings->themeIndex();
        m_config.mouseOffset = m_settings->mouseOffset();
        m_config.keyboardOffset = m_settings->keyboardOffset();
        m_config.useCustomLayout = m_settings->useCustomLayout();
        m_config.customLayoutName = m_settings->customLayoutName();
        m_config.osdMode = m_settings->osdMode();
        m_config.osdFontSize = m_settings->osdFontSize();
        m_config.osdFontColor = m_settings->osdFontColor();
        m_config.osdBgOpacity = m_settings->osdBgOpacity();
        m_config.osdPosX = m_settings->osdPosX();
        m_config.osdPosY = m_settings->osdPosY();
        m_config.osdAnchor = m_settings->osdAnchor();
        m_config.osdFormat = m_settings->osdFormat();
        m_config.osdMaxKeys = m_settings->osdMaxKeys();
        m_config.osdHideWhenEmpty = m_settings->osdHideWhenEmpty();
        m_config.osdSortOrder = m_settings->osdSortOrder();

        if (m_overlayMgr)
            m_config.overlays = m_overlayMgr->saveToJson();

        m_config.appearance = m_settings->appAppearance();
        m_config.obsEnabled = m_settings->obsEnabled();
        m_config.hotkeysEnabled = m_settings->hotkeysEnabled();
        m_config.hkToggleVisibility = m_settings->hotkey(0);
        m_config.hkToggleAutoHide = m_settings->hotkey(1);
        m_config.hkToggleMove = m_settings->hotkey(2);
        m_config.hkOpenPreferences = m_settings->hotkey(3);
        m_config.hkIncreaseOpacity = m_settings->hotkey(4);
        m_config.hkDecreaseOpacity = m_settings->hotkey(5);
        m_config.settingsVisible = m_settings->isVisible();
        m_config.settingsPos = m_settings->pos();
        m_config.settingsSize = m_settings->size();
        ConfigManager::save(m_config);
    }

    InputManager *m_input;
    OverlayManager *m_overlayMgr;
    ProfileManager *m_profileMgr;
    SettingsWindow *m_settings;
    LayoutManager *m_layoutMgr;
    ObsGateway *m_obsGateway = nullptr;
    NativeHotkeyManager *m_hotkeyMgr = nullptr;
    QSystemTrayIcon *m_trayIcon;
    AppConfig m_config;
};

static LONG WINAPI crashHandler(EXCEPTION_POINTERS *ep)
{
    HANDLE h = CreateFileA("zkey_crash.log", GENERIC_WRITE, FILE_SHARE_READ,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h != INVALID_HANDLE_VALUE) {
        SetFilePointer(h, 0, nullptr, FILE_END);
        char buf[256];
        int n = snprintf(buf, sizeof(buf),
            "ZKey crash\nException code: 0x%08lX\nAddress: 0x%p\n",
            ep->ExceptionRecord->ExceptionCode,
            ep->ExceptionRecord->ExceptionAddress);
        DWORD written;
        WriteFile(h, buf, n, &written, nullptr);
        CloseHandle(h);
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

class WheelFilter : public QObject {
public:
    using QObject::QObject;
protected:
    bool eventFilter(QObject *obj, QEvent *event) override {
        if (event->type() == QEvent::Wheel) {
            if (qobject_cast<QComboBox*>(obj)) return true;
            if (qobject_cast<QSpinBox*>(obj)) {
                auto *w = qobject_cast<QWidget*>(obj);
                if (w && !w->hasFocus()) return true;
            }
        }
        return QObject::eventFilter(obj, event);
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    SetUnhandledExceptionFilter(crashHandler);
    app.setStyle("Fusion");
    app.setApplicationName("ZKey");
    app.setApplicationVersion("2.0.0");
    app.setOrganizationName("ZKey");

    auto *wf = new WheelFilter(&app);
    app.installEventFilter(wf);

    QIcon appIcon(":/icon/zkey.png");
    if (!appIcon.isNull())
        app.setWindowIcon(appIcon);

    app.setStyleSheet(
        "QMainWindow, QWidget { background: #0a0a0a; color: #cdd6f4; font-family: 'Segoe UI', 'Inter', sans-serif; font-size: 12px; }"
        "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #1a1a1a, stop:1 #141414); color: #cdd6f4; border: 1px solid #2a2a2a; border-radius: 8px; padding: 8px 20px; font-size: 12px; font-weight: 600; }"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #252525, stop:1 #1e1e1e); border-color: #cba6f7; }"
        "QPushButton:pressed { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #303030, stop:1 #252525); }"
        "QPushButton:disabled { background: #0e0e0e; color: #3e3e3e; border-color: #1a1a1a; }"
        "QComboBox { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #141414, stop:1 #0e0e0e); color: #cdd6f4; border: 1px solid #2a2a2a; border-radius: 8px; padding: 6px 12px; font-size: 12px; min-height: 22px; }"
        "QComboBox:hover { border-color: #454545; }"
        "QComboBox::drop-down { border: none; width: 28px; }"
        "QComboBox QAbstractItemView { background: #0e0e0e; color: #cdd6f4; selection-background-color: #1a1a1a; border: 1px solid #2a2a2a; border-radius: 8px; padding: 6px; outline: none; }"
        "QComboBox QAbstractItemView::item { padding: 6px 12px; border-radius: 4px; min-height: 24px; }"
        "QListWidget { background: #080808; color: #cdd6f4; border: 1px solid #1a1a1a; border-radius: 10px; outline: none; }"
        "QListWidget::item { padding: 10px 14px; border-radius: 8px; margin: 3px 6px; }"
        "QListWidget::item:selected { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #1a1225, stop:1 #141414); color: #cba6f7; font-weight: 600; border: 1px solid #2a1e40; }"
        "QListWidget::item:hover:!selected { background: #0e0e0e; color: #a6adc8; }"
        "QGroupBox { border: 1px solid #1a1a1a; border-radius: 12px; margin-top: 14px; padding-top: 20px; font-weight: 600; background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #0c0c0c, stop:1 #0a0a0a); }"
        "QGroupBox::title { subcontrol-origin: margin; left: 16px; padding: 0 10px; color: #6a6a6a; font-size: 11px; font-weight: 700; letter-spacing: 1.5px; text-transform: uppercase; }"
        "QSlider::groove:horizontal { height: 6px; background: #1a1a1a; border-radius: 3px; }"
        "QSlider::handle:horizontal { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #d4a5ff, stop:1 #cba6f7); width: 18px; height: 18px; margin: -7px 0; border-radius: 9px; border: 2px solid #1a1a1a; }"
        "QSlider::handle:horizontal:hover { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #e0bfff, stop:1 #b4befe); }"
        "QSlider::sub-page:horizontal { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #cba6f7, stop:1 #b4befe); border-radius: 3px; }"
        "QSpinBox { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #141414, stop:1 #0e0e0e); color: #cdd6f4; border: 1px solid #2a2a2a; border-radius: 8px; padding: 5px 10px; font-size: 12px; min-height: 22px; }"
        "QSpinBox:hover { border-color: #454545; }"
        "QSpinBox:focus { border-color: #cba6f7; }"
        "QCheckBox { color: #cdd6f4; spacing: 10px; font-size: 12px; }"
        "QCheckBox::indicator { width: 20px; height: 20px; border: 2px solid #2a2a2a; border-radius: 6px; background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #141414, stop:1 #0e0e0e); }"
        "QCheckBox::indicator:hover { border-color: #454545; }"
        "QCheckBox::indicator:checked { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #d4a5ff, stop:1 #cba6f7); border-color: #cba6f7; }"
        "QLabel { color: #cdd6f4; font-size: 12px; }"
        "QLineEdit { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #141414, stop:1 #0e0e0e); color: #cdd6f4; border: 1px solid #2a2a2a; border-radius: 8px; padding: 7px 12px; font-size: 12px; }"
        "QLineEdit:hover { border-color: #454545; }"
        "QLineEdit:focus { border-color: #cba6f7; }"
        "QScrollArea { border: none; background: transparent; }"
        "QScrollBar:vertical { background: transparent; width: 8px; border-radius: 4px; }"
        "QScrollBar::handle:vertical { background: #1a1a1a; border-radius: 4px; min-height: 24px; }"
        "QScrollBar::handle:vertical:hover { background: #2a2a2a; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }"
        "QScrollBar:horizontal { background: transparent; height: 8px; border-radius: 4px; }"
        "QScrollBar::handle:horizontal { background: #1a1a1a; border-radius: 4px; min-width: 24px; }"
        "QScrollBar::handle:horizontal:hover { background: #2a2a2a; }"
        "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }"
        "QToolTip { background: #0e0e0e; color: #cdd6f4; border: 1px solid #2a2a2a; border-radius: 8px; padding: 8px 12px; font-size: 11px; }"
        "QMessageBox { background: #0a0a0a; }"
        "QInputDialog { background: #0a0a0a; }"
        "QDesktopWidget { background: #0a0a0a; }"
        "OverlayWidget { background: transparent; border: none; }"
    );

    QStringList args = app.arguments();
    QSet<QString> commands = {
        "--help", "-h", "/?",
        "--version", "-v",
        "--list-profiles"
    };
    for (int i = 1; i < args.size(); ++i) {
        if (commands.contains(args[i])) {
            CLIManager cli;
            return cli.run(args);
        }
    }

    int ret;
    {
        App a;
        ret = app.exec();
    }
    return ret;
}

#include "main_overlay.moc"
