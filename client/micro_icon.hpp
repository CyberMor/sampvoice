/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cassert>
#include <algorithm>

#include <d3d9.h>

#include <memory/view.hpp>

#include <util/logger.hpp>
#include <util/texture.hpp>
#include <util/game_utils.hpp>
#include <util/render.hpp>

#include "plugin_config.hpp"

struct MicroIcon {

    MicroIcon(const MicroIcon&) = delete;
    MicroIcon(MicroIcon&&) = delete;
    MicroIcon& operator=(const MicroIcon&) = delete;
    MicroIcon& operator=(MicroIcon&&) = delete;

private:

    MicroIcon() noexcept = default;
    ~MicroIcon() noexcept = default;

public:

    static MicroIcon& Instance() noexcept
    {
        static MicroIcon instance;
        return instance;
    }

private:

    static constexpr float kBaseIconSize                 =  35;
    static constexpr float kBaseIconPadding              =  15;
    static constexpr  int  kAlphaLevelIncrementDeviation =  12;
    static constexpr  int  kAlphaLevelDecrementDeviation = -12;

public:

    bool Initialize(IDirect3DDevice9* const device, const DataView<ubyte_t>& resource_passive_icon,
        const DataView<ubyte_t>& resource_active_icon, const DataView<ubyte_t>& resource_muted_icon) noexcept
    {
        assert(device != nullptr);

        if (!_texture_passive_icon.Initialize(device, resource_passive_icon) ||
            !_texture_active_icon.Initialize(device, resource_active_icon) ||
            !_texture_muted_icon.Initialize(device, resource_muted_icon))
        {
            Logger::Instance().LogToFile("[sv:err:microicon:initialize] : failed to create icons");
            _texture_passive_icon.Deinitialize();
            _texture_active_icon.Deinitialize();
            _texture_muted_icon.Deinitialize();
            return false;
        }

        SetPassiveIcon();
        SyncConfigs();
        Show();

        return true;
    }

    void Deinitialize() noexcept
    {
        _texture_passive_icon.Deinitialize();
        _texture_active_icon.Deinitialize();
        _texture_muted_icon.Deinitialize();
    }

public:

    void Show() noexcept
    {
        _is_showing = true;
    }

    bool IsShowed() noexcept
    {
        return _is_showing;
    }

    void Hide() noexcept
    {
        _is_showing = false;
    }

public:

    void Render() noexcept
    {
        if (_is_showing)
        {
            float icon_size;

            if (!Render::Instance().ConvertBaseYValueToScreenYValue(kBaseIconSize, icon_size))
                return;

            icon_size *= PluginConfig::Instance().GetMicroIconScale();

            float icon_x = PluginConfig::Instance().GetMicroIconPositionX();
            float icon_y = PluginConfig::Instance().GetMicroIconPositionY();

            if (icon_x < 0 || icon_y < 0)
            {
                CRect radar_rect;

                if (!game_utils::get_radar_rectangle(radar_rect))
                    return;

                float icon_padding;

                if (!Render::Instance().ConvertBaseYValueToScreenYValue(kBaseIconPadding, icon_padding))
                    return;

                icon_x = radar_rect.left + (radar_rect.right - radar_rect.left) / 2;
                icon_y = radar_rect.top - (icon_padding + icon_size / 2);
            }

            icon_x -= icon_size / 2;
            icon_y -= icon_size / 2;

            if (_alpha_level_passive_icon > 0)
            {
                _texture_passive_icon.Draw(icon_x, icon_y, icon_size, icon_size,
                    (PluginConfig::Instance().GetMicroIconColor() & 0x00FFFFFF) |
                    (_alpha_level_passive_icon << 24), PluginConfig::Instance().GetMicroIconAngle());
            }
            if (_alpha_level_active_icon > 0)
            {
                _texture_active_icon.Draw(icon_x, icon_y, icon_size, icon_size,
                    (PluginConfig::Instance().GetMicroIconColor() & 0x00FFFFFF) |
                    (_alpha_level_active_icon << 24), PluginConfig::Instance().GetMicroIconAngle());
            }
            if (_alpha_level_muted_icon > 0)
            {
                _texture_muted_icon.Draw(icon_x, icon_y, icon_size, icon_size,
                    (PluginConfig::Instance().GetMicroIconColor() & 0x00FFFFFF) |
                    (_alpha_level_muted_icon << 24), PluginConfig::Instance().GetMicroIconAngle());
            }
        }
    }

    void Update() noexcept
    {
        if (_alpha_level_deviation_passive_icon != 0)
        {
            _alpha_level_passive_icon += _alpha_level_deviation_passive_icon;

            if (_alpha_level_passive_icon < 0)
            {
                _alpha_level_deviation_passive_icon = 0;
                _alpha_level_passive_icon = 0;
            }
            else if (_alpha_level_passive_icon > 255)
            {
                _alpha_level_deviation_passive_icon = 0;
                _alpha_level_passive_icon = 255;
            }
        }
        if (_alpha_level_deviation_active_icon != 0)
        {
            _alpha_level_active_icon += _alpha_level_deviation_active_icon;

            if (_alpha_level_active_icon < 0)
            {
                _alpha_level_deviation_active_icon = 0;
                _alpha_level_active_icon = 0;
            }
            else if (_alpha_level_active_icon > 255)
            {
                _alpha_level_deviation_active_icon = 0;
                _alpha_level_active_icon = 255;
            }
        }
        if (_alpha_level_deviation_muted_icon != 0)
        {
            _alpha_level_muted_icon += _alpha_level_deviation_muted_icon;

            if (_alpha_level_muted_icon < 0)
            {
                _alpha_level_deviation_muted_icon = 0;
                _alpha_level_muted_icon = 0;
            }
            else if (_alpha_level_muted_icon > 255)
            {
                _alpha_level_deviation_muted_icon = 0;
                _alpha_level_muted_icon = 255;
            }
        }
    }

public:

    void SetPassiveIcon() noexcept
    {
        _alpha_level_active_icon            = 0;
        _alpha_level_deviation_active_icon  = 0;
        _alpha_level_muted_icon             = 0;
        _alpha_level_deviation_muted_icon   = 0;

        _alpha_level_passive_icon           = 255;
        _alpha_level_deviation_passive_icon = 0;
    }

    void SetActiveIcon() noexcept
    {
        _alpha_level_passive_icon           = 0;
        _alpha_level_deviation_passive_icon = 0;
        _alpha_level_muted_icon             = 0;
        _alpha_level_deviation_muted_icon   = 0;

        _alpha_level_active_icon            = 255;
        _alpha_level_deviation_active_icon  = 0;
    }

    void SetMutedIcon() noexcept
    {
        _alpha_level_passive_icon           = 0;
        _alpha_level_deviation_passive_icon = 0;
        _alpha_level_active_icon            = 0;
        _alpha_level_deviation_active_icon  = 0;

        _alpha_level_muted_icon             = 255;
        _alpha_level_deviation_muted_icon   = 0;
    }

public:

    void SwitchToPassiveIcon() noexcept
    {
        _alpha_level_deviation_active_icon  = kAlphaLevelDecrementDeviation;
        _alpha_level_deviation_muted_icon   = kAlphaLevelDecrementDeviation;

        _alpha_level_deviation_passive_icon = kAlphaLevelIncrementDeviation;
    }

    void SwitchToActiveIcon() noexcept
    {
        _alpha_level_deviation_passive_icon = kAlphaLevelDecrementDeviation;
        _alpha_level_deviation_muted_icon   = kAlphaLevelDecrementDeviation;

        _alpha_level_deviation_active_icon  = kAlphaLevelIncrementDeviation;
    }

    void SwitchToMutedIcon() noexcept
    {
        _alpha_level_deviation_passive_icon = kAlphaLevelDecrementDeviation;
        _alpha_level_deviation_active_icon  = kAlphaLevelDecrementDeviation;

        _alpha_level_deviation_muted_icon   = kAlphaLevelIncrementDeviation;
    }

public:

    sdword_t GetMicroIconPositionX() const noexcept
    {
        return PluginConfig::Instance().GetMicroIconPositionX();
    }

    sdword_t GetMicroIconPositionY() const noexcept
    {
        return PluginConfig::Instance().GetMicroIconPositionY();
    }

    D3DCOLOR GetMicroIconColor() const noexcept
    {
        return PluginConfig::Instance().GetMicroIconColor();
    }

    fdword_t GetMicroIconAngle() const noexcept
    {
        return PluginConfig::Instance().GetMicroIconAngle();
    }

    fdword_t GetMicroIconScale() const noexcept
    {
        return PluginConfig::Instance().GetMicroIconScale();
    }

public:

    void SetMicroIconPositionX(const sdword_t x) noexcept
    {
        PluginConfig::Instance().SetMicroIconPositionX(x);
    }

    void SetMicroIconPositionY(const sdword_t y) noexcept
    {
        PluginConfig::Instance().SetMicroIconPositionY(y);
    }

    void SetMicroIconPosition(const sdword_t x, const sdword_t y) noexcept
    {
        PluginConfig::Instance().SetMicroIconPositionX(x);
        PluginConfig::Instance().SetMicroIconPositionY(y);
    }

    void SetMicroIconColor(const D3DCOLOR color) noexcept
    {
        PluginConfig::Instance().SetMicroIconColor(color);
    }

    void SetMicroIconAngle(const fdword_t angle) noexcept
    {
        PluginConfig::Instance().SetMicroIconAngle(angle);
    }

    void SetMicroIconScale(const fdword_t scale) noexcept
    {
        PluginConfig::Instance().SetMicroIconScale(std::clamp(scale, 0.2F, 2.0F));
    }

public:

    void ResetConfigs() noexcept
    {
        PluginConfig::Instance().SetMicroIconScale(PluginConfig::kDefaultMicroIconScale);
        PluginConfig::Instance().SetMicroIconPositionX(PluginConfig::kDefaultMicroIconPositionX);
        PluginConfig::Instance().SetMicroIconPositionY(PluginConfig::kDefaultMicroIconPositionY);
        PluginConfig::Instance().SetMicroIconColor(PluginConfig::kDefaultMicroIconColor);
        PluginConfig::Instance().SetMicroIconAngle(PluginConfig::kDefaultMicroIconAngle);
    }

    void SyncConfigs() noexcept
    {
        SetMicroIconScale(PluginConfig::Instance().GetMicroIconScale());
        SetMicroIconPositionX(PluginConfig::Instance().GetMicroIconPositionX());
        SetMicroIconPositionY(PluginConfig::Instance().GetMicroIconPositionY());
        SetMicroIconColor(PluginConfig::Instance().GetMicroIconColor());
        SetMicroIconAngle(PluginConfig::Instance().GetMicroIconAngle());
    }

private:

    bool    _is_showing = false;

private:

    int     _alpha_level_passive_icon = 0;
    int     _alpha_level_deviation_passive_icon = 0;
    Texture _texture_passive_icon;

    int     _alpha_level_active_icon = 0;
    int     _alpha_level_deviation_active_icon = 0;
    Texture _texture_active_icon;

    int     _alpha_level_muted_icon = 0;
    int     _alpha_level_deviation_muted_icon = 0;
    Texture _texture_muted_icon;

};
