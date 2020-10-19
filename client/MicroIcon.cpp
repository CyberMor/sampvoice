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

bool MicroIcon::Init(IDirect3DDevice9* const pDevice, const Resource& rPassiveIcon,
    const Resource& rActiveIcon, const Resource& rMutedIcon) noexcept
{
    if (pDevice == nullptr)
        return false;

    if (MicroIcon::initStatus)
        return false;

    try
    {
        MicroIcon::tPassiveIcon = MakeTexture(pDevice, rPassiveIcon);
        MicroIcon::tActiveIcon = MakeTexture(pDevice, rActiveIcon);
        MicroIcon::tMutedIcon = MakeTexture(pDevice, rMutedIcon);
    }
    catch (const std::exception& exception)
    {
        Logger::LogToFile("[sv:err:microicon:init] : failed to create icons");
        MicroIcon::tPassiveIcon.reset();
        MicroIcon::tActiveIcon.reset();
        MicroIcon::tMutedIcon.reset();
        return false;
    }

    MicroIcon::SetPassiveIcon();

    if (!PluginConfig::IsMicroLoaded())
    {
        PluginConfig::SetMicroLoaded(true);
        MicroIcon::ResetConfigs();
    }

    MicroIcon::initStatus = true;
    MicroIcon::SyncConfigs();

    return true;
}

void MicroIcon::Free() noexcept
{
    if (!MicroIcon::initStatus)
        return;

    MicroIcon::tPassiveIcon.reset();
    MicroIcon::tActiveIcon.reset();
    MicroIcon::tMutedIcon.reset();

    MicroIcon::initStatus = false;
}

void MicroIcon::Show() noexcept
{
    MicroIcon::showStatus = true;
}

bool MicroIcon::IsShowed() noexcept
{
    return MicroIcon::showStatus;
}

void MicroIcon::Hide() noexcept
{
    MicroIcon::showStatus = false;
}

void MicroIcon::SetPassiveIcon() noexcept
{
    MicroIcon::alphaLevelActiveIcon = 0;
    MicroIcon::alphaLevelDeviationActiveIcon = 0;
    MicroIcon::alphaLevelMutedIcon = 0;
    MicroIcon::alphaLevelDeviationMutedIcon = 0;

    MicroIcon::alphaLevelPassiveIcon = 255;
    MicroIcon::alphaLevelDeviationPassiveIcon = 0;
}

void MicroIcon::SetActiveIcon() noexcept
{
    MicroIcon::alphaLevelPassiveIcon = 0;
    MicroIcon::alphaLevelDeviationPassiveIcon = 0;
    MicroIcon::alphaLevelMutedIcon = 0;
    MicroIcon::alphaLevelDeviationMutedIcon = 0;

    MicroIcon::alphaLevelActiveIcon = 255;
    MicroIcon::alphaLevelDeviationActiveIcon = 0;
}

void MicroIcon::SetMutedIcon() noexcept
{
    MicroIcon::alphaLevelPassiveIcon = 0;
    MicroIcon::alphaLevelDeviationPassiveIcon = 0;
    MicroIcon::alphaLevelActiveIcon = 0;
    MicroIcon::alphaLevelDeviationActiveIcon = 0;

    MicroIcon::alphaLevelMutedIcon = 255;
    MicroIcon::alphaLevelDeviationMutedIcon = 0;
}

void MicroIcon::SwitchToPassiveIcon() noexcept
{
    MicroIcon::alphaLevelDeviationActiveIcon = kAlphaLevelDecrementDeviation;
    MicroIcon::alphaLevelDeviationMutedIcon = kAlphaLevelDecrementDeviation;

    MicroIcon::alphaLevelDeviationPassiveIcon = kAlphaLevelIncrementDeviation;
}

void MicroIcon::SwitchToActiveIcon() noexcept
{
    MicroIcon::alphaLevelDeviationPassiveIcon = kAlphaLevelDecrementDeviation;
    MicroIcon::alphaLevelDeviationMutedIcon = kAlphaLevelDecrementDeviation;

    MicroIcon::alphaLevelDeviationActiveIcon = kAlphaLevelIncrementDeviation;
}

void MicroIcon::SwitchToMutedIcon() noexcept
{
    MicroIcon::alphaLevelDeviationPassiveIcon = kAlphaLevelDecrementDeviation;
    MicroIcon::alphaLevelDeviationActiveIcon = kAlphaLevelDecrementDeviation;

    MicroIcon::alphaLevelDeviationMutedIcon = kAlphaLevelIncrementDeviation;
}

void MicroIcon::Render() noexcept
{
    if (!MicroIcon::initStatus)
        return;

    if (!MicroIcon::showStatus)
        return;

    float vIconSize { 0.f };

    if (!Render::ConvertBaseYValueToScreenYValue(kBaseIconSize, vIconSize))
        return;

    vIconSize *= PluginConfig::GetMicroIconScale();

    float vIconX = PluginConfig::GetMicroIconPositionX();
    float vIconY = PluginConfig::GetMicroIconPositionY();

    if (vIconX < 0.f || vIconY < 0.f)
    {
        CRect radarRect;

        if (!GameUtil::GetRadarRect(radarRect))
            return;

        float vIconPadding { 0.f };

        if (!Render::ConvertBaseYValueToScreenYValue(kBaseIconPadding, vIconPadding))
            return;

        vIconX = radarRect.left + (radarRect.right - radarRect.left) / 2.f;
        vIconY = radarRect.top - (vIconPadding + vIconSize / 2.f);
    }

    vIconX -= vIconSize / 2.f;
    vIconY -= vIconSize / 2.f;

    if (MicroIcon::alphaLevelPassiveIcon > 0)
    {
        MicroIcon::tPassiveIcon->Draw(vIconX, vIconY, vIconSize, vIconSize, (PluginConfig::GetMicroIconColor() &
            0x00ffffff) | (MicroIcon::alphaLevelPassiveIcon << 24), PluginConfig::GetMicroIconAngle());
    }

    if (MicroIcon::alphaLevelActiveIcon > 0)
    {
        MicroIcon::tActiveIcon->Draw(vIconX, vIconY, vIconSize, vIconSize, (PluginConfig::GetMicroIconColor() &
            0x00ffffff) | (MicroIcon::alphaLevelActiveIcon << 24), PluginConfig::GetMicroIconAngle());
    }

    if (MicroIcon::alphaLevelMutedIcon > 0)
    {
        MicroIcon::tMutedIcon->Draw(vIconX, vIconY, vIconSize, vIconSize, (PluginConfig::GetMicroIconColor() &
            0x00ffffff) | (MicroIcon::alphaLevelMutedIcon << 24), PluginConfig::GetMicroIconAngle());
    }
}

void MicroIcon::Update() noexcept
{
    if (MicroIcon::alphaLevelDeviationPassiveIcon != 0)
    {
        MicroIcon::alphaLevelPassiveIcon += MicroIcon::alphaLevelDeviationPassiveIcon;

        if (MicroIcon::alphaLevelPassiveIcon < 0)
        {
            MicroIcon::alphaLevelDeviationPassiveIcon = 0;
            MicroIcon::alphaLevelPassiveIcon = 0;
        }
        else if (MicroIcon::alphaLevelPassiveIcon > 255)
        {
            MicroIcon::alphaLevelDeviationPassiveIcon = 0;
            MicroIcon::alphaLevelPassiveIcon = 255;
        }
    }

    if (MicroIcon::alphaLevelDeviationActiveIcon != 0)
    {
        MicroIcon::alphaLevelActiveIcon += MicroIcon::alphaLevelDeviationActiveIcon;

        if (MicroIcon::alphaLevelActiveIcon < 0)
        {
            MicroIcon::alphaLevelDeviationActiveIcon = 0;
            MicroIcon::alphaLevelActiveIcon = 0;
        }
        else if (MicroIcon::alphaLevelActiveIcon > 255)
        {
            MicroIcon::alphaLevelDeviationActiveIcon = 0;
            MicroIcon::alphaLevelActiveIcon = 255;
        }
    }

    if (MicroIcon::alphaLevelDeviationMutedIcon != 0)
    {
        MicroIcon::alphaLevelMutedIcon += MicroIcon::alphaLevelDeviationMutedIcon;

        if (MicroIcon::alphaLevelMutedIcon < 0)
        {
            MicroIcon::alphaLevelDeviationMutedIcon = 0;
            MicroIcon::alphaLevelMutedIcon = 0;
        }
        else if (MicroIcon::alphaLevelMutedIcon > 255)
        {
            MicroIcon::alphaLevelDeviationMutedIcon = 0;
            MicroIcon::alphaLevelMutedIcon = 255;
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
    MicroIcon::SetMicroIconScale(PluginConfig::GetMicroIconScale());
    MicroIcon::SetMicroIconPositionX(PluginConfig::GetMicroIconPositionX());
    MicroIcon::SetMicroIconPositionY(PluginConfig::GetMicroIconPositionY());
    MicroIcon::SetMicroIconColor(PluginConfig::GetMicroIconColor());
    MicroIcon::SetMicroIconAngle(PluginConfig::GetMicroIconAngle());
}

bool MicroIcon::initStatus { false };
bool MicroIcon::showStatus { false };

int MicroIcon::alphaLevelPassiveIcon { 0 };
int MicroIcon::alphaLevelDeviationPassiveIcon { 0 };
TexturePtr MicroIcon::tPassiveIcon { nullptr };

int MicroIcon::alphaLevelActiveIcon { 0 };
int MicroIcon::alphaLevelDeviationActiveIcon { 0 };
TexturePtr MicroIcon::tActiveIcon { nullptr };

int MicroIcon::alphaLevelMutedIcon { 0 };
int MicroIcon::alphaLevelDeviationMutedIcon { 0 };
TexturePtr MicroIcon::tMutedIcon { nullptr };
