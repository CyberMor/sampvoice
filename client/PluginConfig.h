/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <string>

#include <d3d9.h>

struct PluginConfig {

    PluginConfig() = delete;
    ~PluginConfig() = delete;
    PluginConfig(const PluginConfig&) = delete;
    PluginConfig(PluginConfig&&) = delete;
    PluginConfig& operator=(const PluginConfig&) = delete;
    PluginConfig& operator=(PluginConfig&&) = delete;

public:

    /*
        All settings related to the screen are stored in the base resolution (640x480) and,
        upon receipt of the value, are converted to the screen resolution by the CRender module
    */

    static constexpr int      kConfigVersion            = 2;
    static constexpr bool     kDefValSoundEnable        = true;
    static constexpr int      kDefValSoundVolume        = 100;
    static constexpr bool     kDefValSoundBalancer      = false;
    static constexpr bool     kDefValSoundFilter        = false;
    static constexpr float    kDefValSpeakerIconScale   = 1.f;
    static constexpr int      kDefValSpeakerIconOffsetX = 0;
    static constexpr int      kDefValSpeakerIconOffsetY = 0;
    static constexpr bool     kDefValMicroEnable        = true;
    static constexpr int      kDefValMicroVolume        = 75;
    static constexpr float    kDefValMicroIconScale     = 1.f;
    static constexpr int      kDefValMicroIconPositionX = -100;
    static constexpr int      kDefValMicroIconPositionY = -100;
    static constexpr D3DCOLOR kDefValMicroIconColor     = 0x00ffffff;
    static constexpr float    kDefValMicroIconAngle     = 0.f;

public:

    static bool Load(const std::string& path);
    static bool Save(const std::string& path);

    static void Reset() noexcept;
    static bool IsLoaded() noexcept;

    static bool IsPlaybackLoaded() noexcept;
    static bool IsSpeakerLoaded() noexcept;
    static bool IsRecordLoaded() noexcept;
    static bool IsMicroLoaded() noexcept;

    static void SetPlaybackLoaded(bool value) noexcept;
    static void SetSpeakerLoaded(bool value) noexcept;
    static void SetRecordLoaded(bool value) noexcept;
    static void SetMicroLoaded(bool value) noexcept;

    static bool GetSoundEnable() noexcept;
    static int GetSoundVolume() noexcept;
    static bool GetSoundBalancer() noexcept;
    static bool GetSoundFilter() noexcept;
    static float GetSpeakerIconScale() noexcept;
    static int GetSpeakerIconOffsetX() noexcept;
    static int GetSpeakerIconOffsetY() noexcept;
    static bool GetMicroEnable() noexcept;
    static int GetMicroVolume() noexcept;
    static const std::string& GetDeviceName() noexcept;
    static float GetMicroIconScale() noexcept;
    static int GetMicroIconPositionX() noexcept;
    static int GetMicroIconPositionY() noexcept;
    static D3DCOLOR GetMicroIconColor() noexcept;
    static float GetMicroIconAngle() noexcept;

    static void SetSoundEnable(bool value) noexcept;
    static void SetSoundVolume(int value) noexcept;
    static void SetSoundBalancer(bool value) noexcept;
    static void SetSoundFilter(bool value) noexcept;
    static void SetSpeakerIconScale(float value) noexcept;
    static void SetSpeakerIconOffsetX(int value) noexcept;
    static void SetSpeakerIconOffsetY(int value) noexcept;
    static void SetMicroEnable(bool value) noexcept;
    static void SetMicroVolume(int value) noexcept;
    static void SetDeviceName(std::string value) noexcept;
    static void SetMicroIconScale(float value) noexcept;
    static void SetMicroIconPositionX(int value) noexcept;
    static void SetMicroIconPositionY(int value) noexcept;
    static void SetMicroIconColor(D3DCOLOR value) noexcept;
    static void SetMicroIconAngle(float value) noexcept;

private:

    static bool        _load_status;

    static bool        _playback_loaded;
    static bool        _speaker_loaded;
    static bool        _record_loaded;
    static bool        _micro_loaded;

    static bool        _sound_enable;
    static int         _sound_volume;
    static bool        _sound_balancer;
    static bool        _sound_filter;

    static float       _speaker_icon_scale;
    static int         _speaker_icon_offset_x;
    static int         _speaker_icon_offset_y;

    static bool        _micro_enable;
    static int         _micro_volume;
    static std::string _device_name;

    static float       _micro_icon_scale;
    static int         _micro_icon_position_x;
    static int         _micro_icon_position_y;
    static D3DCOLOR    _micro_icon_color;
    static float       _micro_icon_angle;

};
