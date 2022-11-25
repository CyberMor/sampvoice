/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "KeyFilter.h"

#include "Logger.h"

bool KeyFilter::AddKey(const BYTE key_id) noexcept
{
    if (_status_keys[key_id]) return false;

    Logger::LogToFile("[dbg:keyfilter] : adding key (id:0x%hhx)...", key_id);

    _status_keys[key_id] = true;

    return true;
}

bool KeyFilter::RemoveKey(const BYTE key_id) noexcept
{
    PushReleaseEvent(key_id);

    if (!_status_keys[key_id]) return false;

    Logger::LogToFile("[dbg:keyfilter] : removing key (id:0x%hhx)...", key_id);

    _status_keys[key_id] = false;

    return true;
}

void KeyFilter::RemoveAllKeys() noexcept
{
    Logger::LogToFile("[dbg:keyfilter] : removing all keys...");

    for (WORD key_id = 0; key_id < _status_keys.size(); ++key_id)
    {
        RemoveKey(key_id);
    }

    _active_keys = 0;
}

void KeyFilter::ReleaseAllKeys() noexcept
{
    Logger::LogToFile("[dbg:keyfilter] : releasing all keys...");

    for (WORD key_id = 0; key_id < _status_keys.size(); ++key_id)
    {
        PushReleaseEvent(key_id);
    }

    _active_keys = 0;
}

bool KeyFilter::PushPressEvent(const BYTE key_id) noexcept
{
    if (!_status_keys[key_id]) return false;
    if (_pressed_keys[key_id]) return false;

    _pressed_keys[key_id] = _key_queue.try_emplace(key_id, true, _active_keys + 1);
    if (_pressed_keys[key_id]) ++_active_keys;

    return _pressed_keys[key_id];
}

bool KeyFilter::PushReleaseEvent(const BYTE key_id) noexcept
{
    if (!_pressed_keys[key_id]) return false;

    _pressed_keys[key_id] = !_key_queue.try_emplace(key_id, false, _active_keys - 1);
    if (!_pressed_keys[key_id]) --_active_keys;

    return !_pressed_keys[key_id];
}

bool KeyFilter::PopEvent(KeyEvent& event) noexcept
{
    if (_key_queue.empty()) return false;

    event = *_key_queue.front();
    _key_queue.pop();

    return true;
}

SPSCQueue<KeyEvent> KeyFilter::_key_queue { 256 };

std::array<bool, 256> KeyFilter::_pressed_keys;
std::array<bool, 256> KeyFilter::_status_keys;

int KeyFilter::_active_keys = 0;
