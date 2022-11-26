/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "MicroIcon.h"

#include <algorithm>

#include <util/Logger.h>
#include <util/GameUtil.h>
#include <util/Render.h>

#include "PluginConfig.h"

bool MicroIcon::Init(IDirect3DDevice9* const device, const Resource& resource_passive_icon,
    const Resource& resource_active_icon, const Resource& resource_muted_icon) noexcept
{
    if (device == nullptr) return false;

    if (_init_status) return false;

    _texture_passive_icon = { device, resource_passive_icon };
    _texture_active_icon = { device, resource_active_icon };
    _texture_muted_icon = { device, resource_muted_icon };

    if (!_texture_passive_icon.Valid() ||
        !_texture_active_icon.Valid() ||
        !_texture_muted_icon.Valid())
    {
        Logger::LogToFile("[sv:err:microicon:init] : failed to create icons");
        _texture_passive_icon = {};
        _texture_active_icon = {};
        _texture_muted_icon = {};
        return false;
    }

    SetPassiveIcon();

    if (!PluginConfig::IsMicroLoaded())
    {
        PluginConfig::SetMicroLoaded(true);
        ResetConfigs();
    }

    _init_status = true;
    SyncConfigs();

    return true;
}

void MicroIcon::Free() noexcept
{
    if (_init_status)
    {
        _texture_passive_icon = {};
        _texture_active_icon = {};
        _texture_muted_icon = {};

        _init_status = false;
    }
}

void MicroIcon::Show() noexcept
{
    _show_status = true;
}

bool MicroIcon::IsShowed() noexcept
{
    return _show_status;
}

void MicroIcon::Hide() noexcept
{
    _show_status = false;
}

void MicroIcon::SetPassiveIcon() noexcept
{
    _alpha_level_active_icon            = 0;
    _alpha_level_deviation_active_icon  = 0;
    _alpha_level_muted_icon             = 0;
    _alpha_level_deviation_muted_icon   = 0;

    _alpha_level_passive_icon           = 255;
    _alpha_level_deviation_passive_icon = 0;
}

void MicroIcon::SetActiveIcon() noexcept
{
    _alpha_level_passive_icon           = 0;
    _alpha_level_deviation_passive_icon = 0;
    _alpha_level_muted_icon             = 0;
    _alpha_level_deviation_muted_icon   = 0;

    _alpha_level_active_icon            = 255;
    _alpha_level_deviation_active_icon  = 0;
}

void MicroIcon::SetMutedIcon() noexcept
{
    _alpha_level_passive_icon           = 0;
    _alpha_level_deviation_passive_icon = 0;
    _alpha_level_active_icon            = 0;
    _alpha_level_deviation_active_icon  = 0;

    _alpha_level_muted_icon             = 255;
    _alpha_level_deviation_muted_icon   = 0;
}

void MicroIcon::SwitchToPassiveIcon() noexcept
{
    _alpha_level_deviation_active_icon  = kAlphaLevelDecrementDeviation;
    _alpha_level_deviation_muted_icon   = kAlphaLevelDecrementDeviation;

    _alpha_level_deviation_passive_icon = kAlphaLevelIncrementDeviation;
}

void MicroIcon::SwitchToActiveIcon() noexcept
{
    _alpha_level_deviation_passive_icon = kAlphaLevelDecrementDeviation;
    _alpha_level_deviation_muted_icon   = kAlphaLevelDecrementDeviation;

    _alpha_level_deviation_active_icon  = kAlphaLevelIncrementDeviation;
}

void MicroIcon::SwitchToMutedIcon() noexcept
{
    _alpha_level_deviation_passive_icon = kAlphaLevelDecrementDeviation;
    _alpha_level_deviation_active_icon  = kAlphaLevelDecrementDeviation;

    _alpha_level_deviation_muted_icon   = kAlphaLevelIncrementDeviation;
}

void MicroIcon::Render() noexcept
{
    if (_init_status && _show_status)
    {
        float icon_size = 0.f;

        if (!Render::ConvertBaseYValueToScreenYValue(kBaseIconSize, icon_size))
            return;

        icon_size *= PluginConfig::GetMicroIconScale();

        float icon_x = PluginConfig::GetMicroIconPositionX();
        float icon_y = PluginConfig::GetMicroIconPositionY();

        if (icon_x < 0.f || icon_y < 0.f)
        {
            CRect radar_rect;

            if (!GameUtil::GetRadarRect(radar_rect))
                return;

            float icon_padding = 0.f;

            if (!Render::ConvertBaseYValueToScreenYValue(kBaseIconPadding, icon_padding))
                return;

            icon_x = radar_rect.left + (radar_rect.right - radar_rect.left) / 2.f;
            icon_y = radar_rect.top - (icon_padding + icon_size / 2.f);
        }

        icon_x -= icon_size / 2.f;
        icon_y -= icon_size / 2.f;

        if (_alpha_level_passive_icon > 0)
        {
            _texture_passive_icon->Draw(icon_x, icon_y, icon_size, icon_size, (PluginConfig::GetMicroIconColor() &
                0x00ffffff) | (_alpha_level_passive_icon << 24), PluginConfig::GetMicroIconAngle());
        }

        if (_alpha_level_active_icon > 0)
        {
            _texture_active_icon->Draw(icon_x, icon_y, icon_size, icon_size, (PluginConfig::GetMicroIconColor() &
                0x00ffffff) | (_alpha_level_active_icon << 24), PluginConfig::GetMicroIconAngle());
        }

        if (_alpha_level_muted_icon > 0)
        {
            _texture_muted_icon->Draw(icon_x, icon_y, icon_size, icon_size, (PluginConfig::GetMicroIconColor() &
                0x00ffffff) | (_alpha_level_muted_icon << 24), PluginConfig::GetMicroIconAngle());
        }
    }
}

void MicroIcon::Update() noexcept
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

int MicroIcon::GetMicroIconPositionX() noexcept
{
    return PluginConfig::GetMicroIconPositionX();
}

int MicroIcon::GetMicroIconPositionY() noexcept
{
    return PluginConfig::GetMicroIconPositionY();
}

D3DCOLOR MicroIcon::GetMicroIconColor() noexcept
{
    return PluginConfig::GetMicroIconColor();
}

float MicroIcon::GetMicroIconAngle() noexcept
{
    return PluginConfig::GetMicroIconAngle();
}

float MicroIcon::GetMicroIconScale() noexcept
{
    return PluginConfig::GetMicroIconScale();
}

void MicroIcon::SetMicroIconPositionX(const int x) noexcept
{
    PluginConfig::SetMicroIconPositionX(x);
}

void MicroIcon::SetMicroIconPositionY(const int y) noexcept
{
    PluginConfig::SetMicroIconPositionY(y);
}

void MicroIcon::SetMicroIconPosition(const int x, const int y) noexcept
{
    PluginConfig::SetMicroIconPositionX(x);
    PluginConfig::SetMicroIconPositionY(y);
}

void MicroIcon::SetMicroIconColor(const D3DCOLOR color) noexcept
{
    PluginConfig::SetMicroIconColor(color);
}

void MicroIcon::SetMicroIconAngle(const float angle) noexcept
{
    PluginConfig::SetMicroIconAngle(angle);
}

void MicroIcon::SetMicroIconScale(const float scale) noexcept
{
    PluginConfig::SetMicroIconScale(std::clamp(scale, 0.2f, 2.f));
}

void MicroIcon::ResetConfigs() noexcept
{
    PluginConfig::SetMicroIconScale(PluginConfig::kDefValMicroIconScale);
    PluginConfig::SetMicroIconPositionX(PluginConfig::kDefValMicroIconPositionX);
    PluginConfig::SetMicroIconPositionY(PluginConfig::kDefValMicroIconPositionY);
    PluginConfig::SetMicroIconColor(PluginConfig::kDefValMicroIconColor);
    PluginConfig::SetMicroIconAngle(PluginConfig::kDefValMicroIconAngle);
}

void MicroIcon::SyncConfigs() noexcept
{
    SetMicroIconScale(PluginConfig::GetMicroIconScale());
    SetMicroIconPositionX(PluginConfig::GetMicroIconPositionX());
    SetMicroIconPositionY(PluginConfig::GetMicroIconPositionY());
    SetMicroIconColor(PluginConfig::GetMicroIconColor());
    SetMicroIconAngle(PluginConfig::GetMicroIconAngle());
}

bool    MicroIcon::_init_status = false;
bool    MicroIcon::_show_status = false;

int     MicroIcon::_alpha_level_passive_icon = 0;
int     MicroIcon::_alpha_level_deviation_passive_icon = 0;
Texture MicroIcon::_texture_passive_icon;

int     MicroIcon::_alpha_level_active_icon = 0;
int     MicroIcon::_alpha_level_deviation_active_icon = 0;
Texture MicroIcon::_texture_active_icon;

int     MicroIcon::_alpha_level_muted_icon = 0;
int     MicroIcon::_alpha_level_deviation_muted_icon = 0;
Texture MicroIcon::_texture_muted_icon;
