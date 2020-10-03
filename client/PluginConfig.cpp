/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "PluginConfig.h"

#include <fstream>

#include <util/Render.h>

bool PluginConfig::Load(const std::string& configFilePath)
{
    constexpr auto kFileMode = std::ios::in | std::ios::binary;

    std::ifstream configFile { configFilePath, kFileMode };

    if (!configFile || !configFile.is_open())
        return false;

    PluginConfig::Reset();

    if (int configVersion { -1 };
        !configFile.read(reinterpret_cast<char*>(&configVersion), sizeof(configVersion)) ||
        configVersion != kConfigVersion) return false;

    return PluginConfig::loadStatus =

        configFile.read(reinterpret_cast<char*>(&playbackLoaded),     sizeof(playbackLoaded))     &&
        configFile.read(reinterpret_cast<char*>(&speakerLoaded),      sizeof(speakerLoaded))      &&
        configFile.read(reinterpret_cast<char*>(&recordLoaded),       sizeof(recordLoaded))       &&
        configFile.read(reinterpret_cast<char*>(&microLoaded),        sizeof(microLoaded))        &&

        configFile.read(reinterpret_cast<char*>(&soundEnable),        sizeof(soundEnable))        &&
        configFile.read(reinterpret_cast<char*>(&soundVolume),        sizeof(soundVolume))        &&
        configFile.read(reinterpret_cast<char*>(&soundBalancer),      sizeof(soundBalancer))      &&
        configFile.read(reinterpret_cast<char*>(&soundFilter),        sizeof(soundFilter))        &&

        configFile.read(reinterpret_cast<char*>(&speakerIconScale),   sizeof(speakerIconScale))   &&
        configFile.read(reinterpret_cast<char*>(&speakerIconOffsetX), sizeof(speakerIconOffsetX)) &&
        configFile.read(reinterpret_cast<char*>(&speakerIconOffsetY), sizeof(speakerIconOffsetY)) &&

        configFile.read(reinterpret_cast<char*>(&microEnable),        sizeof(microEnable))        &&
        configFile.read(reinterpret_cast<char*>(&microVolume),        sizeof(microVolume))        &&
        std::getline(configFile, deviceName, '\0')                                                &&

        configFile.read(reinterpret_cast<char*>(&microIconScale),     sizeof(microIconScale))     &&
        configFile.read(reinterpret_cast<char*>(&microIconPositionX), sizeof(microIconPositionX)) &&
        configFile.read(reinterpret_cast<char*>(&microIconPositionY), sizeof(microIconPositionY)) &&
        configFile.read(reinterpret_cast<char*>(&microIconColor),     sizeof(microIconColor))     &&
        configFile.read(reinterpret_cast<char*>(&microIconAngle),     sizeof(microIconAngle))     ;
}

bool PluginConfig::Save(const std::string& configFilePath)
{
    constexpr auto kFileMode = std::ios::out | std::ios::binary | std::ios::trunc;

    std::ofstream configFile { configFilePath, kFileMode };

    if (!configFile || !configFile.is_open())
        return false;

    const auto configVersion { kConfigVersion };

    return configFile.write(reinterpret_cast<const char*>(&configVersion),      sizeof(configVersion))      &&

           configFile.write(reinterpret_cast<const char*>(&playbackLoaded),     sizeof(playbackLoaded))     &&
           configFile.write(reinterpret_cast<const char*>(&speakerLoaded),      sizeof(speakerLoaded))      &&
           configFile.write(reinterpret_cast<const char*>(&recordLoaded),       sizeof(recordLoaded))       &&
           configFile.write(reinterpret_cast<const char*>(&microLoaded),        sizeof(microLoaded))        &&

           configFile.write(reinterpret_cast<const char*>(&soundEnable),        sizeof(soundEnable))        &&
           configFile.write(reinterpret_cast<const char*>(&soundVolume),        sizeof(soundVolume))        &&
           configFile.write(reinterpret_cast<const char*>(&soundBalancer),      sizeof(soundBalancer))      &&
           configFile.write(reinterpret_cast<const char*>(&soundFilter),        sizeof(soundFilter))        &&

           configFile.write(reinterpret_cast<const char*>(&speakerIconScale),   sizeof(speakerIconScale))   &&
           configFile.write(reinterpret_cast<const char*>(&speakerIconOffsetX), sizeof(speakerIconOffsetX)) &&
           configFile.write(reinterpret_cast<const char*>(&speakerIconOffsetY), sizeof(speakerIconOffsetY)) &&

           configFile.write(reinterpret_cast<const char*>(&microEnable),        sizeof(microEnable))        &&
           configFile.write(reinterpret_cast<const char*>(&microVolume),        sizeof(microVolume))        &&
           configFile.write(deviceName.data(),                                  deviceName.size() + 1)      &&

           configFile.write(reinterpret_cast<const char*>(&microIconScale),     sizeof(microIconScale))     &&
           configFile.write(reinterpret_cast<const char*>(&microIconPositionX), sizeof(microIconPositionX)) &&
           configFile.write(reinterpret_cast<const char*>(&microIconPositionY), sizeof(microIconPositionY)) &&
           configFile.write(reinterpret_cast<const char*>(&microIconColor),     sizeof(microIconColor))     &&
           configFile.write(reinterpret_cast<const char*>(&microIconAngle),     sizeof(microIconAngle))     ;
}

void PluginConfig::Reset() noexcept
{
    PluginConfig::loadStatus = false;

    PluginConfig::playbackLoaded = false;
    PluginConfig::speakerLoaded = false;
    PluginConfig::recordLoaded = false;
    PluginConfig::microLoaded = false;

    PluginConfig::soundEnable = kDefValSoundEnable;
    PluginConfig::soundVolume = kDefValSoundVolume;
    PluginConfig::soundBalancer = kDefValSoundBalancer;
    PluginConfig::soundFilter = kDefValSoundFilter;

    PluginConfig::speakerIconScale = kDefValSpeakerIconScale;
    PluginConfig::speakerIconOffsetX = kDefValSpeakerIconOffsetX;
    PluginConfig::speakerIconOffsetY = kDefValSpeakerIconOffsetY;

    PluginConfig::microEnable = kDefValMicroEnable;
    PluginConfig::microVolume = kDefValMicroVolume;
    PluginConfig::deviceName.clear();

    PluginConfig::microIconScale = kDefValMicroIconScale;
    PluginConfig::microIconPositionX = kDefValMicroIconPositionX;
    PluginConfig::microIconPositionY = kDefValMicroIconPositionY;
    PluginConfig::microIconColor = kDefValMicroIconColor;
    PluginConfig::microIconAngle = kDefValMicroIconAngle;
}

bool PluginConfig::IsLoaded() noexcept
{
    return PluginConfig::loadStatus;
}

bool PluginConfig::IsPlaybackLoaded() noexcept
{
    return PluginConfig::playbackLoaded;
}

bool PluginConfig::IsSpeakerLoaded() noexcept
{
    return PluginConfig::speakerLoaded;
}

bool PluginConfig::IsRecordLoaded() noexcept
{
    return PluginConfig::recordLoaded;
}

bool PluginConfig::IsMicroLoaded() noexcept
{
    return PluginConfig::microLoaded;
}

void PluginConfig::SetPlaybackLoaded(const bool status) noexcept
{
    PluginConfig::playbackLoaded = status;
}

void PluginConfig::SetSpeakerLoaded(const bool status) noexcept
{
    PluginConfig::speakerLoaded = status;
}

void PluginConfig::SetRecordLoaded(const bool status) noexcept
{
    PluginConfig::recordLoaded = status;
}

void PluginConfig::SetMicroLoaded(const bool status) noexcept
{
    PluginConfig::microLoaded = status;
}

bool PluginConfig::GetSoundEnable() noexcept
{
    return PluginConfig::soundEnable;
}

int PluginConfig::GetSoundVolume() noexcept
{
    return PluginConfig::soundVolume;
}

bool PluginConfig::GetSoundBalancer() noexcept
{
    return PluginConfig::soundBalancer;
}

bool PluginConfig::GetSoundFilter() noexcept
{
    return PluginConfig::soundFilter;
}

float PluginConfig::GetSpeakerIconScale() noexcept
{
    return PluginConfig::speakerIconScale;
}

int PluginConfig::GetSpeakerIconOffsetX() noexcept
{
    float screenValue { 0.f };

    return Render::ConvertBaseXValueToScreenXValue(PluginConfig::speakerIconOffsetX, screenValue)
        ? std::roundf(screenValue) : kDefValSpeakerIconOffsetX;
}

int PluginConfig::GetSpeakerIconOffsetY() noexcept
{
    float screenValue { 0.f };

    return Render::ConvertBaseYValueToScreenYValue(PluginConfig::speakerIconOffsetY, screenValue)
        ? std::roundf(screenValue) : kDefValSpeakerIconOffsetY;
}

bool PluginConfig::GetMicroEnable() noexcept
{
    return PluginConfig::microEnable;
}

int PluginConfig::GetMicroVolume() noexcept
{
    return PluginConfig::microVolume;
}

const std::string& PluginConfig::GetDeviceName() noexcept
{
    return PluginConfig::deviceName;
}

float PluginConfig::GetMicroIconScale() noexcept
{
    return PluginConfig::microIconScale;
}

int PluginConfig::GetMicroIconPositionX() noexcept
{
    float screenValue { 0.f };

    return Render::ConvertBaseXValueToScreenXValue(PluginConfig::microIconPositionX, screenValue)
        ? std::roundf(screenValue) : kDefValMicroIconPositionX;
}

int PluginConfig::GetMicroIconPositionY() noexcept
{
    float screenValue { 0.f };

    return Render::ConvertBaseYValueToScreenYValue(PluginConfig::microIconPositionY, screenValue)
        ? std::roundf(screenValue) : kDefValMicroIconPositionY;
}

D3DCOLOR PluginConfig::GetMicroIconColor() noexcept
{
    return PluginConfig::microIconColor;
}

float PluginConfig::GetMicroIconAngle() noexcept
{
    return PluginConfig::microIconAngle;
}

void PluginConfig::SetSoundEnable(const bool soundEnable) noexcept
{
    PluginConfig::soundEnable = soundEnable;
}

void PluginConfig::SetSoundVolume(const int soundVolume) noexcept
{
    PluginConfig::soundVolume = soundVolume;
}

void PluginConfig::SetSoundBalancer(const bool soundBalancer) noexcept
{
    PluginConfig::soundBalancer = soundBalancer;
}

void PluginConfig::SetSoundFilter(const bool soundFilter) noexcept
{
    PluginConfig::soundFilter = soundFilter;
}

void PluginConfig::SetSpeakerIconScale(const float speakerIconScale) noexcept
{
    PluginConfig::speakerIconScale = speakerIconScale;
}

void PluginConfig::SetSpeakerIconOffsetX(const int speakerIconOffsetX) noexcept
{
    float baseValue { 0.f };

    if (Render::ConvertScreenXValueToBaseXValue(speakerIconOffsetX, baseValue))
        PluginConfig::speakerIconOffsetX = std::roundf(baseValue);
}

void PluginConfig::SetSpeakerIconOffsetY(const int speakerIconOffsetY) noexcept
{
    float baseValue { 0.f };

    if (Render::ConvertScreenYValueToBaseYValue(speakerIconOffsetY, baseValue))
        PluginConfig::speakerIconOffsetY = std::roundf(baseValue);
}

void PluginConfig::SetMicroEnable(const bool microEnable) noexcept
{
    PluginConfig::microEnable = microEnable;
}

void PluginConfig::SetMicroVolume(const int microVolume) noexcept
{
    PluginConfig::microVolume = microVolume;
}

void PluginConfig::SetDeviceName(std::string deviceName) noexcept
{
    PluginConfig::deviceName = std::move(deviceName);
}

void PluginConfig::SetMicroIconScale(const float microIconScale) noexcept
{
    PluginConfig::microIconScale = microIconScale;
}

void PluginConfig::SetMicroIconPositionX(const int microIconPositionX) noexcept
{
    float baseValue { 0.f };

    if (Render::ConvertScreenXValueToBaseXValue(microIconPositionX, baseValue))
        PluginConfig::microIconPositionX = std::roundf(baseValue);
}

void PluginConfig::SetMicroIconPositionY(const int microIconPositionY) noexcept
{
    float baseValue { 0.f };

    if (Render::ConvertScreenYValueToBaseYValue(microIconPositionY, baseValue))
        PluginConfig::microIconPositionY = std::roundf(baseValue);
}

void PluginConfig::SetMicroIconColor(const D3DCOLOR microIconColor) noexcept
{
    PluginConfig::microIconColor = microIconColor;
}

void PluginConfig::SetMicroIconAngle(const float microIconAngle) noexcept
{
    PluginConfig::microIconAngle = microIconAngle;
}

bool PluginConfig::loadStatus { false };

bool PluginConfig::playbackLoaded { false };
bool PluginConfig::speakerLoaded { false };
bool PluginConfig::recordLoaded { false };
bool PluginConfig::microLoaded { false };

bool PluginConfig::soundEnable { kDefValSoundEnable };
int PluginConfig::soundVolume { kDefValSoundVolume };
bool PluginConfig::soundBalancer { kDefValSoundBalancer };
bool PluginConfig::soundFilter { kDefValSoundFilter };

float PluginConfig::speakerIconScale { kDefValSpeakerIconScale };
int PluginConfig::speakerIconOffsetX { kDefValSpeakerIconOffsetX };
int PluginConfig::speakerIconOffsetY { kDefValSpeakerIconOffsetY };

bool PluginConfig::microEnable { kDefValMicroEnable };
int PluginConfig::microVolume { kDefValMicroVolume };
std::string PluginConfig::deviceName;

float PluginConfig::microIconScale { kDefValMicroIconScale };
int PluginConfig::microIconPositionX { kDefValMicroIconPositionX };
int PluginConfig::microIconPositionY { kDefValMicroIconPositionY };
D3DCOLOR PluginConfig::microIconColor { kDefValMicroIconColor };
float PluginConfig::microIconAngle { kDefValMicroIconAngle };
