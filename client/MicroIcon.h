/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <d3d9.h>

#include <util/Resource.h>
#include <util/Texture.h>

struct MicroIcon {

    MicroIcon() = delete;
    ~MicroIcon() = delete;
    MicroIcon(const MicroIcon&) = delete;
    MicroIcon(MicroIcon&&) = delete;
    MicroIcon& operator=(const MicroIcon&) = delete;
    MicroIcon& operator=(MicroIcon&&) = delete;

private:

    static constexpr float kBaseIconSize                 =  35.f;
    static constexpr float kBaseIconPadding              =  15.f;
    static constexpr int   kAlphaLevelIncrementDeviation =  12;
    static constexpr int   kAlphaLevelDecrementDeviation = -12;

public:

    static bool Init(IDirect3DDevice9* device,
        const Resource& resource_passive_icon,
        const Resource& resource_active_icon,
        const Resource& resource_muted_icon) noexcept;
    static void Free() noexcept;

    static void Show() noexcept;
    static bool IsShowed() noexcept;
    static void Hide() noexcept;

    static void Render() noexcept;
    static void Update() noexcept;

    static void SetPassiveIcon() noexcept;
    static void SetActiveIcon() noexcept;
    static void SetMutedIcon() noexcept;

    static void SwitchToPassiveIcon() noexcept;
    static void SwitchToActiveIcon() noexcept;
    static void SwitchToMutedIcon() noexcept;

    static int GetMicroIconPositionX() noexcept;
    static int GetMicroIconPositionY() noexcept;
    static D3DCOLOR GetMicroIconColor() noexcept;
    static float GetMicroIconAngle() noexcept;
    static float GetMicroIconScale() noexcept;

    static void SetMicroIconPositionX(int x) noexcept;
    static void SetMicroIconPositionY(int y) noexcept;
    static void SetMicroIconPosition(int x, int y) noexcept;
    static void SetMicroIconColor(D3DCOLOR color) noexcept;
    static void SetMicroIconAngle(float angle) noexcept;
    static void SetMicroIconScale(float scale) noexcept;

    static void ResetConfigs() noexcept;
    static void SyncConfigs() noexcept;

private:

    static bool    _init_status;
    static bool    _show_status;

    static int     _alpha_level_passive_icon;
    static int     _alpha_level_deviation_passive_icon;
    static Texture _texture_passive_icon;

    static int     _alpha_level_active_icon;
    static int     _alpha_level_deviation_active_icon;
    static Texture _texture_active_icon;

    static int     _alpha_level_muted_icon;
    static int     _alpha_level_deviation_muted_icon;
    static Texture _texture_muted_icon;

};
