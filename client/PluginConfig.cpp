/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "PluginConfig.h"

#include <fstream>

#include <util/Render.h>

bool PluginConfig::Load(const std::string& path)
{
    std::fstream file { path, std::ios::in | std::ios::binary };
    if (!file || !file.is_open()) return false;

    Reset();

    if (int config_version; !file.read(reinterpret_cast<char*>(&config_version),
        sizeof(config_version)) || config_version != kConfigVersion)
    {
        return false;
    }

    return _load_status =

        file.read(reinterpret_cast<char*>(&_playback_loaded),       sizeof(_playback_loaded))       &&
        file.read(reinterpret_cast<char*>(&_speaker_loaded),        sizeof(_speaker_loaded))        &&
        file.read(reinterpret_cast<char*>(&_record_loaded),         sizeof(_record_loaded))         &&
        file.read(reinterpret_cast<char*>(&_micro_loaded),          sizeof(_micro_loaded))          &&

        file.read(reinterpret_cast<char*>(&_sound_enable),          sizeof(_sound_enable))          &&
        file.read(reinterpret_cast<char*>(&_sound_volume),          sizeof(_sound_volume))          &&
        file.read(reinterpret_cast<char*>(&_sound_balancer),        sizeof(_sound_balancer))        &&
        file.read(reinterpret_cast<char*>(&_sound_filter),          sizeof(_sound_filter))          &&

        file.read(reinterpret_cast<char*>(&_speaker_icon_scale),    sizeof(_speaker_icon_scale))    &&
        file.read(reinterpret_cast<char*>(&_speaker_icon_offset_x), sizeof(_speaker_icon_offset_x)) &&
        file.read(reinterpret_cast<char*>(&_speaker_icon_offset_y), sizeof(_speaker_icon_offset_y)) &&

        file.read(reinterpret_cast<char*>(&_micro_enable),          sizeof(_micro_enable))          &&
        file.read(reinterpret_cast<char*>(&_micro_volume),          sizeof(_micro_volume))          &&
        std::getline(file, _device_name, '\0')                                                      &&

        file.read(reinterpret_cast<char*>(&_micro_icon_scale),      sizeof(_micro_icon_scale))      &&
        file.read(reinterpret_cast<char*>(&_micro_icon_position_x), sizeof(_micro_icon_position_x)) &&
        file.read(reinterpret_cast<char*>(&_micro_icon_position_y), sizeof(_micro_icon_position_y)) &&
        file.read(reinterpret_cast<char*>(&_micro_icon_color),      sizeof(_micro_icon_color))      &&
        file.read(reinterpret_cast<char*>(&_micro_icon_angle),      sizeof(_micro_icon_angle))      ;
}

bool PluginConfig::Save(const std::string& path)
{
    std::fstream file { path, std::ios::out | std::ios::binary | std::ios::trunc };
    if (!file || !file.is_open()) return false;

    return file.write(reinterpret_cast<const char*>(&kConfigVersion),         sizeof(kConfigVersion))         &&

           file.write(reinterpret_cast<const char*>(&_playback_loaded),       sizeof(_playback_loaded))       &&
           file.write(reinterpret_cast<const char*>(&_speaker_loaded),        sizeof(_speaker_loaded))        &&
           file.write(reinterpret_cast<const char*>(&_record_loaded),         sizeof(_record_loaded))         &&
           file.write(reinterpret_cast<const char*>(&_micro_loaded),          sizeof(_micro_loaded))          &&

           file.write(reinterpret_cast<const char*>(&_sound_enable),          sizeof(_sound_enable))          &&
           file.write(reinterpret_cast<const char*>(&_sound_volume),          sizeof(_sound_volume))          &&
           file.write(reinterpret_cast<const char*>(&_sound_balancer),        sizeof(_sound_balancer))        &&
           file.write(reinterpret_cast<const char*>(&_sound_filter),          sizeof(_sound_filter))          &&

           file.write(reinterpret_cast<const char*>(&_speaker_icon_scale),    sizeof(_speaker_icon_scale))    &&
           file.write(reinterpret_cast<const char*>(&_speaker_icon_offset_x), sizeof(_speaker_icon_offset_x)) &&
           file.write(reinterpret_cast<const char*>(&_speaker_icon_offset_y), sizeof(_speaker_icon_offset_y)) &&

           file.write(reinterpret_cast<const char*>(&_micro_enable),          sizeof(_micro_enable))          &&
           file.write(reinterpret_cast<const char*>(&_micro_volume),          sizeof(_micro_volume))          &&
           file.write(_device_name.data(),                                    _device_name.size() + 1)        &&

           file.write(reinterpret_cast<const char*>(&_micro_icon_scale),      sizeof(_micro_icon_scale))      &&
           file.write(reinterpret_cast<const char*>(&_micro_icon_position_x), sizeof(_micro_icon_position_x)) &&
           file.write(reinterpret_cast<const char*>(&_micro_icon_position_y), sizeof(_micro_icon_position_y)) &&
           file.write(reinterpret_cast<const char*>(&_micro_icon_color),      sizeof(_micro_icon_color))      &&
           file.write(reinterpret_cast<const char*>(&_micro_icon_angle),      sizeof(_micro_icon_angle))      ;
}

void PluginConfig::Reset() noexcept
{
    _load_status = false;

    _playback_loaded = false;
    _speaker_loaded = false;
    _record_loaded = false;
    _micro_loaded = false;

    _sound_enable = kDefValSoundEnable;
    _sound_volume = kDefValSoundVolume;
    _sound_balancer = kDefValSoundBalancer;
    _sound_filter = kDefValSoundFilter;

    _speaker_icon_scale = kDefValSpeakerIconScale;
    _speaker_icon_offset_x = kDefValSpeakerIconOffsetX;
    _speaker_icon_offset_y = kDefValSpeakerIconOffsetY;

    _micro_enable = kDefValMicroEnable;
    _micro_volume = kDefValMicroVolume;
    _device_name.clear();

    _micro_icon_scale = kDefValMicroIconScale;
    _micro_icon_position_x = kDefValMicroIconPositionX;
    _micro_icon_position_y = kDefValMicroIconPositionY;
    _micro_icon_color = kDefValMicroIconColor;
    _micro_icon_angle = kDefValMicroIconAngle;
}

bool PluginConfig::IsLoaded() noexcept
{
    return _load_status;
}

bool PluginConfig::IsPlaybackLoaded() noexcept
{
    return _playback_loaded;
}

bool PluginConfig::IsSpeakerLoaded() noexcept
{
    return _speaker_loaded;
}

bool PluginConfig::IsRecordLoaded() noexcept
{
    return _record_loaded;
}

bool PluginConfig::IsMicroLoaded() noexcept
{
    return _micro_loaded;
}

void PluginConfig::SetPlaybackLoaded(const bool value) noexcept
{
    _playback_loaded = value;
}

void PluginConfig::SetSpeakerLoaded(const bool value) noexcept
{
    _speaker_loaded = value;
}

void PluginConfig::SetRecordLoaded(const bool value) noexcept
{
    _record_loaded = value;
}

void PluginConfig::SetMicroLoaded(const bool value) noexcept
{
    _micro_loaded = value;
}

bool PluginConfig::GetSoundEnable() noexcept
{
    return _sound_enable;
}

int PluginConfig::GetSoundVolume() noexcept
{
    return _sound_volume;
}

bool PluginConfig::GetSoundBalancer() noexcept
{
    return _sound_balancer;
}

bool PluginConfig::GetSoundFilter() noexcept
{
    return _sound_filter;
}

float PluginConfig::GetSpeakerIconScale() noexcept
{
    return _speaker_icon_scale;
}

int PluginConfig::GetSpeakerIconOffsetX() noexcept
{
    if (float screen_value; Render::ConvertBaseXValueToScreenXValue(_speaker_icon_offset_x, screen_value))
    {
        return std::roundf(screen_value);
    }

    return kDefValSpeakerIconOffsetX;
}

int PluginConfig::GetSpeakerIconOffsetY() noexcept
{
    if (float screen_value; Render::ConvertBaseYValueToScreenYValue(_speaker_icon_offset_y, screen_value))
    {
        return std::roundf(screen_value);
    }

    return kDefValSpeakerIconOffsetY;
}

bool PluginConfig::GetMicroEnable() noexcept
{
    return _micro_enable;
}

int PluginConfig::GetMicroVolume() noexcept
{
    return _micro_volume;
}

const std::string& PluginConfig::GetDeviceName() noexcept
{
    return _device_name;
}

float PluginConfig::GetMicroIconScale() noexcept
{
    return _micro_icon_scale;
}

int PluginConfig::GetMicroIconPositionX() noexcept
{
    if (float screen_value; Render::ConvertBaseXValueToScreenXValue(_micro_icon_position_x, screen_value))
    {
        return std::roundf(screen_value);
    }

    return kDefValMicroIconPositionX;
}

int PluginConfig::GetMicroIconPositionY() noexcept
{
    if (float screen_value; Render::ConvertBaseYValueToScreenYValue(_micro_icon_position_y, screen_value))
    {
        return std::roundf(screen_value);
    }

    return kDefValMicroIconPositionY;
}

D3DCOLOR PluginConfig::GetMicroIconColor() noexcept
{
    return _micro_icon_color;
}

float PluginConfig::GetMicroIconAngle() noexcept
{
    return _micro_icon_angle;
}

void PluginConfig::SetSoundEnable(const bool value) noexcept
{
    _sound_enable = value;
}

void PluginConfig::SetSoundVolume(const int value) noexcept
{
    _sound_volume = value;
}

void PluginConfig::SetSoundBalancer(const bool value) noexcept
{
    _sound_balancer = value;
}

void PluginConfig::SetSoundFilter(const bool value) noexcept
{
    _sound_filter = value;
}

void PluginConfig::SetSpeakerIconScale(const float value) noexcept
{
    _speaker_icon_scale = value;
}

void PluginConfig::SetSpeakerIconOffsetX(const int value) noexcept
{
    if (float base_value; Render::ConvertScreenXValueToBaseXValue(value, base_value))
        _speaker_icon_offset_x = std::roundf(base_value);
}

void PluginConfig::SetSpeakerIconOffsetY(const int value) noexcept
{
    if (float base_value; Render::ConvertScreenYValueToBaseYValue(value, base_value))
        _speaker_icon_offset_y = std::roundf(base_value);
}

void PluginConfig::SetMicroEnable(const bool value) noexcept
{
    _micro_enable = value;
}

void PluginConfig::SetMicroVolume(const int value) noexcept
{
    _micro_volume = value;
}

void PluginConfig::SetDeviceName(std::string value) noexcept
{
    _device_name = std::move(value);
}

void PluginConfig::SetMicroIconScale(const float value) noexcept
{
    _micro_icon_scale = value;
}

void PluginConfig::SetMicroIconPositionX(const int value) noexcept
{
    if (float base_value; Render::ConvertScreenXValueToBaseXValue(value, base_value))
        _micro_icon_position_x = std::roundf(base_value);
}

void PluginConfig::SetMicroIconPositionY(const int value) noexcept
{
    if (float base_value; Render::ConvertScreenYValueToBaseYValue(value, base_value))
        _micro_icon_position_y = std::roundf(base_value);
}

void PluginConfig::SetMicroIconColor(const D3DCOLOR value) noexcept
{
    _micro_icon_color = value;
}

void PluginConfig::SetMicroIconAngle(const float value) noexcept
{
    _micro_icon_angle = value;
}

bool PluginConfig::_load_status = false;

bool PluginConfig::_playback_loaded = false;
bool PluginConfig::_speaker_loaded = false;
bool PluginConfig::_record_loaded = false;
bool PluginConfig::_micro_loaded = false;

bool PluginConfig::_sound_enable = kDefValSoundEnable;
int  PluginConfig::_sound_volume = kDefValSoundVolume;
bool PluginConfig::_sound_balancer = kDefValSoundBalancer;
bool PluginConfig::_sound_filter = kDefValSoundFilter;

float PluginConfig::_speaker_icon_scale = kDefValSpeakerIconScale;
int   PluginConfig::_speaker_icon_offset_x = kDefValSpeakerIconOffsetX;
int   PluginConfig::_speaker_icon_offset_y = kDefValSpeakerIconOffsetY;

bool        PluginConfig::_micro_enable = kDefValMicroEnable;
int         PluginConfig::_micro_volume = kDefValMicroVolume;
std::string PluginConfig::_device_name;

float    PluginConfig::_micro_icon_scale = kDefValMicroIconScale;
int      PluginConfig::_micro_icon_position_x = kDefValMicroIconPositionX;
int      PluginConfig::_micro_icon_position_y = kDefValMicroIconPositionY;
D3DCOLOR PluginConfig::_micro_icon_color = kDefValMicroIconColor;
float    PluginConfig::_micro_icon_angle = kDefValMicroIconAngle;
