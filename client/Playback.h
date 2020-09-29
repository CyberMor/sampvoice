/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <audio/bass.h>
#include <util/Memory.hpp>
#include <util/AddressesBase.h>

class Playback {
public:

    static bool Init(const AddressesBase& addrBase);
    static void Free();

    static bool GetSoundEnable();
    static int GetSoundVolume();
    static bool GetSoundBalancer();
    static bool GetSoundFilter();

    static void SetSoundEnable(bool soundEnable);
    static void SetSoundVolume(int soundVolume);
    static void SetSoundBalancer(bool soundBalancer);
    static void SetSoundFilter(bool soundFilter);

    static void SyncConfigs();
    static void ResetConfigs();

    static void Update();

private:

    static BOOL WINAPI BassInitHookFunc(
        int device, DWORD freq, DWORD flags,
        HWND win, const GUID* dsguid
    );

private:

    static bool initStatus;
    static bool loadStatus;

    static HSTREAM deviceOutputChannel;

    static Memory::CallHookPtr bassInitHook;

};
