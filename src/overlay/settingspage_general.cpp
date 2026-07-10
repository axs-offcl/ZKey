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

#include "settingspage_general.h"
#include "toggleswitch.h"
#include "styledslider.h"
#include "../core/layoutmanager.h"
#include "overlaywidget.h"
#include "overlaymanager.h"
#include "../core/keyboards.h"
#include "../core/keyboarddetector.h"
#include "settingswindow.h"
#include "../core/config.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QSharedPointer>
#include <QPainter>
#include <QPainterPath>
#include <QNetworkInterface>
#include <QClipboard>
#include <QApplication>
#include <QMessageBox>
#ifdef Q_OS_WIN
#include <windows.h>
#endif

static QPixmap renderKeyboardPreview(int layoutIndex, int, int)
{
    int w = 240, h = 120;
    QPixmap pm(w, h);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing, true);

    auto &t = ThemeManager::theme(0);
    if (layoutIndex < 0 || layoutIndex >= keyboardCount) return pm;
    auto &layout = keyboards[layoutIndex];

    int rows = layout.rows.size();
    if (rows == 0) return pm;

    double totalUnits = 0;
    for (auto &row : layout.rows) {
        double rowUnits = 0;
        for (auto &key : row) {
            if (key.extraSpacing > 30) rowUnits += 3;
            else if (key.extraSpacing > 10) rowUnits += 2;
            else rowUnits += 1;
        }
        if (rowUnits > totalUnits) totalUnits = rowUnits;
    }
    if (totalUnits == 0) return pm;

    double margin = 8;
    double keyW = (w - margin * 2) / totalUnits;
    double keyH = (h - margin * 2) / rows;
    double pad = 2;
    double radius = 3;

    QColor keyBg = t.keyBg.lighter(130);
    QColor keyBorder = t.keyBg.lighter(160);

    for (int r = 0; r < rows; ++r) {
        double x = margin;
        for (auto &key : layout.rows[r]) {
            int wMult = 1;
            if (key.extraSpacing > 30) wMult = 3;
            else if (key.extraSpacing > 10) wMult = 2;

            QRectF kr(x + pad, margin + r * keyH + pad, keyW * wMult - pad * 2, keyH - pad * 2);
            p.setPen(QPen(keyBorder, 1));
            p.setBrush(keyBg);
            p.drawRoundedRect(kr, radius, radius);
            x += keyW * wMult;
        }
    }
    p.end();
    return pm;
}

static QPixmap renderMousePreview(int shapeIndex, int w, int h)
{
    QPixmap pm(w, h);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing, true);

    auto &t = ThemeManager::theme(0);
    QColor bodyFill = t.keyBg.lighter(130);
    QColor bodyLight = t.keyBg.lighter(150);

    int mx = w / 2 - 20;
    int my = h / 2 - 25;
    int mw = 40;
    int mh = 50;

    QPainterPath bodyPath;

    if (shapeIndex == 0) {
        double r = mw * 0.23;
        double cy = my + mh * 0.62;
        bodyPath.moveTo(mx + r, my);
        bodyPath.lineTo(mx + mw - r, my);
        bodyPath.quadTo(mx + mw, my, mx + mw, my + r);
        bodyPath.lineTo(mx + mw, cy);
        bodyPath.quadTo(mx + mw, my + mh, mx + mw * 0.5, my + mh);
        bodyPath.quadTo(mx, my + mh, mx, cy);
        bodyPath.lineTo(mx, my + r);
        bodyPath.quadTo(mx, my, mx + r, my);
        bodyPath.closeSubpath();
        p.setPen(Qt::NoPen);
        p.setBrush(bodyFill);
        p.drawPath(bodyPath);
        int splitY = my + mh * 0.42;
        p.setPen(QPen(bodyFill.lighter(140), 0.8));
        p.drawLine(mx, splitY, mx + mw, splitY);
        QPainterPath wheel;
        int ww = mw * 0.12;
        int wh = mh * 0.22;
        wheel.addRoundedRect(QRectF(mx + mw/2 - ww/2, my + mh*0.05, ww, wh), ww/2, ww/2);
        p.fillPath(wheel, QColor(70, 70, 70));
        p.setPen(QPen(bodyFill.lighter(140), 0.8));
        p.drawLine(mx + mw/2, my, mx + mw/2, splitY);
        p.save();
        p.setClipPath(bodyPath);
        QRectF left(mx, my, mw/2, splitY - my);
        QRectF right(mx + mw/2, my, mw/2, splitY - my);
        p.fillRect(left, bodyLight);
        p.fillRect(right, QColor(60, 60, 70));
        QRectF bottom(mx, splitY, mw, mh - (splitY - my));
        p.fillRect(bottom, QColor(60, 60, 70));
        p.restore();
        p.setPen(QPen(bodyFill.lighter(140), 0.8));
        p.setBrush(Qt::NoBrush);
        p.drawPath(bodyPath);
    } else if (shapeIndex == 5) {
        int vmx = w / 2 - 10;
        int vmy = h / 2 - 28;
        int vmw = 20;
        int vmh = 56;
        bodyPath.addRoundedRect(QRect(vmx, vmy, vmw, vmh), 6, 6);
        p.setPen(Qt::NoPen);
        p.setBrush(bodyFill);
        p.drawPath(bodyPath);
        p.setClipPath(bodyPath);
        int btnH = vmh * 43 / 100;
        p.setBrush(bodyLight);
        p.drawRect(vmx, vmy, vmw/2 - 1, btnH);
        p.drawRect(vmx + vmw/2 + 1, vmy, vmw/2 - 1, btnH);
        p.setClipping(false);
        p.setPen(QPen(bodyFill.lighter(140), 0.8));
        p.setBrush(Qt::NoBrush);
        p.drawPath(bodyPath);
    } else if (shapeIndex == 4) {
        bodyPath.addEllipse(QRect(mx, my, mw, mh));
        p.setPen(Qt::NoPen);
        p.setBrush(bodyFill);
        p.drawPath(bodyPath);
        p.setClipPath(bodyPath);
        int btnH = mh * 43 / 100;
        p.setBrush(bodyLight);
        p.drawRect(mx, my, mw/2 - 1, btnH);
        p.drawRect(mx + mw/2 + 1, my, mw/2 - 1, btnH);
        p.setClipping(false);
        p.setPen(QPen(bodyFill.lighter(140), 0.8));
        p.setBrush(Qt::NoBrush);
        p.drawPath(bodyPath);
    } else if (shapeIndex == 3) {
        bodyPath.addRect(QRect(mx, my, mw, mh));
        p.setPen(Qt::NoPen);
        p.setBrush(bodyFill);
        p.drawPath(bodyPath);
        p.setClipPath(bodyPath);
        int btnH = mh * 43 / 100;
        p.setBrush(bodyLight);
        p.drawRect(mx, my, mw/2 - 1, btnH);
        p.drawRect(mx + mw/2 + 1, my, mw/2 - 1, btnH);
        p.setClipping(false);
        p.setPen(QPen(bodyFill.lighter(140), 0.8));
        p.setBrush(Qt::NoBrush);
        p.drawPath(bodyPath);
    } else if (shapeIndex == 2) {
        bodyPath.moveTo(mx, my + mh * 30 / 100);
        bodyPath.quadTo(mx - 2, my + mh * 55 / 100, mx, my + mh * 75 / 100);
        bodyPath.quadTo(mx + mw / 2, my + mh + 3, mx + mw, my + mh * 70 / 100);
        bodyPath.quadTo(mx + mw + 2, my + mh * 40 / 100, mx + mw, my);
        bodyPath.quadTo(mx + mw / 2, my - 1, mx, my);
        bodyPath.closeSubpath();
        p.setPen(Qt::NoPen);
        p.setBrush(bodyFill);
        p.drawPath(bodyPath);
        p.setClipPath(bodyPath);
        int btnH = mh * 43 / 100;
        p.setBrush(bodyLight);
        p.drawRect(mx, my, mw/2 - 1, btnH);
        p.drawRect(mx + mw/2 + 1, my, mw/2 - 1, btnH);
        p.setClipping(false);
        p.setPen(QPen(bodyFill.lighter(140), 0.8));
        p.setBrush(Qt::NoBrush);
        p.drawPath(bodyPath);
    } else if (shapeIndex == 1) {
        bodyPath.addRoundedRect(QRect(mx, my, mw, mh), 3, 3);
        p.setPen(Qt::NoPen);
        p.setBrush(bodyFill);
        p.drawPath(bodyPath);
        p.setClipPath(bodyPath);
        int btnH = mh * 43 / 100;
        p.setBrush(bodyLight);
        p.drawRect(mx, my, mw/2 - 1, btnH);
        p.drawRect(mx + mw/2 + 1, my, mw/2 - 1, btnH);
        p.setClipping(false);
        p.setPen(QPen(bodyFill.lighter(140), 0.8));
        p.setBrush(Qt::NoBrush);
        p.drawPath(bodyPath);
    }

    p.end();
    return pm;
}

SettingsPage_General::SettingsPage_General(SettingsContext &ctx, QWidget *parent)
    : SettingsPage(ctx, parent)
{
    auto *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page);
    layout->setSpacing(6);
    layout->setContentsMargins(16, 12, 16, 12);

    auto *kbGroup = new QGroupBox("KEYBOARD");
    auto *kbForm = new QFormLayout(kbGroup);
    kbForm->setSpacing(4);
    kbForm->setContentsMargins(12, 16, 12, 8);
    kbForm->setLabelAlignment(Qt::AlignRight);

    m_layoutCombo = new QComboBox;
    m_layoutCombo->addItems({"Whole Keyboard", "No Numpad", "Only Numbers", "Compact Numbers", "Control Buttons", "60%"});
    m_layoutCombo->hide();

    // Keyboard layout preview cards
    static const char *layoutNames[] = {"Whole KB", "No Numpad", "Numbers", "Compact", "Ctrl Btns", "60%"};
    auto *kbCardRow = new QHBoxLayout;
    kbCardRow->setSpacing(6);
    for (int i = 0; i < keyboardCount && i < 6; ++i) {
        auto *card = new QPushButton;
        card->setFixedSize(80, 56);
        card->setToolTip(layoutNames[i]);
        card->setProperty("layoutIndex", i);
        card->setFlat(true);
        card->setText(layoutNames[i]);
        card->setStyleSheet(
            "QPushButton { border: 2px solid #333; border-radius: 6px; padding: 0; background: #1c1c1c; color: #ccc; font-size: 9px; font-weight: bold; }"
            "QPushButton:hover { border: 2px solid #555; background: #2a2a2a; }"
            "QPushButton[activeLayout=\"true\"] { border: 2px solid #e74c3c; background: #2a2a2a; color: #fff; }");
        connect(card, &QPushButton::clicked, this, [this, i]() {
            m_layoutCombo->setCurrentIndex(i);
            for (auto *c : m_layoutCards) {
                c->setProperty("activeLayout", c->property("layoutIndex").toInt() == i);
                c->style()->unpolish(c);
                c->style()->polish(c);
            }
        });
        m_layoutCards.append(card);
        kbCardRow->addWidget(card);
    }
    kbCardRow->addStretch();
    kbForm->addRow(kbCardRow);

    m_showKeyboardToggle = new ToggleSwitch;
    kbForm->addRow("Enable:", m_showKeyboardToggle);

    m_kpsToggle = new ToggleSwitch;
    m_kpsResetToggle = new ToggleSwitch;
    auto *kpsRow = new QHBoxLayout;
    kpsRow->setSpacing(12);
    auto *kpsShowCol = new QVBoxLayout;
    kpsShowCol->setSpacing(2);
    kpsShowCol->setContentsMargins(0, 0, 0, 0);
    auto *kpsShowLabel = new QLabel("Show");
    kpsShowLabel->setStyleSheet("font-size: 9px;");
    kpsShowLabel->setAlignment(Qt::AlignCenter);
    kpsShowCol->addWidget(m_kpsToggle, 0, Qt::AlignCenter);
    kpsShowCol->addWidget(kpsShowLabel);
    auto *kpsResetCol = new QVBoxLayout;
    kpsResetCol->setSpacing(2);
    kpsResetCol->setContentsMargins(0, 0, 0, 0);
    auto *kpsResetLabel = new QLabel("Reset");
    kpsResetLabel->setStyleSheet("font-size: 9px;");
    kpsResetLabel->setAlignment(Qt::AlignCenter);
    kpsResetCol->addWidget(m_kpsResetToggle, 0, Qt::AlignCenter);
    kpsResetCol->addWidget(kpsResetLabel);
    kpsRow->addLayout(kpsShowCol);
    kpsRow->addLayout(kpsResetCol);
    kpsRow->addStretch();
    kbForm->addRow("KPS counter:", kpsRow);

    auto *srcCombo = new QComboBox;
    srcCombo->addItems({"Built-in", "Custom"});
    m_layoutSourceCombo = srcCombo;
    m_customLayoutCombo = new QComboBox;
    m_customLayoutCombo->setEnabled(false);
    m_customLayoutCombo->setMaximumWidth(140);
    m_editLayoutBtn = new QPushButton("Edit");
    m_editLayoutBtn->setEnabled(false);
    m_editLayoutBtn->setFixedWidth(50);
    auto *srcRow = new QHBoxLayout;
    srcRow->setSpacing(4);
    srcRow->addWidget(srcCombo);
    srcRow->addWidget(m_customLayoutCombo);
    srcRow->addWidget(m_editLayoutBtn);
    kbForm->addRow("Source:", srcRow);

    m_keyboardLangCombo = new QComboBox;
    m_keyboardLangCombo->addItems({"Auto-detect", "US QWERTY", "French AZERTY", "German QWERTZ", "Spanish", "Arabic", "Turkish", "Portuguese",
        "Colemak", "Dvorak", "Workman", "Colemak DH", "Canary", "Engram", "Sturdy",
        "Hands Down", "Hiéroglyphes", "Focal", "QWERTY Wide", "RSTHD", "JCUKEN (RU)",
        "BÉPO", "AZERTY BÉP", "Colemak-QWER"});
    m_keyboardLangCombo->setToolTip("Auto-detect uses your Windows keyboard layout");
    kbForm->addRow("Language:", m_keyboardLangCombo);

    m_kbOffsetX = new QSpinBox;
    m_kbOffsetX->setRange(0, 500);
    m_kbOffsetX->setValue(0);
    m_kbOffsetY = new QSpinBox;
    m_kbOffsetY->setRange(0, 500);
    m_kbOffsetY->setValue(0);
    auto *kbOffRow = new QHBoxLayout;
    kbOffRow->setSpacing(4);
    kbOffRow->addWidget(m_kbOffsetX);
    kbOffRow->addWidget(new QLabel("Y:"));
    kbOffRow->addWidget(m_kbOffsetY);
    kbForm->addRow("Offset:", kbOffRow);

    layout->addWidget(kbGroup);

    auto *mouseGroup = new QGroupBox("MOUSE");
    auto *mouseForm = new QFormLayout(mouseGroup);
    mouseForm->setSpacing(4);
    mouseForm->setContentsMargins(12, 16, 12, 8);
    mouseForm->setLabelAlignment(Qt::AlignRight);

    // Mouse shape preview card
    m_mousePreviewBtn = new QPushButton;
    m_mousePreviewBtn->setFixedSize(80, 56);
    m_mousePreviewBtn->setToolTip("Mouse shape preview");
    m_mousePreviewBtn->setFlat(true);
    // Build mouse card pixmap
    QPixmap mPx(80, 56);
    mPx.fill(QColor("#1c1c1c"));
    {
        QPainter cp(&mPx);
        cp.setRenderHint(QPainter::Antialiasing);
        // Mouse body
        int mx = 24, my = 4, mw = 32, mh = 40;
        QPainterPath bodyPath;
        double r = mw * 0.25;
        double cy = my + mh * 0.58;
        bodyPath.moveTo(mx + r, my);
        bodyPath.lineTo(mx + mw - r, my);
        bodyPath.quadTo(mx + mw, my, mx + mw, my + r);
        bodyPath.lineTo(mx + mw, cy);
        bodyPath.quadTo(mx + mw, my + mh, mx + mw * 0.5, my + mh);
        bodyPath.quadTo(mx, my + mh, mx, cy);
        bodyPath.lineTo(mx, my + r);
        bodyPath.quadTo(mx, my, mx + r, my);
        bodyPath.closeSubpath();
        cp.setPen(Qt::NoPen);
        cp.setBrush(QColor("#3a3a50"));
        cp.drawPath(bodyPath);
        int splitY = my + mh * 0.40;
        cp.setPen(QPen(QColor("#4a4a60"), 0.6));
        cp.drawLine(mx, splitY, mx + mw, splitY);
        cp.setPen(Qt::NoPen);
        cp.setBrush(QColor("#555566"));
        cp.drawRoundedRect(QRectF(mx + mw/2 - 2, my + 3, 4, mh * 0.20), 2, 2);
        cp.setPen(QPen(QColor("#4a4a60"), 0.6));
        cp.drawLine(mx + mw/2, my, mx + mw/2, splitY);
        cp.save();
        cp.setClipPath(bodyPath);
        cp.fillRect(QRectF(mx, my, mw/2, splitY - my), QColor("#4a4a60"));
        cp.fillRect(QRectF(mx + mw/2, my, mw/2, splitY - my), QColor("#3a3a50"));
        cp.fillRect(QRectF(mx, splitY, mw, mh - (splitY - my)), QColor("#3a3a50"));
        cp.restore();
        cp.setPen(QPen(QColor("#4a4a60"), 0.8));
        cp.setBrush(Qt::NoBrush);
        cp.drawPath(bodyPath);
        // Label
        cp.setPen(QColor("#cccccc"));
        QFont lf = cp.font();
        lf.setPixelSize(9);
        cp.setFont(lf);
        cp.drawText(QRect(0, 42, 80, 12), Qt::AlignCenter, "Standard");
    }
    m_mousePreviewBtn->setIcon(QIcon(mPx));
    m_mousePreviewBtn->setIconSize(QSize(80, 56));
    m_mousePreviewBtn->setStyleSheet("QPushButton { border: 2px solid #333; border-radius: 6px; padding: 0; background: #1c1c1c; }"
                                      "QPushButton:hover { border: 2px solid #555; background: #2a2a2a; }");
    mouseForm->addRow(m_mousePreviewBtn);

    m_showMouseToggle = new ToggleSwitch;
    mouseForm->addRow("Enable:", m_showMouseToggle);

    // Per-button show + reset toggles (Click# always shown when Show is on)
    auto addMouseButtonRow = [&](const QString &label, ToggleSwitch *&showSw, ToggleSwitch *&resetSw) {
        showSw = new ToggleSwitch;
        resetSw = new ToggleSwitch;

        auto *showCol = new QVBoxLayout;
        showCol->setSpacing(2);
        showCol->setContentsMargins(0, 0, 0, 0);
        auto *showLabel = new QLabel("Show");
        showLabel->setStyleSheet("font-size: 9px;");
        showLabel->setAlignment(Qt::AlignCenter);
        showCol->addWidget(showSw, 0, Qt::AlignCenter);
        showCol->addWidget(showLabel);

        auto *resetCol = new QVBoxLayout;
        resetCol->setSpacing(2);
        resetCol->setContentsMargins(0, 0, 0, 0);
        auto *resetLabel = new QLabel("Reset");
        resetLabel->setStyleSheet("font-size: 9px;");
        resetLabel->setAlignment(Qt::AlignCenter);
        resetCol->addWidget(resetSw, 0, Qt::AlignCenter);
        resetCol->addWidget(resetLabel);

        auto *row = new QHBoxLayout;
        row->setSpacing(12);
        row->addLayout(showCol);
        row->addLayout(resetCol);
        row->addStretch();
        mouseForm->addRow(label, row);
    };

    addMouseButtonRow("Left button (LMB)", m_mouseLmbShow, m_mouseLmbReset);
    addMouseButtonRow("Right button (RMB)", m_mouseRmbShow, m_mouseRmbReset);
    addMouseButtonRow("Middle button (MMB)", m_mouseMmbShow, m_mouseMmbReset);
    addMouseButtonRow("Side button 4 (M4)", m_mouseM4Show, m_mouseM4Reset);
    addMouseButtonRow("Side button 5 (M5)", m_mouseM5Show, m_mouseM5Reset);

    m_mouseScrollToggle = new ToggleSwitch;
    mouseForm->addRow("Scroll wheel flash:", m_mouseScrollToggle);

    m_mouseOffsetX = new QSpinBox;
    m_mouseOffsetX->setRange(0, 500);
    m_mouseOffsetX->setValue(0);
    m_mouseOffsetY = new QSpinBox;
    m_mouseOffsetY->setRange(0, 500);
    m_mouseOffsetY->setValue(0);
    auto *mOffRow = new QHBoxLayout;
    mOffRow->setSpacing(4);
    mOffRow->addWidget(m_mouseOffsetX);
    mOffRow->addWidget(new QLabel("Y:"));
    mOffRow->addWidget(m_mouseOffsetY);
    mouseForm->addRow("Offset:", mOffRow);

    m_mouseLockToggle = new ToggleSwitch;
    mouseForm->addRow("Lock:", m_mouseLockToggle);

    layout->addWidget(mouseGroup);

    auto *obsGroup = new QGroupBox("OBS BROWSER SOURCE");
    auto *obsForm = new QFormLayout(obsGroup);
    obsForm->setSpacing(4);
    obsForm->setContentsMargins(12, 16, 12, 8);
    obsForm->setLabelAlignment(Qt::AlignRight);

    m_obsToggle = new ToggleSwitch;
    obsForm->addRow("Enable:", m_obsToggle);

    // Port
    m_obsPortSpin = new QSpinBox;
    m_obsPortSpin->setRange(1024, 65535);
    m_obsPortSpin->setValue(9250);
    obsForm->addRow("Port:", m_obsPortSpin);

    auto *obsInfoLabel = new QLabel(
        "In OBS, add a Browser source and paste a URL below. Set the source size\n"
        "to about 600 wide by 500 tall (adjust to fit your layout), and leave the\n"
        "background transparent. The overlay reconnects on its own if ZKey restarts."
    );
    obsInfoLabel->setStyleSheet("font-size: 11px; padding: 4px;");
    obsInfoLabel->setWordWrap(true);
    obsForm->addRow("", obsInfoLabel);

    // Single PC URL
    auto *localhostRow = new QHBoxLayout;
    m_obsLocalhostUrl = new QLineEdit;
    m_obsLocalhostUrl->setReadOnly(true);
    m_obsLocalhostUrl->setText("http://localhost:9250");
    m_obsCopyLocalhost = new QPushButton("Copy");
    m_obsCopyLocalhost->setFixedWidth(60);
    m_obsCopyLocalhost->setStyleSheet(
        "QPushButton { background: #1a1a2e; color: #cba6f7; border: 1px solid #cba6f7; "
        "border-radius: 6px; padding: 4px 8px; font-size: 11px; font-weight: 700; }"
        "QPushButton:hover { background: #2a1e40; border-color: #d4a5ff; }"
        "QPushButton:pressed { background: #3a2e50; }");
    localhostRow->addWidget(m_obsLocalhostUrl);
    localhostRow->addWidget(m_obsCopyLocalhost);
    obsForm->addRow("SINGLE PC:", localhostRow);

    // 2PC / LAN URL
    auto *lanRow = new QHBoxLayout;
    m_obsLanUrl = new QLineEdit;
    m_obsLanUrl->setReadOnly(true);
    m_obsLanUrl->setText("http://127.0.0.1:9250");
    m_obsCopyLan = new QPushButton("Copy");
    m_obsCopyLan->setFixedWidth(60);
    m_obsCopyLan->setStyleSheet(
        "QPushButton { background: #1a1a2e; color: #cba6f7; border: 1px solid #cba6f7; "
        "border-radius: 6px; padding: 4px 8px; font-size: 11px; font-weight: 700; }"
        "QPushButton:hover { background: #2a1e40; border-color: #d4a5ff; }"
        "QPushButton:pressed { background: #3a2e50; }");
    lanRow->addWidget(m_obsLanUrl);
    lanRow->addWidget(m_obsCopyLan);
    obsForm->addRow("2PC / LAN:", lanRow);

    // Network adapter dropdown
    m_obsAdapterCombo = new QComboBox;
    for (const QNetworkInterface &iface : QNetworkInterface::allInterfaces()) {
        if (iface.flags().testFlag(QNetworkInterface::IsUp) &&
            iface.flags().testFlag(QNetworkInterface::IsRunning) &&
            !iface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            for (const QNetworkAddressEntry &entry : iface.addressEntries()) {
                if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                    m_obsAdapterCombo->addItem(iface.humanReadableName() + " - " + entry.ip().toString(), entry.ip().toString());
                }
            }
        }
    }
    obsForm->addRow("Network adapter:", m_obsAdapterCombo);

    auto *adapterTip = new QLabel(
        "Seeing the wrong 2PC address? If your PC has more than one network\n"
        "adapter (a VPN, or a virtual adapter from WSL or a VM), pick the one\n"
        "your streaming PC is on. The 2PC / LAN URL above updates to match."
    );
    adapterTip->setStyleSheet("font-size: 11px; padding: 4px;");
    adapterTip->setWordWrap(true);
    obsForm->addRow("", adapterTip);

    // Allow on local network (firewall rule)
    auto *firewallSep = new QLabel("");
    obsForm->addRow("", firewallSep);

    m_obsAllowLanBtn = new QPushButton("Allow through firewall");
    m_obsAllowLanBtn->setStyleSheet(
        "QPushButton { background: #1a1a2e; color: #cba6f7; border: 1px solid #cba6f7; "
        "border-radius: 8px; padding: 10px 20px; font-size: 12px; font-weight: 700; }"
        "QPushButton:hover { background: #2a1e40; border-color: #d4a5ff; }"
        "QPushButton:pressed { background: #3a2e50; }");
    auto *firewallDesc = new QLabel(
        "Adds a Windows Firewall rule so a second PC can reach the overlay.\n"
        "Only needed if the overlay loads here but not on the streaming PC.\n"
        "A Windows permission prompt will appear; this is expected."
    );
    firewallDesc->setStyleSheet("font-size: 11px; padding: 4px;");
    firewallDesc->setWordWrap(true);
    obsForm->addRow("LOCAL NETWORK:", m_obsAllowLanBtn);
    obsForm->addRow("", firewallDesc);

    layout->addWidget(obsGroup);
    layout->addStretch();

    auto notify = [this]() { emit settingsChanged(); };
    auto safeApply = [this](std::function<void(OverlayWidget*)> fn) {
        OverlayWidget *w = m_ctx.activeOverlay();
        if (w) {
            fn(w);
            OverlayWidget *pair = m_ctx.overlayMgr->pairedOverlay(w);
            if (pair) fn(pair);
        }
    };

    connect(m_showKeyboardToggle, &ToggleSwitch::toggled, this, [this, notify](bool val) {
        OverlayWidget *w = m_ctx.activeOverlay();
        if (!w) { notify(); return; }
        // For Combined overlays, always apply to the keyboard half
        OverlayWidget *kbW = m_ctx.overlayMgr->pairedOverlay(w);
        if (kbW && w->property("overlayType").toString() == "Combined") {
            kbW->setShowKeyboard(val);
            kbW->setOverlayVisible(val);
        } else {
            w->setShowKeyboard(val);
            bool mouseVisible = w->showMouse();
            w->setOverlayVisible(val || mouseVisible);
        }
        notify();
    });
    connect(m_showMouseToggle, &ToggleSwitch::toggled, this, [this, notify](bool val) {
        OverlayWidget *w = m_ctx.activeOverlay();
        if (!w) { notify(); return; }
        OverlayWidget *mouseW = m_ctx.overlayMgr->pairedOverlay(w);
        if (val && !mouseW) {
            mouseW = m_ctx.overlayMgr->splitToCombined(w);
        }
        if (mouseW) {
            mouseW->setShowMouse(val);
            mouseW->setOverlayVisible(val);
        }
        notify();
    });
    connect(m_layoutCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this, notify, safeApply](int idx) {
        safeApply([idx](OverlayWidget *w) { w->setLayoutIndex(idx); });
        notify();
    });
    connect(m_keyboardLangCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this, notify](int idx) {
        if (idx == 0) { KeyboardLayoutDetector::clearOverride(); m_keyboardLanguage = -1; }
        else { KeyboardLayoutDetector::setOverride(static_cast<KeyboardLayoutDetector::Layout>(idx - 1)); m_keyboardLanguage = idx - 1; }
        notify();
    });
    connect(m_mouseLockToggle, &ToggleSwitch::toggled, this, [this, notify](bool val) {
        OverlayWidget *w = m_ctx.activeOverlay();
        if (w) { w->setLockMouseSection(val); OverlayWidget *p = m_ctx.overlayMgr->pairedOverlay(w); if (p) p->setLockMouseSection(val); }
        notify();
    });
    connect(m_mouseOffsetX, QOverload<int>::of(&QSpinBox::valueChanged), this, [this, notify]() {
        // Mouse offset only applies to the mouse overlay (the paired one if it's a Combined pair)
        OverlayWidget *w = m_ctx.activeOverlay();
        if (w) {
            OverlayWidget *mouseW = w->showMouse() ? w : m_ctx.overlayMgr->pairedOverlay(w);
            if (mouseW) mouseW->setMouseOffset(QPoint(m_mouseOffsetX->value(), m_mouseOffsetY->value()));
        }
        notify();
    });
    connect(m_mouseOffsetY, QOverload<int>::of(&QSpinBox::valueChanged), this, [this, notify]() {
        OverlayWidget *w = m_ctx.activeOverlay();
        if (w) {
            OverlayWidget *mouseW = w->showMouse() ? w : m_ctx.overlayMgr->pairedOverlay(w);
            if (mouseW) mouseW->setMouseOffset(QPoint(m_mouseOffsetX->value(), m_mouseOffsetY->value()));
        }
        notify();
    });
    connect(m_kbOffsetX, QOverload<int>::of(&QSpinBox::valueChanged), this, [this, notify]() {
        // Keyboard offset only applies to the keyboard overlay
        OverlayWidget *w = m_ctx.activeOverlay();
        if (w) {
            OverlayWidget *kbW = w->showKeyboard() ? w : m_ctx.overlayMgr->pairedOverlay(w);
            if (kbW) kbW->setKeyboardOffset(QPoint(m_kbOffsetX->value(), m_kbOffsetY->value()));
        }
        notify();
    });
    connect(m_kbOffsetY, QOverload<int>::of(&QSpinBox::valueChanged), this, [this, notify]() {
        OverlayWidget *w = m_ctx.activeOverlay();
        if (w) {
            OverlayWidget *kbW = w->showKeyboard() ? w : m_ctx.overlayMgr->pairedOverlay(w);
            if (kbW) kbW->setKeyboardOffset(QPoint(m_kbOffsetX->value(), m_kbOffsetY->value()));
        }
        notify();
    });
    connect(m_layoutSourceCombo, &QComboBox::currentIndexChanged, this, [this, notify, safeApply](int idx) {
        bool custom = (idx == 1);
        m_customLayoutCombo->setEnabled(custom);
        m_editLayoutBtn->setEnabled(custom);
        m_layoutCombo->setEnabled(!custom);
        safeApply([this, custom](OverlayWidget *w) {
            if (custom) {
                QString name = m_customLayoutCombo->currentText();
                if (!name.isEmpty() && m_ctx.layoutMgr->hasLayout(name)) {
                    w->setCustomLayout(m_ctx.layoutMgr->layout(name));
                    w->setUseCustomLayout(true);
                } else {
                    w->setUseCustomLayout(false);
                }
            } else {
                w->setUseCustomLayout(false);
                w->setLayoutIndex(m_layoutCombo->currentIndex());
            }
        }); notify();
    });
    connect(m_customLayoutCombo, &QComboBox::currentIndexChanged, this, [this, notify, safeApply](int idx) {
        Q_UNUSED(idx);
        QString name = m_customLayoutCombo->currentText();
        if (!name.isEmpty() && m_ctx.layoutMgr->hasLayout(name)) {
            CustomLayout l = m_ctx.layoutMgr->layout(name);
            safeApply([l](OverlayWidget *w) {
                w->setCustomLayout(l);
                w->setUseCustomLayout(true);
            });
            notify();
        }
    });
    // Layout editor removed — use L-Editor.exe standalone
    connect(m_obsToggle, &ToggleSwitch::toggled, this, [this](bool) { emit settingsChanged(); });
    connect(m_obsPortSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int) { refreshObsUrls(); emit obsPortChanged(); emit settingsChanged(); });
    connect(m_obsCopyLocalhost, &QPushButton::clicked, this, [this]() { QApplication::clipboard()->setText(m_obsLocalhostUrl->text()); });
    connect(m_obsCopyLan, &QPushButton::clicked, this, [this]() { QApplication::clipboard()->setText(m_obsLanUrl->text()); });
    connect(m_obsAdapterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) { refreshObsUrls(); emit settingsChanged(); });
    connect(m_obsAllowLanBtn, &QPushButton::clicked, this, [this]() {
        int port = m_obsPortSpin->value();
        QString ruleName = QString("ZKey Overlay (Port %1)").arg(port);
        // Remove old rule first (ignore errors), then add new one
        QString cmd = QString(
            "netsh advfirewall firewall delete rule name=\"%1\" >nul 2>&1 & "
            "netsh advfirewall firewall add rule name=\"%1\" dir=in action=allow protocol=tcp localport=%2"
        ).arg(ruleName).arg(port);
        // Use ShellExecuteW with 'runas' to get UAC elevation
#ifdef Q_OS_WIN
        SHELLEXECUTEINFOW sei = { sizeof(sei) };
        sei.lpVerb = L"runas";
        sei.lpFile = L"cmd.exe";
        QString args = QString("/c %1").arg(cmd);
        wchar_t argsBuf[512];
        args.toWCharArray(argsBuf);
        argsBuf[args.size()] = 0;
        sei.lpParameters = argsBuf;
        sei.nShow = SW_HIDE;
        sei.fMask = SEE_MASK_NOCLOSEPROCESS;
        if (ShellExecuteExW(&sei)) {
            QMessageBox::information(this, "Firewall Rule Added",
                QString("A firewall rule has been added to allow incoming TCP connections on port %1.\n\n"
                        "Your streaming PC can now connect to the 2PC / LAN URL.").arg(port));
        } else {
            DWORD err = GetLastError();
            if (err != ERROR_CANCELLED) {
                QMessageBox::warning(this, "Firewall Rule Failed",
                    "Could not add firewall rule. Make sure you approved the UAC prompt.");
            }
        }
#endif
    });

    // Per-button mouse toggle connections
    auto connectMouseBtn = [this, safeApply](ToggleSwitch *showSw, const QString &btn) {
        connect(showSw, &ToggleSwitch::toggled, this, [safeApply, btn](bool val) {
            safeApply([btn, val](OverlayWidget *w) { w->setMouseButtonShow(btn, val); });
        });
    };
    connectMouseBtn(m_mouseLmbShow, "lmb");
    connectMouseBtn(m_mouseRmbShow, "rmb");
    connectMouseBtn(m_mouseMmbShow, "mmb");
    connectMouseBtn(m_mouseM4Show, "m4");
    connectMouseBtn(m_mouseM5Show, "m5");
    // Reset toggles — ON = decay mode (rate counter like KPS), OFF = total count
    auto connectResetBtn = [&](ToggleSwitch *resetSw, const QString &btn) {
        connect(resetSw, &ToggleSwitch::toggled, this, [safeApply, btn](bool val) {
            safeApply([btn, val](OverlayWidget *w) { w->setMouseButtonReset(btn, val); });
        });
    };
    connectResetBtn(m_mouseLmbReset, "lmb");
    connectResetBtn(m_mouseRmbReset, "rmb");
    connectResetBtn(m_mouseMmbReset, "mmb");
    connectResetBtn(m_mouseM4Reset, "m4");
    connectResetBtn(m_mouseM5Reset, "m5");
    connect(m_mouseScrollToggle, &ToggleSwitch::toggled, this, [safeApply](bool val) {
        safeApply([val](OverlayWidget *w) { w->setMouseScroll(val); });
    });
    connect(m_kpsToggle, &ToggleSwitch::toggled, this, [this, notify](bool val) {
        OverlayWidget *w = m_ctx.activeOverlay();
        if (w) {
            OverlayWidget *kbW = m_ctx.overlayMgr->pairedOverlay(w);
            if (kbW && kbW->showKeyboard()) kbW->setShowKps(val);
            else w->setShowKps(val);
        }
        notify();
    });
    // KPS Reset — ON = decay mode (rate counter), OFF = frozen (total count)
    connect(m_kpsResetToggle, &ToggleSwitch::toggled, this, [this, notify](bool val) {
        OverlayWidget *w = m_ctx.activeOverlay();
        if (w) {
            OverlayWidget *kbW = m_ctx.overlayMgr->pairedOverlay(w);
            if (kbW && kbW->showKeyboard()) kbW->setKpsResetMode(val);
            else w->setKpsResetMode(val);
        }
        notify();
    });

    // Refresh custom layout combo when layouts change
    if (m_ctx.layoutMgr) {
        connect(m_ctx.layoutMgr, &LayoutManager::layoutsChanged, this, [this]() {
            refreshCustomLayoutList();
        });
    }

    scrollArea->setWidget(page);
    auto *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->addWidget(scrollArea);
}

void SettingsPage_General::syncToOverlay(OverlayWidget *w)
{
    if (!w) return;
    m_layoutCombo->blockSignals(true);
    m_showKeyboardToggle->blockSignals(true);
    m_showMouseToggle->blockSignals(true);
    m_mouseLockToggle->blockSignals(true);
    m_layoutSourceCombo->blockSignals(true);
    m_customLayoutCombo->blockSignals(true);
    m_obsToggle->blockSignals(true);
    m_mouseLmbShow->blockSignals(true);
    m_mouseRmbShow->blockSignals(true);
    m_mouseMmbShow->blockSignals(true);
    m_mouseM4Show->blockSignals(true);
    m_mouseM5Show->blockSignals(true);
    m_mouseScrollToggle->blockSignals(true);

    m_layoutCombo->setCurrentIndex(w->layoutIndex());

    bool hasKb = false, hasMouse = false;
    if (m_ctx.overlayMgr) {
        for (auto *ow : m_ctx.overlayMgr->allOverlays()) {
            QString t = ow->property("overlayType").toString();
            if ((t == "Keyboard" || t == "Combined" || t == "Custom") && ow->showKeyboard() && ow->isVisible()) hasKb = true;
            if ((t == "Mouse" || t == "Combined" || t == "Custom") && ow->showMouse() && ow->isVisible()) hasMouse = true;
        }
    }
    m_showKeyboardToggle->setChecked(hasKb);
    m_showMouseToggle->setChecked(hasMouse);

    m_mouseLockToggle->setChecked(w->isMouseSectionLocked());
    m_mouseOffsetX->setValue(w->mouseOffset().x());
    m_mouseOffsetY->setValue(w->mouseOffset().y());
    m_kbOffsetX->setValue(w->keyboardOffset().x());
    m_kbOffsetY->setValue(w->keyboardOffset().y());

    // Per-button mouse settings
    m_mouseLmbShow->blockSignals(true);
    m_mouseLmbShow->setChecked(w->mouseButtonShow("lmb"));
    m_mouseLmbShow->blockSignals(false);
    m_mouseLmbReset->blockSignals(true);
    m_mouseLmbReset->setChecked(w->mouseButtonReset("lmb"));
    m_mouseLmbReset->blockSignals(false);
    m_mouseRmbShow->blockSignals(true);
    m_mouseRmbShow->setChecked(w->mouseButtonShow("rmb"));
    m_mouseRmbShow->blockSignals(false);
    m_mouseRmbReset->blockSignals(true);
    m_mouseRmbReset->setChecked(w->mouseButtonReset("rmb"));
    m_mouseRmbReset->blockSignals(false);
    m_mouseMmbShow->blockSignals(true);
    m_mouseMmbShow->setChecked(w->mouseButtonShow("mmb"));
    m_mouseMmbShow->blockSignals(false);
    m_mouseMmbReset->blockSignals(true);
    m_mouseMmbReset->setChecked(w->mouseButtonReset("mmb"));
    m_mouseMmbReset->blockSignals(false);
    m_mouseM4Show->blockSignals(true);
    m_mouseM4Show->setChecked(w->mouseButtonShow("m4"));
    m_mouseM4Show->blockSignals(false);
    m_mouseM4Reset->blockSignals(true);
    m_mouseM4Reset->setChecked(w->mouseButtonReset("m4"));
    m_mouseM4Reset->blockSignals(false);
    m_mouseM5Show->blockSignals(true);
    m_mouseM5Show->setChecked(w->mouseButtonShow("m5"));
    m_mouseM5Show->blockSignals(false);
    m_mouseM5Reset->blockSignals(true);
    m_mouseM5Reset->setChecked(w->mouseButtonReset("m5"));
    m_mouseM5Reset->blockSignals(false);
    m_mouseScrollToggle->setChecked(w->mouseScroll());

    m_kpsToggle->blockSignals(true);
    m_kpsToggle->setChecked(w->showKps());
    m_kpsToggle->blockSignals(false);
    m_kpsResetToggle->blockSignals(true);
    m_kpsResetToggle->setChecked(w->kpsResetMode());
    m_kpsResetToggle->blockSignals(false);

    refreshCustomLayoutList();
    bool isC = w->useCustomLayout();
    m_layoutSourceCombo->setCurrentIndex(isC ? 1 : 0);
    m_customLayoutCombo->setEnabled(isC);
    m_editLayoutBtn->setEnabled(isC);
    m_layoutCombo->setEnabled(!isC);
    if (isC) { int ci = m_customLayoutCombo->findText(w->customLayout().name); if (ci >= 0) m_customLayoutCombo->setCurrentIndex(ci); }

    m_layoutCombo->blockSignals(false);
    m_showKeyboardToggle->blockSignals(false);
    m_showMouseToggle->blockSignals(false);
    m_mouseLockToggle->blockSignals(false);
    m_layoutSourceCombo->blockSignals(false);
    m_customLayoutCombo->blockSignals(false);
    m_obsToggle->blockSignals(false);
    m_mouseLmbShow->blockSignals(false);
    m_mouseLmbReset->blockSignals(false);
    m_mouseRmbShow->blockSignals(false);
    m_mouseRmbReset->blockSignals(false);
    m_mouseMmbShow->blockSignals(false);
    m_mouseMmbReset->blockSignals(false);
    m_mouseM4Show->blockSignals(false);
    m_mouseM4Reset->blockSignals(false);
    m_mouseM5Show->blockSignals(false);
    m_mouseM5Reset->blockSignals(false);
    m_mouseScrollToggle->blockSignals(false);
}

void SettingsPage_General::loadFromConfig()
{
}

int SettingsPage_General::layoutIndex() const { return m_layoutCombo->currentIndex(); }
int SettingsPage_General::keyboardLanguage() const { return m_keyboardLanguage; }
bool SettingsPage_General::showKeyboard() const { return m_showKeyboardToggle->isChecked(); }
bool SettingsPage_General::showMouse() const { return m_showMouseToggle->isChecked(); }
QPoint SettingsPage_General::mouseOffset() const { return QPoint(m_mouseOffsetX->value(), m_mouseOffsetY->value()); }
QPoint SettingsPage_General::keyboardOffset() const { return QPoint(m_kbOffsetX->value(), m_kbOffsetY->value()); }
bool SettingsPage_General::useCustomLayout() const { return m_layoutSourceCombo->currentIndex() == 1; }
QString SettingsPage_General::customLayoutName() const { return m_customLayoutCombo->currentText(); }
void SettingsPage_General::refreshCustomLayoutList() {
    m_customLayoutCombo->blockSignals(true);
    m_customLayoutCombo->clear();
    if (m_ctx.layoutMgr) m_customLayoutCombo->addItems(m_ctx.layoutMgr->layoutNames());
    m_customLayoutCombo->blockSignals(false);
}

bool SettingsPage_General::obsEnabled() const { return m_obsToggle->isChecked(); }
int SettingsPage_General::obsPort() const { return m_obsPortSpin->value(); }
QString SettingsPage_General::selectedLanAddress() const { return m_obsAdapterCombo->currentData().toString(); }

void SettingsPage_General::refreshObsUrls()
{
    int port = m_obsPortSpin->value();
    m_obsLocalhostUrl->setText(QString("http://localhost:%1").arg(port));

    QString lanAddr = m_obsAdapterCombo->currentData().toString();
    if (lanAddr.isEmpty()) lanAddr = "127.0.0.1";
    m_obsLanUrl->setText(QString("http://%1:%2").arg(lanAddr).arg(port));
}
