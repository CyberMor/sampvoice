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

struct Playback {

    Playback() = delete;
    ~Playback() = delete;
    Playback(const Playback&) = delete;
    Playback(Playback&&) = delete;
    Playback& operator=(const Playback&) = delete;
    Playback& operator=(Playback&&) = delete;

public:

    static bool Init(const AddressesBase& addr_base) noexcept;
    static void Free() noexcept;

    static void Tick() noexcept;

    static bool GetSoundEnable() noexcept;
    static int GetSoundVolume() noexcept;
    static bool GetSoundBalancer() noexcept;
    static bool GetSoundFilter() noexcept;

    static void SetSoundEnable(bool sound_enable) noexcept;
    static void SetSoundVolume(int sound_volume) noexcept;
    static void SetSoundBalancer(bool sound_balancer) noexcept;
    static void SetSoundFilter(bool sound_filter) noexcept;

    static void SyncConfigs() noexcept;
    static void ResetConfigs() noexcept;

private:

    static BOOL WINAPI BassInitHookFunc(int device, DWORD freq, DWORD flags, HWND win, const GUID* dsguid) noexcept;

private:

    static bool initStatus;
    static bool loadStatus;

    static HSTREAM deviceOutputChannel;

    static Memory::CallHook bassInitHook;

};
