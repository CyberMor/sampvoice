/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <algorithm>

#include <memory/hacks/call_hook.hpp>
#include <memory/hacks/patch.hpp>

#include <audio/bass.h>
#include <audio/bass_fx.h>
#include <game/CCamera.h>
#include <util/addresses.hpp>
#include <util/logger.hpp>

#include "plugin_config.hpp"
#include "source.hpp"
#include "main.hpp"

#pragma comment(lib, "bass.lib")
#pragma comment(lib, "bass_fx.lib")

struct Listener {

    Listener(const Listener&) = delete;
    Listener(Listener&&) = delete;
    Listener& operator=(const Listener&) = delete;
    Listener& operator=(Listener&&) = delete;

private:

    Listener() noexcept = default;
    ~Listener() noexcept = default;

public:

    static Listener& Instance() noexcept
    {
        static Listener instance;
        return instance;
    }

public:

    bool Initialize() noexcept
    {
        Logger::Instance().LogToFile("[sv:dbg:listener:initialize] module initializing...");

        if (!_bass_initialization_hook.Initialize(Addresses::Instance().BassInitialization(), BassInitializationHook))
        {
            Logger::Instance().LogToFile("[sv:err:listener:initialize] failed to initialize hook");
            _bass_initialization_hook.Deinitialize();
            return false;
        }

        FillWithNops<8>(Addresses::Instance().BassConfiguration());

        Logger::Instance().LogToFile("[sv:dbg:listener:initialize] module initialized");

        return true;
    }

    void Deinitialize() noexcept
    {
        Logger::Instance().LogToFile("[sv:dbg:listener:deinitialize] module releasing...");

        _bass_initialization_hook.Deinitialize();

        _is_loaded = false;

        Logger::Instance().LogToFile("[sv:dbg:listener:deinitialize] module released");
    }

public:

    bool IsLoaded() const noexcept
    {
        return _is_loaded;
    }

public:

    void Tick() noexcept
    {
        if (_is_loaded)
        {
            BASS_Set3DPosition
            (
                reinterpret_cast<const BASS_3DVECTOR*>(&TheCamera.GetPosition()), nullptr,
                reinterpret_cast<const BASS_3DVECTOR*>(&TheCamera.GetMatrix()->at),
                reinterpret_cast<const BASS_3DVECTOR*>(&TheCamera.GetMatrix()->up)
            );

            BASS_Apply3D();
        }
    }

public:

    bool IsSoundEnable() const noexcept
    {
        return PluginConfig::Instance().IsSoundEnable();
    }

    sdword_t GetSoundVolume() const noexcept
    {
        return PluginConfig::Instance().GetSoundVolume();
    }

    bool IsSoundBalancer() const noexcept
    {
        return PluginConfig::Instance().IsSoundBalancer();
    }

    bool IsSoundFilter() const noexcept
    {
        return PluginConfig::Instance().IsSoundFilter();
    }

public:

    void SetSoundEnable(const bool sound_enable) noexcept
    {
        if (_is_loaded)
        {
            PluginConfig::Instance().SetSoundEnable(sound_enable);
            if (!PluginConfig::Instance().IsSoundEnable()) BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 0);
            else BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 100 * PluginConfig::Instance().GetSoundVolume());
        }
    }

    void SetSoundVolume(const sdword_t sound_volume) noexcept
    {
        if (_is_loaded)
        {
            PluginConfig::Instance().SetSoundVolume(std::clamp(sound_volume, 0, 100));
            if (PluginConfig::Instance().IsSoundEnable())
                BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 100 * PluginConfig::Instance().GetSoundVolume());
        }
    }

    void SetSoundBalancer(const bool sound_balancer) noexcept
    {
        static HFX balancer_fx_handle = NULL;

        if (_is_loaded)
        {
            PluginConfig::Instance().SetSoundBalancer(sound_balancer);
            if (PluginConfig::Instance().IsSoundBalancer())
            {
                if (balancer_fx_handle == NULL)
                {
                    balancer_fx_handle = BASS_ChannelSetFX(_channel_device, BASS_FX_BFX_COMPRESSOR2, 0);
                    if (balancer_fx_handle == NULL)
                    {
                        Logger::Instance().LogToFile("[sv:err:listener:setsoundbalancer] failed to "
                            "set balancer effect (code:%d)", BASS_ErrorGetCode());
                        return PluginConfig::Instance().SetSoundBalancer(false);
                    }

                    BASS_BFX_COMPRESSOR2 balancer_parameters;

                    balancer_parameters.lChannel   = BASS_BFX_CHANALL;
                    balancer_parameters.fGain      = 10;
                    balancer_parameters.fAttack    = 0.01;
                    balancer_parameters.fRelease   = 0.01;
                    balancer_parameters.fThreshold = -40;
                    balancer_parameters.fRatio     = 12;

                    if (BASS_FXSetParameters(balancer_fx_handle, &balancer_parameters) == FALSE)
                    {
                        Logger::Instance().LogToFile("[sv:err:listener:setsoundbalancer] failed to "
                            "set parameters (code:%d)", BASS_ErrorGetCode());
                        BASS_ChannelRemoveFX(_channel_device, balancer_fx_handle);
                        balancer_fx_handle = NULL;
                        PluginConfig::Instance().SetSoundBalancer(false);
                    }
                }
            }
            else
            {
                if (balancer_fx_handle != NULL)
                {
                    BASS_ChannelRemoveFX(_channel_device, balancer_fx_handle);
                    balancer_fx_handle = NULL;
                }
            }
        }
    }

    void SetSoundFilter(const bool sound_filter) noexcept
    {
        static HFX filter_fx_handle = NULL;

        if (_is_loaded)
        {
            PluginConfig::Instance().SetSoundFilter(sound_filter);
            if (PluginConfig::Instance().IsSoundFilter())
            {
                if (filter_fx_handle == NULL)
                {
                    filter_fx_handle = BASS_ChannelSetFX(_channel_device, BASS_FX_BFX_BQF, 0);
                    if (filter_fx_handle == NULL)
                    {
                        Logger::Instance().LogToFile("[sv:err:listener:setsoundfilter] failed to "
                            "set filter effect (code:%d)", BASS_ErrorGetCode());
                        return PluginConfig::Instance().SetSoundFilter(false);
                    }

                    BASS_BFX_BQF filter_parameters;

                    filter_parameters.lChannel   = BASS_BFX_CHANALL;
                    filter_parameters.lFilter    = BASS_BFX_BQF_LOWPASS;
                    filter_parameters.fCenter    = 3400;
                    filter_parameters.fBandwidth = 0;
                    filter_parameters.fQ         = 0.707;

                    filter_parameters.fGain = 0;
                    filter_parameters.fS    = 0;

                    if (BASS_FXSetParameters(filter_fx_handle, &filter_parameters) == FALSE)
                    {
                        Logger::Instance().LogToFile("[sv:err:listener:setsoundfilter] failed to "
                            "set parameters (code:%d)", BASS_ErrorGetCode());
                        BASS_ChannelRemoveFX(_channel_device, filter_fx_handle);
                        filter_fx_handle = NULL;
                        PluginConfig::Instance().SetSoundFilter(false);
                    }
                }
            }
            else
            {
                if (filter_fx_handle != NULL)
                {
                    BASS_ChannelRemoveFX(_channel_device, filter_fx_handle);
                    filter_fx_handle = NULL;
                }
            }
        }
    }

public:

    void SyncConfigs() noexcept
    {
        SetSoundEnable(PluginConfig::Instance().IsSoundEnable());
        SetSoundVolume(PluginConfig::Instance().GetSoundVolume());
        SetSoundBalancer(PluginConfig::Instance().IsSoundBalancer());
        SetSoundFilter(PluginConfig::Instance().IsSoundFilter());
    }

    void ResetConfigs() noexcept
    {
        PluginConfig::Instance().SetSoundEnable(PluginConfig::kDefaultSoundEnable);
        PluginConfig::Instance().SetSoundVolume(PluginConfig::kDefaultSoundVolume);
        PluginConfig::Instance().SetSoundBalancer(PluginConfig::kDefaultSoundBalancer);
        PluginConfig::Instance().SetSoundFilter(PluginConfig::kDefaultSoundFilter);
    }

private:

    static BOOL WINAPI BassInitializationHook(const int device, const DWORD frequency,
        const DWORD flags, const HWND window, const GUID* const dsguid) noexcept
    {
        Logger::Instance().LogToFile("[sv:dbg:listener:load] module loading...");

        if (BASS_SetConfig(BASS_CONFIG_UNICODE, TRUE) == FALSE)
            Logger::Instance().LogToFile("[sv:err:listener:load] failed to "
                "set 'BASS_CONFIG_UNICODE' parameter (code:%d)", BASS_ErrorGetCode());
        if (BASS_SetConfig(BASS_CONFIG_UPDATEPERIOD, 0) == FALSE)
            Logger::Instance().LogToFile("[sv:err:listener:load] failed to "
                "set 'BASS_CONFIG_UPDATEPERIOD' parameter (code:%d)", BASS_ErrorGetCode());
        if (BASS_SetConfig(BASS_CONFIG_UPDATETHREADS, 0) == FALSE)
            Logger::Instance().LogToFile("[sv:err:listener:load] failed to "
                "set 'BASS_CONFIG_UPDATETHREADS' parameter (code:%d)", BASS_ErrorGetCode());
        if (BASS_SetConfig(BASS_CONFIG_DEV_PERIOD, kInterval) == FALSE)
            Logger::Instance().LogToFile("[sv:err:listener:load] failed to "
                "set 'BASS_CONFIG_DEV_PERIOD' parameter (code:%d)", BASS_ErrorGetCode());
        if (BASS_SetConfig(BASS_CONFIG_DEV_BUFFER, kSourcePlayingBuffer * kInterval) == FALSE)
            Logger::Instance().LogToFile("[sv:err:listener:load] failed to "
                "set 'BASS_CONFIG_DEV_BUFFER' parameter (code:%d)", BASS_ErrorGetCode());
        if (BASS_SetConfig(BASS_CONFIG_3DALGORITHM, BASS_3DALG_LIGHT) == FALSE)
            Logger::Instance().LogToFile("[sv:err:listener:load] failed to "
                "set 'BASS_CONFIG_3DALGORITHM' parameter (code:%d)", BASS_ErrorGetCode());

        Logger::Instance().LogToFile("[sv:dbg:listener:load] hooked function BASS_Init(device:%d, "
            "frequency:%u, flags:0x%X, window:0x%X, dsguid:0x%X)...", device, frequency, flags, window, dsguid);

        Logger::Instance().LogToFile("[sv:dbg:listener:load] calling function BASS_Init(device:%d, "
            "frequency:%u, flags:0x%X, window:0x%X, dsguid:0x%X)...",
            device, kFrequency, (BASS_DEVICE_MONO | BASS_DEVICE_3D) | flags, window, dsguid);

        if (BASS_Init(device, kFrequency, (BASS_DEVICE_MONO | BASS_DEVICE_3D) | flags, window, dsguid) == FALSE)
        {
            Logger::Instance().LogToFile("[sv:err:listener:load] failed to "
                "initialize bass library (code:%d)", BASS_ErrorGetCode());
            return FALSE;
        }

        if (HIWORD(BASS_FX_GetVersion()) != BASSVERSION)
        {
            Logger::Instance().LogToFile("[sv:err:listener:load] failed to "
                "check version bassfx library (code:%d)", BASS_ErrorGetCode());
            return FALSE;
        }

        Instance()._channel_device = BASS_StreamCreate(0, 0, NULL, STREAMPROC_DEVICE, nullptr);
        if (Instance()._channel_device == NULL)
        {
            Logger::Instance().LogToFile("[sv:err:listener:load] failed to "
                "create device output channel (code:%d)", BASS_ErrorGetCode());
            return FALSE;
        }

        Logger::Instance().LogToFile("[sv:dbg:listener:load] volume(%.2f)", BASS_GetVolume());

        const BASS_3DVECTOR kZeroVector { 0, 0, 0 };

        BASS_Set3DFactors(1, 1, 0);
        BASS_Set3DPosition(&kZeroVector, &kZeroVector, &kZeroVector, &kZeroVector);
        BASS_Apply3D();

        Logger::Instance().LogToFile("[sv:dbg:listener:load] module loaded");

        Instance()._is_loaded = true;
        Instance().SyncConfigs();

        return TRUE;
    }

private:

    volatile bool _is_loaded = false;

private:

    HSTREAM _channel_device = NULL;

    CallHook _bass_initialization_hook;

};
