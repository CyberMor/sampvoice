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
    ~KeyEvent() noexcept = default;
    KeyEvent(const KeyEvent&) noexcept = default;
    KeyEvent(KeyEvent&&) noexcept = default;
    KeyEvent& operator=(const KeyEvent&) noexcept = default;
    KeyEvent& operator=(KeyEvent&&) noexcept = default;

public:

    KeyEvent(const BYTE key_id, const bool is_pressed, const int active_keys) noexcept
        : key_id      { key_id }
        , is_pressed  { is_pressed }
        , active_keys { active_keys }
    {}

public:

    BYTE key_id      = 0;
    bool is_pressed  = false;
    int  active_keys = 0;

};

struct KeyFilter {

    KeyFilter() = delete;
    ~KeyFilter() = delete;
    KeyFilter(const KeyFilter&) = delete;
    KeyFilter(KeyFilter&&) = delete;
    KeyFilter& operator=(const KeyFilter&) = delete;
    KeyFilter& operator=(KeyFilter&&) = delete;

public:

    static bool AddKey(BYTE key_id) noexcept;
    static bool RemoveKey(BYTE key_id) noexcept;

    static void RemoveAllKeys() noexcept;
    static void ReleaseAllKeys() noexcept;

    static bool PushPressEvent(BYTE key_id) noexcept;
    static bool PushReleaseEvent(BYTE key_id) noexcept;

    static bool PopEvent(KeyEvent& event) noexcept;

private:

    static SPSCQueue<KeyEvent> _key_queue;

    static std::array<bool, 256> _pressed_keys;
    static std::array<bool, 256> _status_keys;

    static int _active_keys;

};
