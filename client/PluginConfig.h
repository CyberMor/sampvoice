/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <string>

#include <d3d9.h>

class PluginConfig {

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

    static bool Load(const std::string& configFilePath);
    static bool Save(const std::string& configFilePath);

    static void Reset() noexcept;
    static bool IsLoaded() noexcept;

    static bool IsPlaybackLoaded() noexcept;
    static bool IsSpeakerLoaded() noexcept;
    static bool IsRecordLoaded() noexcept;
    static bool IsMicroLoaded() noexcept;

    static void SetPlaybackLoaded(bool status) noexcept;
    static void SetSpeakerLoaded(bool status) noexcept;
    static void SetRecordLoaded(bool status) noexcept;
    static void SetMicroLoaded(bool status) noexcept;

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

    static void SetSoundEnable(bool soundEnable) noexcept;
    static void SetSoundVolume(int soundVolume) noexcept;
    static void SetSoundBalancer(bool soundBalancer) noexcept;
    static void SetSoundFilter(bool soundFilter) noexcept;
    static void SetSpeakerIconScale(float speakerIconScale) noexcept;
    static void SetSpeakerIconOffsetX(int speakerIconOffsetX) noexcept;
    static void SetSpeakerIconOffsetY(int speakerIconOffsetY) noexcept;
    static void SetMicroEnable(bool microEnable) noexcept;
    static void SetMicroVolume(int microVolume) noexcept;
    static void SetDeviceName(std::string deviceName) noexcept;
    static void SetMicroIconScale(float microIconScale) noexcept;
    static void SetMicroIconPositionX(int microIconPositionX) noexcept;
    static void SetMicroIconPositionY(int microIconPositionY) noexcept;
    static void SetMicroIconColor(D3DCOLOR microIconColor) noexcept;
    static void SetMicroIconAngle(float microIconAngle) noexcept;

private:

    static bool loadStatus;

    static bool playbackLoaded;
    static bool speakerLoaded;
    static bool recordLoaded;
    static bool microLoaded;

    static bool soundEnable;
    static int soundVolume;
    static bool soundBalancer;
    static bool soundFilter;

    static float speakerIconScale;
    static int speakerIconOffsetX;
    static int speakerIconOffsetY;

    static bool microEnable;
    static int microVolume;
    static std::string deviceName;

    static float microIconScale;
    static int microIconPositionX;
    static int microIconPositionY;
    static D3DCOLOR microIconColor;
    static float microIconAngle;

};
