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

class MicroIcon {

    MicroIcon() = delete;
    ~MicroIcon() = delete;
    MicroIcon(const MicroIcon&) = delete;
    MicroIcon(MicroIcon&&) = delete;
    MicroIcon& operator=(const MicroIcon&) = delete;
    MicroIcon& operator=(MicroIcon&&) = delete;

private:

    static constexpr float kBaseIconSize = 35.f;
    static constexpr float kBaseIconPadding = 15.f;
    static constexpr int kAlphaLevelIncrementDeviation = 12;
    static constexpr int kAlphaLevelDecrementDeviation = -12;

public:

    static bool Init(IDirect3DDevice9* pDevice, const Resource& rPassiveIcon,
        const Resource& rActiveIcon, const Resource& rMutedIcon) noexcept;
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

    static bool initStatus;
    static bool showStatus;

    static int alphaLevelPassiveIcon;
    static int alphaLevelDeviationPassiveIcon;
    static TexturePtr tPassiveIcon;

    static int alphaLevelActiveIcon;
    static int alphaLevelDeviationActiveIcon;
    static TexturePtr tActiveIcon;

    static int alphaLevelMutedIcon;
    static int alphaLevelDeviationMutedIcon;
    static TexturePtr tMutedIcon;

};
