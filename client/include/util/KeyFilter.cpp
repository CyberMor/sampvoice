/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "KeyFilter.h"

#include "Logger.h"

bool KeyFilter::AddKey(const BYTE keyId) noexcept
{
    if (KeyFilter::statusKeys[keyId])
        return false;

    Logger::LogToFile("[dbg:keyfilter] : adding key (id:0x%hhx)...", keyId);

    KeyFilter::statusKeys[keyId] = true;

    return true;
}

bool KeyFilter::RemoveKey(const BYTE keyId) noexcept
{
    KeyFilter::ReleaseKey(keyId);

    if (!KeyFilter::statusKeys[keyId])
        return false;

    Logger::LogToFile("[dbg:keyfilter] : removing key (id:0x%hhx)...", keyId);

    KeyFilter::statusKeys[keyId] = false;

    return true;
}

void KeyFilter::RemoveAllKeys() noexcept
{
    Logger::LogToFile("[dbg:keyfilter] : removing all keys...");

    for (DWORD keyId = 0; keyId < sizeof(KeyFilter::statusKeys); ++keyId)
        KeyFilter::RemoveKey(keyId);

    KeyFilter::activeKeys = 0;
}

void KeyFilter::ReleaseAllKeys() noexcept
{
    Logger::LogToFile("[dbg:keyfilter] : releasing all keys...");

    for (DWORD keyId = 0; keyId < sizeof(KeyFilter::statusKeys); ++keyId)
        KeyFilter::ReleaseKey(keyId);

    KeyFilter::activeKeys = 0;
}

bool KeyFilter::PopKey(KeyEvent& const event) noexcept
{
    if (KeyFilter::keyQueue.empty())
        return false;

    event = std::move(*KeyFilter::keyQueue.front());
    KeyFilter::keyQueue.pop();

    return true;
}

void KeyFilter::OnWndMessage(HWND hWnd, UINT uMsg,
                             WPARAM wParam, LPARAM lParam) noexcept
{
    switch (uMsg)
    {
        case WM_KEYDOWN: KeyFilter::PressKey(wParam); break;
        case WM_KEYUP: KeyFilter::ReleaseKey(wParam); break;
    }
}

bool KeyFilter::PressKey(const BYTE keyId) noexcept
{
    if (!KeyFilter::statusKeys[keyId])
        return false;

    if (KeyFilter::pressedKeys[keyId])
        return false;

    KeyFilter::pressedKeys[keyId] = KeyFilter::keyQueue
        .try_emplace(keyId, true, KeyFilter::activeKeys + 1);

    if (KeyFilter::pressedKeys[keyId])
        ++KeyFilter::activeKeys;

    return KeyFilter::pressedKeys[keyId];
}

bool KeyFilter::ReleaseKey(const BYTE keyId) noexcept
{
    if (!KeyFilter::pressedKeys[keyId])
        return false;

    KeyFilter::pressedKeys[keyId] = !KeyFilter::keyQueue
        .try_emplace(keyId, false, KeyFilter::activeKeys - 1);

    if (!KeyFilter::pressedKeys[keyId])
        --KeyFilter::activeKeys;

    return !KeyFilter::pressedKeys[keyId];
}

SPSCQueue<KeyEvent> KeyFilter::keyQueue { 256 };

int KeyFilter::activeKeys { 0 };

bool KeyFilter::pressedKeys[256] {};
bool KeyFilter::statusKeys[256] {};
