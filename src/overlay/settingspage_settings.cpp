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

#include "settingspage_settings.h"
#include "toggleswitch.h"
#include "overlaywidget.h"
#include "overlaymanager.h"
#include "settingswindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QDesktopServices>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

SettingsPage_Settings::SettingsPage_Settings(SettingsContext &ctx, QWidget *parent)
    : SettingsPage(ctx, parent)
{
    auto *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("QScrollArea { background: transparent; }");

    auto *page = new QWidget;
    page->setStyleSheet("background: transparent;");
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(16, 12, 16, 12);
    layout->setSpacing(10);

    // ── App Info ──
    auto *infoGroup = new QGroupBox("APP INFO");
    auto *infoForm = new QFormLayout(infoGroup);
    infoForm->setSpacing(6);
    infoForm->setContentsMargins(12, 16, 12, 8);
    infoForm->setLabelAlignment(Qt::AlignRight);

    m_versionLabel = new QLabel("1.0.0");
    infoForm->addRow("Version:", m_versionLabel);

    layout->addWidget(infoGroup);

    // ── Hotkeys ──
    auto *hkGroup = new QGroupBox("HOTKEYS");
    auto *hkLayout = new QVBoxLayout(hkGroup);
    hkLayout->setSpacing(6);
    hkLayout->setContentsMargins(12, 16, 12, 8);

    m_hotkeysEnabledToggle = new ToggleSwitch;
    auto *hkEnabledRow = new QHBoxLayout;
    hkEnabledRow->addWidget(new QLabel("Enable Hotkeys:"));
    hkEnabledRow->addWidget(m_hotkeysEnabledToggle);
    hkEnabledRow->addStretch();
    hkLayout->addLayout(hkEnabledRow);

    auto *sep = new QFrame;
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("max-height: 1px;");
    hkLayout->addWidget(sep);

    struct HkDef { const char *label; };
    HkDef hkDefs[] = {
        {"Toggle Visibility"},
        {"Toggle Auto-Hide"},
        {"Toggle Move"},
        {"Open Preferences"},
        {"Increase Opacity"},
        {"Decrease Opacity"},
    };
    for (int i = 0; i < 6; i++) {
        auto *row = new QHBoxLayout;
        row->setSpacing(12);
        auto *lbl = new QLabel(hkDefs[i].label);
        lbl->setFixedWidth(130);
        lbl->setStyleSheet("font-size: 12px;");
        auto *enabledToggle = new ToggleSwitch;
        enabledToggle->setFixedSize(44, 24);
        auto *keyLbl = new QLabel("None");
        keyLbl->setFixedWidth(110);
        keyLbl->setAlignment(Qt::AlignCenter);
        keyLbl->setStyleSheet("font-size: 11px; border: 1px solid; border-radius: 4px; padding: 2px 8px;");
        auto *changeBtn = new QPushButton("Change");
        changeBtn->setFixedHeight(26);
        changeBtn->setFixedWidth(70);
        changeBtn->setStyleSheet(
            "QPushButton { background: #1a1a2e; color: #cba6f7; border: 1px solid #3a3a5a; "
            "border-radius: 5px; padding: 4px 10px; font-size: 11px; font-weight: 600; }"
            "QPushButton:hover { background: #2a1e40; border-color: #cba6f7; }"
            "QPushButton:pressed { background: #3a2e50; }");
        m_hkRows[i].enabled = enabledToggle;
        m_hkRows[i].changeBtn = changeBtn;
        m_hkRows[i].keyLabel = keyLbl;
        row->addWidget(lbl);
        row->addWidget(enabledToggle);
        row->addStretch();
        row->addWidget(keyLbl);
        row->addWidget(changeBtn);
        hkLayout->addLayout(row);
    }
    layout->addWidget(hkGroup);

    // ── Updates ──
    auto *updateGroup = new QGroupBox("UPDATES");
    auto *updateLayout = new QVBoxLayout(updateGroup);
    updateLayout->setSpacing(6);
    updateLayout->setContentsMargins(12, 16, 12, 8);

    m_checkUpdateBtn = new QPushButton("Check for Updates");
    m_checkUpdateBtn->setFixedHeight(32);
    m_checkUpdateBtn->setStyleSheet(
        "QPushButton { background: #1a1a2e; color: #89b4fa; border: 1px solid #3a3a5a; "
        "border-radius: 5px; padding: 6px 16px; font-size: 12px; font-weight: 600; }"
        "QPushButton:hover { background: #1e2a40; border-color: #89b4fa; }"
        "QPushButton:pressed { background: #2a3a50; }");
    updateLayout->addWidget(m_checkUpdateBtn);

    layout->addWidget(updateGroup);

    // ── Feedback ──
    auto *feedbackGroup = new QGroupBox("FEEDBACK");
    auto *feedbackLayout = new QVBoxLayout(feedbackGroup);
    feedbackLayout->setSpacing(6);
    feedbackLayout->setContentsMargins(12, 16, 12, 8);

    auto *feedbackBtn = new QPushButton("Report Issue / Send Feedback");
    feedbackBtn->setFixedHeight(32);
    feedbackBtn->setStyleSheet(
        "QPushButton { background: #1a1a2e; color: #a6e3a1; border: 1px solid #3a3a5a; "
        "border-radius: 5px; padding: 6px 16px; font-size: 12px; font-weight: 600; }"
        "QPushButton:hover { background: #1e2a30; border-color: #a6e3a1; }"
        "QPushButton:pressed { background: #2a3a40; }");
    connect(feedbackBtn, &QPushButton::clicked, this, []() {
        QDesktopServices::openUrl(QUrl("https://github.com/axs-offcl/ZKey/issues/new?title=&labels=feedback&template=bug_report.md"));
    });
    feedbackLayout->addWidget(feedbackBtn);

    layout->addWidget(feedbackGroup);

    // ── L-Editor Download ──
    auto *lEditorBtn = new QPushButton("  Get L-Editor  — Design Custom Layouts");
    lEditorBtn->setCursor(Qt::PointingHandCursor);
    lEditorBtn->setMinimumHeight(36);
    lEditorBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #1a1a1a;"
        "  color: white;"
        "  border: 1px solid white;"
        "  border-radius: 6px;"
        "  padding: 6px 16px;"
        "  font-weight: bold;"
        "  font-size: 12px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #2a2a2a;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #0a0a0a;"
        "}"
    );
    connect(lEditorBtn, &QPushButton::clicked, this, []() {
        QDesktopServices::openUrl(QUrl("https://github.com/axs-offcl/ZKey/releases"));
    });
    layout->addWidget(lEditorBtn);

    layout->addStretch();

    // ── Signal connections ──
    connect(m_hotkeysEnabledToggle, &ToggleSwitch::toggled, this, [this]() {
        emit hotkeyConfigChanged();
        emit settingsChanged();
    });

    for (int i = 0; i < 6; i++) {
        connect(m_hkRows[i].changeBtn, &QPushButton::clicked, this, [this, i]() {
            HotkeyRecorderDialog dlg(this->window());
            if (dlg.exec() == QDialog::Accepted) {
                AppConfig::Hotkey hk = dlg.recordedHotkey();
                m_hkCache[i] = hk;
                m_hkRows[i].keyLabel->setText(hotkeyToString(hk));
                emit hotkeyConfigChanged();
                emit settingsChanged();
            }
        });
        connect(m_hkRows[i].enabled, &ToggleSwitch::toggled, this, [this, i](bool val) {
            m_hkCache[i].enabled = val;
            emit hotkeyConfigChanged();
            emit settingsChanged();
        });
    }

    connect(m_checkUpdateBtn, &QPushButton::clicked, this, [this]() {
        m_checkUpdateBtn->setText("Checking...");
        m_checkUpdateBtn->setEnabled(false);

        auto *netMgr = new QNetworkAccessManager(this);
        auto *reply = netMgr->get(QNetworkRequest(QUrl("https://api.github.com/repos/axs-offcl/ZKey/releases/latest")));
        connect(reply, &QNetworkReply::finished, this, [this, reply, netMgr]() {
            m_checkUpdateBtn->setText("Check for Updates");
            m_checkUpdateBtn->setEnabled(true);
            if (reply->error() == QNetworkReply::NoError) {
                QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
                QJsonObject obj = doc.object();
                QString latestTag = obj.value("tag_name").toString();
                QString currentVersion = m_versionLabel->text();
                if (latestTag != currentVersion && !latestTag.isEmpty()) {
                    QDesktopServices::openUrl(QUrl("https://github.com/axs-offcl/ZKey/releases/latest"));
                }
            }
            reply->deleteLater();
            netMgr->deleteLater();
        });
    });

    scrollArea->setWidget(page);
    auto *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->addWidget(scrollArea);
}

void SettingsPage_Settings::syncToOverlay(OverlayWidget *) {}

void SettingsPage_Settings::loadFromConfig() {}

bool SettingsPage_Settings::hotkeysEnabled() const
{
    return m_hotkeysEnabledToggle ? m_hotkeysEnabledToggle->isChecked() : false;
}
