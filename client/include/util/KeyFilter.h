/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <array>

#include <Windows.h>

#include <SPSCQueue.h>

struct KeyEvent {

    KeyEvent() noexcept = default;
    KeyEvent(const KeyEvent&) noexcept = default;
    KeyEvent(KeyEvent&&) = delete;
    KeyEvent& operator=(const KeyEvent&) noexcept = default;
    KeyEvent& operator=(KeyEvent&&) = delete;

public:

    explicit KeyEvent(BYTE keyId, bool isPressed, int activeKeys) noexcept
        : keyId(keyId), isPressed(isPressed), activeKeys(activeKeys) {}

    ~KeyEvent() noexcept = default;

public:

    BYTE keyId { NULL };
    bool isPressed { false };
    int activeKeys { 0 };

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

    static bool PushPressEvent(BYTE keyId) noexcept;
    static bool PushReleaseEvent(BYTE keyId) noexcept;

    static bool PopEvent(KeyEvent& event) noexcept;

private:

    static SPSCQueue<KeyEvent> keyQueue;

    static std::array<bool, 256> pressedKeys;
    static std::array<bool, 256> statusKeys;

    static int activeKeys;

};
