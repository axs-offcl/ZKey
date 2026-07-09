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

#pragma once
#include "settingspage.h"
#include <QComboBox>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>
#include <QLineEdit>

class ToggleSwitch;

class SettingsPage_General : public SettingsPage {
    Q_OBJECT
public:
    explicit SettingsPage_General(SettingsContext &ctx, QWidget *parent = nullptr);

    void syncToOverlay(OverlayWidget *w) override;
    void loadFromConfig() override;

    int layoutIndex() const;
    int keyboardLanguage() const;
    bool showKeyboard() const;
    bool showMouse() const;
    QPoint mouseOffset() const;
    QPoint keyboardOffset() const;
    bool useCustomLayout() const;
    QString customLayoutName() const;
    void refreshCustomLayoutList();
    bool isMouseSectionLocked() const;
    bool obsEnabled() const;
    int obsPort() const;
    QString selectedLanAddress() const;

signals:
    void obsPortChanged();

private:
    QComboBox *m_layoutCombo = nullptr;
    QList<QPushButton *> m_layoutCards;
    QComboBox *m_layoutSourceCombo = nullptr;
    QComboBox *m_customLayoutCombo = nullptr;
    QComboBox *m_keyboardLangCombo = nullptr;
    QPushButton *m_editLayoutBtn = nullptr;
    QPushButton *m_downloadLEditorBtn = nullptr;
    ToggleSwitch *m_showKeyboardToggle = nullptr;
    ToggleSwitch *m_showMouseToggle = nullptr;
    QSpinBox *m_mouseOffsetX = nullptr;
    QSpinBox *m_mouseOffsetY = nullptr;
    QSpinBox *m_kbOffsetX = nullptr;
    QSpinBox *m_kbOffsetY = nullptr;
    ToggleSwitch *m_mouseLockToggle = nullptr;
    ToggleSwitch *m_obsToggle = nullptr;
    QSpinBox *m_obsPortSpin = nullptr;
    QLineEdit *m_obsLocalhostUrl = nullptr;
    QPushButton *m_obsCopyLocalhost = nullptr;
    QLineEdit *m_obsLanUrl = nullptr;
    QPushButton *m_obsCopyLan = nullptr;
    QComboBox *m_obsAdapterCombo = nullptr;
    QPushButton *m_obsAllowLanBtn = nullptr;
    int m_keyboardLanguage = -1;

    // Per-button mouse toggles
    ToggleSwitch *m_mouseLmbShow = nullptr;
    ToggleSwitch *m_mouseLmbReset = nullptr;
    ToggleSwitch *m_mouseRmbShow = nullptr;
    ToggleSwitch *m_mouseRmbReset = nullptr;
    ToggleSwitch *m_mouseMmbShow = nullptr;
    ToggleSwitch *m_mouseMmbReset = nullptr;
    ToggleSwitch *m_mouseM4Show = nullptr;
    ToggleSwitch *m_mouseM4Reset = nullptr;
    ToggleSwitch *m_mouseM5Show = nullptr;
    ToggleSwitch *m_mouseM5Reset = nullptr;
    ToggleSwitch *m_mouseScrollToggle = nullptr;
    ToggleSwitch *m_kpsToggle = nullptr;
    ToggleSwitch *m_kpsResetToggle = nullptr;
    QPushButton *m_mousePreviewBtn = nullptr;

    void refreshObsUrls();
};
