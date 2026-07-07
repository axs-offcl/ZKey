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
#include <QObject>
#include <QSet>
#include <QVector>
#include <QMutex>
#include <atomic>
#include <process.h>
#include <windows.h>

class InputManager : public QObject {
    Q_OBJECT
public:
    explicit InputManager(QObject *parent = nullptr);
    ~InputManager() override;

    void start();
    void stop();
    bool isKeyDown(int vkCode) const;
    bool isMouseButtonDown(int vkCode) const;
    int mouseWheelDelta() const;
    void clearMouseWheelDelta();

    QPointF mouseDelta() const;
    void clearMouseDelta();

    // Public for testing
    void setKeyState(int vkCode, bool pressed);
    void setMouseButtonState(int button, bool pressed);

    // OSD press order
    QVector<int> getPressedKeysOrdered() const;
    QSet<int> pressedKeys() const;

signals:
    void keyChanged(int vkCode, bool pressed);
    void mouseChanged(int button, bool pressed);
    void mouseWheelScrolled(int delta);
    void mouseMoved(double dx, double dy);

private:
    // Raw Input hidden window
    static LRESULT CALLBACK rawInputWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static unsigned __stdcall rawInputThreadProc(void *arg);
    void processRawInput(const RAWINPUT *raw);

    HWND m_hwnd = nullptr;
    HANDLE m_threadHandle = nullptr;
    std::atomic<bool> m_running{false};
    DWORD m_hwndThreadId = 0;

    mutable QMutex m_mutex;
    QSet<int> m_pressedKeys;
    QVector<int> m_pressOrder;
    int m_mouseWheel = 0;
    double m_mouseDeltaX = 0;
    double m_mouseDeltaY = 0;

    static InputManager *s_instance;
};
