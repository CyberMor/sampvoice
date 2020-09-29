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
    if (statusKeys[keyId]) return false;

    Logger::LogToFile("[dbg:keyfilter] : adding key (id:0x%hhx)...", keyId);

    statusKeys[keyId] = true;

    return true;
}

bool KeyFilter::RemoveKey(const BYTE keyId) noexcept
{
    ReleaseKey(keyId);

    if (!statusKeys[keyId]) return false;

    Logger::LogToFile("[dbg:keyfilter] : removing key (id:0x%hhx)...", keyId);

    statusKeys[keyId] = false;

    return true;
}

void KeyFilter::RemoveAllKeys() noexcept
{
    Logger::LogToFile("[dbg:keyfilter] : removing all keys...");

    for (DWORD keyId = 0; keyId < sizeof(statusKeys); ++keyId)
        RemoveKey(keyId);

    activeKeys = 0;
}

void KeyFilter::ReleaseAllKeys() noexcept
{
    Logger::LogToFile("[dbg:keyfilter] : releasing all keys...");

    for (DWORD keyId = 0; keyId < sizeof(statusKeys); ++keyId)
        ReleaseKey(keyId);

    activeKeys = 0;
}

bool KeyFilter::PopKey(KeyEvent& event) noexcept
{
    if (keyQueue.empty()) return false;

    event = *keyQueue.front();
    keyQueue.pop();

    return true;
}

void KeyFilter::OnWndMessage(HWND hWnd, UINT uMsg,
                             WPARAM wParam, LPARAM lParam) noexcept
{
    switch (uMsg)
    {
        case WM_KEYDOWN: PressKey(wParam); break;
        case WM_KEYUP: ReleaseKey(wParam); break;
    }
}

bool KeyFilter::PressKey(const BYTE keyId) noexcept
{
    if (!statusKeys[keyId]) return false;
    if (pressedKeys[keyId]) return false;

    pressedKeys[keyId] = keyQueue.try_emplace(keyId, true, activeKeys + 1);
    if (pressedKeys[keyId]) ++activeKeys;

    return pressedKeys[keyId];
}

bool KeyFilter::ReleaseKey(const BYTE keyId) noexcept
{
    if (!pressedKeys[keyId]) return false;

    pressedKeys[keyId] = !keyQueue.try_emplace(keyId, false, activeKeys - 1);
    if (!pressedKeys[keyId]) --activeKeys;

    return !pressedKeys[keyId];
}

SPSCQueue<KeyEvent> KeyFilter::keyQueue { 256 };

int KeyFilter::activeKeys { 0 };

bool KeyFilter::pressedKeys[256] {};
bool KeyFilter::statusKeys[256] {};
