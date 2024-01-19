/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cmath>
#include <cstring>

#include <d3d9.h>

#include <system/file.hpp>
#include <other/version.hpp>
#include <util/render.hpp>

struct PluginConfig {

    PluginConfig(const PluginConfig&) = delete;
    PluginConfig(PluginConfig&&) = delete;
    PluginConfig& operator=(const PluginConfig&) = delete;
    PluginConfig& operator=(PluginConfig&&) = delete;

private:

    PluginConfig() noexcept { *_language = '\0'; *_micro_device = '\0'; }
    ~PluginConfig() noexcept = default;

public:

    static PluginConfig& Instance() noexcept
    {
        static PluginConfig instance;
        return instance;
    }

public:

    /*
        All settings related to the screen are stored in the base resolution (640x480) and,
        upon receipt of the value, are converted to the screen resolution by the CRender module
    */

    static constexpr Version  kConfigVersion             = MakeVersion(1, 0, 0);

    static constexpr bool     kDefaultSoundEnable        = true;
    static constexpr sdword_t kDefaultSoundVolume        = 100;

    static constexpr bool     kDefaultSoundBalancer      = false;
    static constexpr bool     kDefaultSoundFilter        = false;

    static constexpr fdword_t kDefaultSpeakerIconScale   = 1;
    static constexpr sdword_t kDefaultSpeakerIconOffsetX = 0;
    static constexpr sdword_t kDefaultSpeakerIconOffsetY = 0;

    static constexpr bool     kDefaultMicroEnable        = true;
    static constexpr sdword_t kDefaultMicroVolume        = 75;

    static constexpr fdword_t kDefaultMicroIconScale     = 1;
    static constexpr sdword_t kDefaultMicroIconPositionX = -100;
    static constexpr sdword_t kDefaultMicroIconPositionY = -100;
    static constexpr D3DCOLOR kDefaultMicroIconColor     = 0x00FFFFFF;
    static constexpr fdword_t kDefaultMicroIconAngle     = 0;

public:

    bool Save(const cstr_t path) const noexcept
    {
        File file { path, "wb" };
        if (file.Invalid())
        {
            Logger::Instance().LogToFile("[sv:inf:pluginconfig:save] failed to "
                "open file for writing (%s)", path);
            return false;
        }

        return file.WritePacket<LittleEndian>(kConfigVersion) == 1 &&
            file.PutLine(_language) &&
            file.WritePacket<LittleEndian>(_sound_enable, _sound_volume, _sound_balancer,
                _sound_filter, _speaker_icon_scale, _speaker_icon_offset_x,
                _speaker_icon_offset_y, _micro_enable, _micro_volume) == 1 &&
            file.PutLine(_micro_device) &&
            file.WritePacket<LittleEndian>(_micro_icon_scale, _micro_icon_position_x,
                _micro_icon_position_y, _micro_icon_color, _micro_icon_angle) == 1;
    }

    bool Load(const cstr_t path) noexcept
    {
        File file { path, "rb" };
        if (file.Invalid())
        {
            Logger::Instance().LogToFile("[sv:inf:pluginconfig:load] failed to "
                "open file for reading (%s)", path);
            return false;
        }

        Version version;

        if (file.ReadPacket<LittleEndian>(version) != 1)
            return false;

        if (version != kConfigVersion)
        {
            Logger::Instance().LogToFile("[sv:inf:pluginconfig:load] unsupported "
                "config version (file:%hhu.%hhu.%hu) (host:%hhu.%hhu.%hu)",
                GetVersionMajor(version), GetVersionMinor(version), GetVersionPatch(version),
                GetVersionMajor(kConfigVersion), GetVersionMinor(kConfigVersion),
                GetVersionPatch(kConfigVersion));
            return false;
        }

        return file.GetLine(_language) == 1 &&
            file.ReadPacket<LittleEndian>(_sound_enable, _sound_volume, _sound_balancer,
                _sound_filter, _speaker_icon_scale, _speaker_icon_offset_x,
                _speaker_icon_offset_y, _micro_enable, _micro_volume) == 1 &&
            file.GetLine(_micro_device) == 1 &&
            file.ReadPacket<LittleEndian>(_micro_icon_scale, _micro_icon_position_x,
                _micro_icon_position_y, _micro_icon_color, _micro_icon_angle) == 1;
    }

public:

    bool HasLanguage() const noexcept
    {
        return *_language != '\0';
    }

    cstr_t GetLanguage() const noexcept
    {
        return _language;
    }

    bool IsSoundEnable() const noexcept
    {
        return _sound_enable;
    }

    sdword_t GetSoundVolume() const noexcept
    {
        return _sound_volume;
    }

    bool IsSoundBalancer() const noexcept
    {
        return _sound_balancer;
    }

    bool IsSoundFilter() const noexcept
    {
        return _sound_filter;
    }

    fdword_t GetSpeakerIconScale() const noexcept
    {
        return _speaker_icon_scale;
    }

    sdword_t GetSpeakerIconOffsetX() const noexcept
    {
        if (fdword_t screen_value; Render::Instance().ConvertBaseXValueToScreenXValue(_speaker_icon_offset_x, screen_value))
            return std::roundf(screen_value);

        return kDefaultSpeakerIconOffsetX;
    }

    sdword_t GetSpeakerIconOffsetY() const noexcept
    {
        if (fdword_t screen_value; Render::Instance().ConvertBaseYValueToScreenYValue(_speaker_icon_offset_y, screen_value))
            return std::roundf(screen_value);

        return kDefaultSpeakerIconOffsetY;
    }

public:

    bool IsMicroEnable() const noexcept
    {
        return _micro_enable;
    }

    sdword_t GetMicroVolume() const noexcept
    {
        return _micro_volume;
    }

    bool HasMicroDevice() const noexcept
    {
        return *_micro_device != '\0';
    }

    cstr_t GetMicroDevice() const noexcept
    {
        return _micro_device;
    }

    fdword_t GetMicroIconScale() const noexcept
    {
        return _micro_icon_scale;
    }

    sdword_t GetMicroIconPositionX() const noexcept
    {
        if (fdword_t screen_value; Render::Instance().ConvertBaseXValueToScreenXValue(_micro_icon_position_x, screen_value))
            return std::roundf(screen_value);

        return kDefaultMicroIconPositionX;
    }

    sdword_t GetMicroIconPositionY() const noexcept
    {
        if (fdword_t screen_value; Render::Instance().ConvertBaseYValueToScreenYValue(_micro_icon_position_y, screen_value))
            return std::roundf(screen_value);

        return kDefaultMicroIconPositionY;
    }

    D3DCOLOR GetMicroIconColor() const noexcept
    {
        return _micro_icon_color;
    }

    fdword_t GetMicroIconAngle() const noexcept
    {
        return _micro_icon_angle;
    }

public:

    void SetLanguage(const cstr_t value) noexcept
    {
        if (const size_t length = std::strlen(value); length < sizeof(_language))
            std::memcpy(_language, value, length + 1);
    }

    void ResetLanguage() noexcept
    {
        *_language = '\0';
    }

    void SetSoundEnable(const bool value) noexcept
    {
        _sound_enable = value;
    }

    void SetSoundVolume(const sdword_t value) noexcept
    {
        _sound_volume = value;
    }

    void SetSoundBalancer(const bool value) noexcept
    {
        _sound_balancer = value;
    }

    void SetSoundFilter(const bool value) noexcept
    {
        _sound_filter = value;
    }

    void SetSpeakerIconScale(const fdword_t value) noexcept
    {
        _speaker_icon_scale = value;
    }

    void SetSpeakerIconOffsetX(const sdword_t value) noexcept
    {
        if (fdword_t base_value; Render::Instance().ConvertScreenXValueToBaseXValue(value, base_value))
            _speaker_icon_offset_x = std::roundf(base_value);
    }

    void SetSpeakerIconOffsetY(const sdword_t value) noexcept
    {
        if (fdword_t base_value; Render::Instance().ConvertScreenYValueToBaseYValue(value, base_value))
            _speaker_icon_offset_y = std::roundf(base_value);
    }

public:

    void SetMicroEnable(const bool value) noexcept
    {
        _micro_enable = value;
    }

    void SetMicroVolume(const sdword_t value) noexcept
    {
        _micro_volume = value;
    }

    void SetMicroDevice(const cstr_t value) noexcept
    {
        if (const size_t length = std::strlen(value); length < sizeof(_micro_device))
            std::memcpy(_micro_device, value, length + 1);
    }

    void ResetMicroDevice() noexcept
    {
        *_micro_device = '\0';
    }

    void SetMicroIconScale(const fdword_t value) noexcept
    {
        _micro_icon_scale = value;
    }

    void SetMicroIconPositionX(const sdword_t value) noexcept
    {
        if (fdword_t base_value; Render::Instance().ConvertScreenXValueToBaseXValue(value, base_value))
            _micro_icon_position_x = std::roundf(base_value);
    }

    void SetMicroIconPositionY(const sdword_t value) noexcept
    {
        if (fdword_t base_value; Render::Instance().ConvertScreenYValueToBaseYValue(value, base_value))
            _micro_icon_position_y = std::roundf(base_value);
    }

    void SetMicroIconColor(const D3DCOLOR value) noexcept
    {
        _micro_icon_color = value;
    }

    void SetMicroIconAngle(const fdword_t value) noexcept
    {
        _micro_icon_angle = value;
    }

private:

    char     _language[64];

    bool     _sound_enable          = kDefaultSoundEnable;
    sdword_t _sound_volume          = kDefaultSoundVolume;

    bool     _sound_balancer        = kDefaultSoundBalancer;
    bool     _sound_filter          = kDefaultSoundFilter;

    fdword_t _speaker_icon_scale    = kDefaultSpeakerIconScale;
    sdword_t _speaker_icon_offset_x = kDefaultSpeakerIconOffsetX;
    sdword_t _speaker_icon_offset_y = kDefaultSpeakerIconOffsetY;

    bool     _micro_enable          = kDefaultMicroEnable;
    sdword_t _micro_volume          = kDefaultMicroVolume;
    char     _micro_device[512];

    fdword_t _micro_icon_scale      = kDefaultMicroIconScale;
    sdword_t _micro_icon_position_x = kDefaultMicroIconPositionX;
    sdword_t _micro_icon_position_y = kDefaultMicroIconPositionY;
    D3DCOLOR _micro_icon_color      = kDefaultMicroIconColor;
    fdword_t _micro_icon_angle      = kDefaultMicroIconAngle;

};
