/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "GameUtil.h"

#include <game/CWorld.h>
#include <game/CCamera.h>
#include <game/CMenuManager.h>
#include <samp/CNetGame.h>

#include "Render.h"
#include "Samp.h"

bool GameUtil::IsKeyPressed(const int keyId) noexcept
{
    return (GetKeyState(keyId) & 0x8000) != 0;
}

bool GameUtil::IsMenuActive() noexcept
{
    return FrontEndMenuManager.m_bMenuActive;
}

bool GameUtil::IsWindowActive() noexcept
{
    HWND gameWindow { nullptr };

    if (!Render::GetWindowHandle(gameWindow))
        return false;

    return gameWindow == GetForegroundWindow();
}

bool GameUtil::IsGameActive() noexcept
{
    return GameUtil::IsWindowActive() &&
          !GameUtil::IsMenuActive();
}

bool GameUtil::HasPlayerPed() noexcept
{
    if (!Samp::IsLoaded()) return false;

    const auto pNetGame = SAMP::pNetGame();
    if (!pNetGame) return false;

    const auto pPlayerPool = pNetGame->GetPlayerPool();
    if (!pPlayerPool) return false;

    const auto pLocalPlayer = pPlayerPool->GetLocalPlayer();
    if (!pLocalPlayer) return false;

    return pLocalPlayer->m_bIsActive;
}

bool GameUtil::IsPlayerVisible(const WORD playerId) noexcept
{
    if (!Samp::IsLoaded()) return false;

    const auto pNetGame = SAMP::pNetGame();
    if (!pNetGame) return false;

    const auto pPlayerPool = pNetGame->GetPlayerPool();
    if (!pPlayerPool) return false;

    const auto pPlayer = pPlayerPool->GetPlayer(playerId);
    if (!pPlayer) return false;

    const auto pPlayerPed = pPlayer->m_pPed;
    if (!pPlayerPed) return false;

    const auto pPlayerGamePed = pPlayerPed->m_pGamePed;
    if (!pPlayerGamePed) return false;

    return pPlayerGamePed->GetIsOnScreen() && CWorld::GetIsLineOfSightClear(
        TheCamera.GetPosition(), pPlayerGamePed->GetPosition(),
        true, false, false, true, true, true, true
    );
}

bool GameUtil::GetRadarRect(CRect& radarRect) noexcept
{
    float screenWidth { 0 };
    float screenHeight { 0 };

    if (!Render::GetScreenSize(screenWidth, screenHeight))
        return false;

    auto multWidth = *reinterpret_cast<float*>(0x859520); // 1.f / 640.f
    auto multHeight = *reinterpret_cast<float*>(0x859524); // 1.f / 448.f
    auto radarWidth = *reinterpret_cast<float*>(0x866b74); // 76.f
    auto radarHeight = *reinterpret_cast<float*>(0x866b78); // 94.f

#if defined(SAMP_R3)

    if (TheCamera.m_bWideScreenOn)
    {
        multWidth = 0.00222f * screenHeight / screenWidth;
        multHeight = 0.00242f;
        radarWidth = 82.f;
        radarHeight = 96.f;
    }

#endif

    const float posY = (1.f - multHeight * 104.f) * screenHeight;

    radarRect.left = (40.f - 4.f) * screenWidth * multWidth;
    radarRect.top = posY - multHeight * screenHeight * 4.f;
    radarRect.right = (40.f + 4.f + radarHeight) * screenWidth * multWidth;
    radarRect.bottom = (4.f + radarWidth) * multHeight * screenHeight + posY;

    return true;
}

void GameUtil::DisableAntiCheat(const AddressesBase& addrBase)
{
    struct patch_t
    {
        const size_t offset;
        const size_t length;
        const char* bytes;
    };

    const std::vector<patch_t> patches =
    {
#define DefinePatch(offset, bytes) { offset, sizeof(bytes) - 1, bytes }
#if defined(SAMP_R1)
        DefinePatch(0x298116, "\xB8\x45\x00\x00\x00\xC2\x1C\x00"),
        DefinePatch(0x286923, "\xB8\x45\x00\x00\x00\xC2\x1C\x00"),
        DefinePatch(0x99250,  "\xC3")
#elif defined(SAMP_R3)
        DefinePatch(0xC4DC0,  "\xB8\x45\x00\x00\x00\xC2\x1C\x00"),
        DefinePatch(0x9D1A0,  "\xC3")
#endif
#undef  DefinePatch
    };

    for (const patch_t& patch : patches)
    {
        const auto address = addrBase.GetBaseAddr() + patch.offset;

        {
            const Memory::UnprotectScope scope { address, patch.length };
            std::memcpy(reinterpret_cast<void*>(address), patch.bytes, patch.length);
        }
    }
}
