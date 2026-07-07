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

#include "inputmanager.h"
#include <QDebug>
#include <QApplication>

InputManager *InputManager::s_instance = nullptr;

InputManager::InputManager(QObject *parent)
    : QObject(parent)
{
    s_instance = this;
}

InputManager::~InputManager()
{
    stop();
}

// --- Hidden window to receive WM_INPUT messages ---

LRESULT CALLBACK InputManager::rawInputWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_INPUT && s_instance) {
        UINT size = 0;
        // Get size of raw input data
        if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER)) == 0 && size > 0) {
            RAWINPUT *raw = static_cast<RAWINPUT *>(malloc(size));
            if (raw && GetRawInputData((HRAWINPUT)lParam, RID_INPUT, raw, &size, sizeof(RAWINPUTHEADER)) == size) {
                s_instance->processRawInput(raw);
            }
            free(raw);
        }
        // Mark message as processed (prevent further processing)
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

unsigned __stdcall InputManager::rawInputThreadProc(void *arg)
{
    auto *self = static_cast<InputManager *>(arg);

    // Register window class
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = rawInputWndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = L"ZKeyRawInput";
    RegisterClassEx(&wc);

    // Create hidden message-only window
    self->m_hwnd = CreateWindowEx(
        0, L"ZKeyRawInput", L"ZKey Raw Input",
        0, 0, 0, 0, 0,
        HWND_MESSAGE, nullptr, GetModuleHandle(nullptr), nullptr);

    if (!self->m_hwnd) {
        qWarning("Failed to create raw input window");
        return 1;
    }

    self->m_hwndThreadId = GetCurrentThreadId();

    // Register raw input devices: keyboard + mouse with RIDEV_INPUTSINK
    RAWINPUTDEVICE rid[2];

    // Keyboard
    rid[0].usUsagePage = 0x01;              // HID_USAGE_PAGE_GENERIC
    rid[0].usUsage     = 0x06;              // HID_USAGE_GENERIC_KEYBOARD
    rid[0].dwFlags     = RIDEV_INPUTSINK;   // Receive even when not focused
    rid[0].hwndTarget  = self->m_hwnd;

    // Mouse
    rid[1].usUsagePage = 0x01;              // HID_USAGE_PAGE_GENERIC
    rid[1].usUsage     = 0x02;              // HID_USAGE_GENERIC_MOUSE
    rid[1].dwFlags     = RIDEV_INPUTSINK;   // Receive even when not focused
    rid[1].hwndTarget  = self->m_hwnd;

    if (!RegisterRawInputDevices(rid, 2, sizeof(RAWINPUTDEVICE))) {
        qWarning("Failed to register raw input devices");
        DestroyWindow(self->m_hwnd);
        self->m_hwnd = nullptr;
        return 1;
    }

    // Message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    DestroyWindow(self->m_hwnd);
    self->m_hwnd = nullptr;
    return 0;
}

// --- Raw Input processing ---

void InputManager::processRawInput(const RAWINPUT *raw)
{
    if (raw->header.dwType == RIM_TYPEKEYBOARD) {
        const RAWKEYBOARD &kb = raw->data.keyboard;

        // VKey from raw input (already a virtual key code)
        UINT vk = kb.VKey;
        // Flags: RI_KEY_BREAK (0x01) = key up
        bool pressed = !(kb.Flags & RI_KEY_BREAK);

        // Ignore injected/transition keys (e.g., menu key release after Alt+Tab)
        if (kb.Flags & RI_KEY_E0) {
            // E0 prefix keys: handle extended keys
            switch (vk) {
            case VK_CONTROL: vk = VK_RCONTROL; break;
            case VK_MENU:    vk = VK_RMENU;    break;
            case VK_SHIFT:   vk = VK_RSHIFT;   break;
            // Numpad Enter, Insert, Delete, Home, End, Page Up/Down, Arrows
            // These already have correct VK codes from raw input
            }
        }

        setKeyState(static_cast<int>(vk), pressed);
    }
    else if (raw->header.dwType == RIM_TYPEMOUSE) {
        const RAWMOUSE &ms = raw->data.mouse;

        // Button states (RI_MOUSE_* flags)
        if (ms.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
            setMouseButtonState(0x01, true);
        if (ms.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)
            setMouseButtonState(0x01, false);
        if (ms.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
            setMouseButtonState(0x02, true);
        if (ms.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
            setMouseButtonState(0x02, false);
        if (ms.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
            setMouseButtonState(0x04, true);
        if (ms.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP)
            setMouseButtonState(0x04, false);
        if (ms.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN)
            setMouseButtonState(0x05, true);
        if (ms.usButtonFlags & RI_MOUSE_BUTTON_4_UP)
            setMouseButtonState(0x05, false);
        if (ms.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN)
            setMouseButtonState(0x06, true);
        if (ms.usButtonFlags & RI_MOUSE_BUTTON_5_UP)
            setMouseButtonState(0x06, false);

        // Wheel
        if (ms.usButtonFlags & RI_MOUSE_WHEEL) {
            short delta = static_cast<short>(ms.usButtonData);
            {
                QMutexLocker lock(&m_mutex);
                m_mouseWheel = delta;
            }
            emit mouseWheelScrolled(delta);
        }

        // Movement (raw input provides relative movement)
        if (ms.lLastX != 0 || ms.lLastY != 0) {
            double dx = static_cast<double>(ms.lLastX);
            double dy = static_cast<double>(ms.lLastY);
            {
                QMutexLocker lock(&m_mutex);
                m_mouseDeltaX = dx;
                m_mouseDeltaY = dy;
            }
            emit mouseMoved(dx, dy);
        }
    }
}

// --- Start/Stop ---

void InputManager::start()
{
    if (m_threadHandle)
        return;

    m_running = true;
    m_threadHandle = reinterpret_cast<HANDLE>(
        _beginthreadex(nullptr, 0, &rawInputThreadProc, this, 0,
                       reinterpret_cast<unsigned *>(&m_hwndThreadId)));
}

void InputManager::stop()
{
    m_running = false;
    if (m_hwndThreadId != 0) {
        PostThreadMessage(m_hwndThreadId, WM_QUIT, 0, 0);
        m_hwndThreadId = 0;
    }
    if (m_threadHandle) {
        WaitForSingleObject(m_threadHandle, 2000);
        CloseHandle(m_threadHandle);
        m_threadHandle = nullptr;
    }
}

// --- Query methods ---

QVector<int> InputManager::getPressedKeysOrdered() const
{
    QMutexLocker lock(&m_mutex);
    return m_pressOrder;
}

QSet<int> InputManager::pressedKeys() const
{
    QMutexLocker lock(&m_mutex);
    return m_pressedKeys;
}

bool InputManager::isKeyDown(int vkCode) const
{
    QMutexLocker lock(&m_mutex);
    return m_pressedKeys.contains(vkCode);
}

bool InputManager::isMouseButtonDown(int vkCode) const
{
    QMutexLocker lock(&m_mutex);
    return m_pressedKeys.contains(vkCode);
}

int InputManager::mouseWheelDelta() const
{
    QMutexLocker lock(&m_mutex);
    return m_mouseWheel;
}

void InputManager::clearMouseWheelDelta()
{
    QMutexLocker lock(&m_mutex);
    m_mouseWheel = 0;
}

QPointF InputManager::mouseDelta() const
{
    QMutexLocker lock(&m_mutex);
    return QPointF(m_mouseDeltaX, m_mouseDeltaY);
}

void InputManager::clearMouseDelta()
{
    QMutexLocker lock(&m_mutex);
    m_mouseDeltaX = 0;
    m_mouseDeltaY = 0;
}

// --- State management ---

void InputManager::setKeyState(int vkCode, bool pressed)
{
    {
        QMutexLocker lock(&m_mutex);
        if (pressed) {
            if (!m_pressedKeys.contains(vkCode))
                m_pressOrder.prepend(vkCode);
            m_pressedKeys.insert(vkCode);
            // Normalize: left/right modifier -> also store generic code
            if (vkCode == 0xA0 || vkCode == 0xA1) m_pressedKeys.insert(0x10);
            if (vkCode == 0xA2 || vkCode == 0xA3) m_pressedKeys.insert(0x11);
            if (vkCode == 0xA4 || vkCode == 0xA5) m_pressedKeys.insert(0x12);
        } else {
            m_pressedKeys.remove(vkCode);
            m_pressOrder.removeAll(vkCode);
            // Normalize: only remove generic code if neither side is still held
            if (vkCode == 0xA0 || vkCode == 0xA1)
                if (!m_pressedKeys.contains(0xA0) && !m_pressedKeys.contains(0xA1))
                    m_pressedKeys.remove(0x10);
            if (vkCode == 0xA2 || vkCode == 0xA3)
                if (!m_pressedKeys.contains(0xA2) && !m_pressedKeys.contains(0xA3))
                    m_pressedKeys.remove(0x11);
            if (vkCode == 0xA4 || vkCode == 0xA5)
                if (!m_pressedKeys.contains(0xA4) && !m_pressedKeys.contains(0xA5))
                    m_pressedKeys.remove(0x12);
        }
    }
    emit keyChanged(vkCode, pressed);
}

void InputManager::setMouseButtonState(int button, bool pressed)
{
    {
        QMutexLocker lock(&m_mutex);
        if (pressed) {
            if (!m_pressedKeys.contains(button))
                m_pressOrder.prepend(button);
            m_pressedKeys.insert(button);
        } else {
            m_pressedKeys.remove(button);
            m_pressOrder.removeAll(button);
        }
    }
    emit mouseChanged(button, pressed);
}
