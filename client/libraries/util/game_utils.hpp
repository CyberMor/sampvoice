/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cstring>

#include <Windows.h>

#include <memory/hacks/unprotect_scope.hpp>

#include <game/CRect.h>
#include <game/CWorld.h>
#include <game/CCamera.h>
#include <game/CMenuManager.h>
#include <samp/CNetGame.h>

#include "render.hpp"
#include "samp.hpp"

namespace game_utils
{
    inline bool is_key_pressed(const int key) noexcept
    {
        return GetKeyState(key) & 0x8000;
    }

    inline bool is_menu_active() noexcept
    {
        return FrontEndMenuManager.m_bMenuActive;
    }

    inline bool is_window_active() noexcept
    {
        HWND game_window;

        if (!Render::Instance().GetWindowHandle(game_window))
            return false;

        return game_window == GetForegroundWindow();
    }

    inline bool is_game_active() noexcept
    {
        return is_window_active() == true &&
               is_menu_active() == false;
    }

    inline bool has_player_ped() noexcept
    {
        if (!Samp::Instance().IsLoaded())
            return false;

        const auto pNetGame = SAMP::pNetGame();
        if (pNetGame == nullptr) return false;

        const auto pPlayerPool = pNetGame->GetPlayerPool();
        if (pPlayerPool == nullptr) return false;

        const auto pLocalPlayer = pPlayerPool->GetLocalPlayer();
        if (pLocalPlayer == nullptr) return false;

        return pLocalPlayer->m_bIsActive;
    }

    inline bool is_player_visible(const uword_t player) noexcept
    {
        if (!Samp::Instance().IsLoaded())
            return false;

        const auto pNetGame = SAMP::pNetGame();
        if (pNetGame == nullptr) return false;

        const auto pPlayerPool = pNetGame->GetPlayerPool();
        if (pPlayerPool == nullptr) return false;

        const auto pPlayer = pPlayerPool->GetPlayer(player);
        if (pPlayer == nullptr) return false;

        const auto pPlayerPed = pPlayer->m_pPed;
        if (pPlayerPed == nullptr) return false;

        const auto pPlayerGamePed = pPlayerPed->m_pGamePed;
        if (pPlayerGamePed == nullptr) return false;

        return pPlayerGamePed->GetIsOnScreen() && CWorld::GetIsLineOfSightClear
            (TheCamera.GetPosition(), pPlayerGamePed->GetPosition(),
            true, false, false, true, true, true, true);
    }

    inline bool get_radar_rectangle(CRect& rectangle) noexcept
    {
        float screen_width, screen_height;

        if (!Render::Instance().GetScreenSize(screen_width, screen_height))
            return false;

        float mult_width   = *reinterpret_cast<float*>(0x859520); // 1 / 640
        float mult_height  = *reinterpret_cast<float*>(0x859524); // 1 / 448
        float radar_width  = *reinterpret_cast<float*>(0x866B74); // 76
        float radar_height = *reinterpret_cast<float*>(0x866B78); // 94

#if defined(SAMP_R3)
        if (TheCamera.m_bWideScreenOn)
        {
            mult_width   = 0.00222F * screen_height / screen_width;
            mult_height  = 0.00242F;
            radar_width  = 82;
            radar_height = 96;
        }
#endif

        const float position_y = (1 - mult_height * 104) * screen_height;

        rectangle.left   = (40 - 4) * screen_width * mult_width;
        rectangle.top    = position_y - mult_height * screen_height * 4;
        rectangle.right  = (40 + 4 + radar_height) * screen_width * mult_width;
        rectangle.bottom = (4 + radar_width) * mult_height * screen_height + position_y;

        return true;
    }

#define ApplyPatch(base, offset, bytes)                        \
{                                                              \
    const ptr_t address = static_cast<adr_t>(base) + (offset); \
    const UnprotectScope<sizeof(bytes) - 1> scope { address }; \
    std::memcpy(address, bytes, sizeof(bytes) - 1);            \
}

    inline void disable_anticheat(const ptr_t base) noexcept
    {
#if defined(SAMP_R1)
        ApplyPatch(base, 0x298116, "\xB8\x45\x00\x00\x00\xC2\x1C\x00");
        ApplyPatch(base, 0x286923, "\xB8\x45\x00\x00\x00\xC2\x1C\x00");
        ApplyPatch(base, 0x99250,  "\xC3");
#elif defined(SAMP_R3)
        ApplyPatch(base, 0xC4DC0,  "\xB8\x45\x00\x00\x00\xC2\x1C\x00");
        ApplyPatch(base, 0x9D1A0,  "\xC3");
#else
#error Unknown SAMP version
#endif
    }

#undef ApplyPatch
}
