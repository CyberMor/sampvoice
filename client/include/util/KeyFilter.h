/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <Windows.h>

#include <SPSCQueue.h>

struct KeyEvent {

    KeyEvent() noexcept = default;
    KeyEvent(const KeyEvent&) noexcept = default;
    KeyEvent(KeyEvent&&) noexcept = default;
    KeyEvent& operator=(const KeyEvent&) noexcept = default;
    KeyEvent& operator=(KeyEvent&&) noexcept = default;

public:

    KeyEvent(BYTE keyId, bool isPressed, int actKeys) noexcept
        : keyId(keyId), isPressed(isPressed), actKeys(actKeys) {}

    ~KeyEvent() noexcept = default;

public:

    BYTE keyId { NULL };
    bool isPressed { false };
    int actKeys { 0 };

};

class KeyFilter {

    KeyFilter() = delete;
    ~KeyFilter() = delete;
    KeyFilter(const KeyFilter&) = delete;
    KeyFilter(KeyFilter&&) = delete;
    KeyFilter& operator=(const KeyFilter&) = delete;
    KeyFilter& operator=(KeyFilter&&) = delete;

public:

    static bool AddKey(BYTE keyId) noexcept;
    static bool RemoveKey(BYTE keyId) noexcept;

    static void RemoveAllKeys() noexcept;
    static void ReleaseAllKeys() noexcept;

    static bool PopKey(KeyEvent& event) noexcept;

    static void OnWndMessage(HWND hWnd, UINT uMsg,
                             WPARAM wParam, LPARAM lParam) noexcept;

private:

    static bool PressKey(BYTE keyId) noexcept;
    static bool ReleaseKey(BYTE keyId) noexcept;

private:

    static SPSCQueue<KeyEvent> keyQueue;

    static int activeKeys;

    static bool pressedKeys[256];
    static bool statusKeys[256];

};
