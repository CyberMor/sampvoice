/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "GameUtil.h"

#include <cstring>

#include <game/CWorld.h>
#include <game/CCamera.h>
#include <game/CMenuManager.h>
#include <samp/CNetGame.h>

#include "Render.h"
#include "Samp.h"

bool GameUtil::IsKeyPressed(const int key_id) noexcept
{
    return GetKeyState(key_id) & 0x8000;
}

bool GameUtil::IsMenuActive() noexcept
{
    return FrontEndMenuManager.m_bMenuActive;
}

bool GameUtil::IsWindowActive() noexcept
{
    HWND game_window;
    if (!Render::GetWindowHandle(game_window)) return false;
    return game_window == GetForegroundWindow();
}

bool GameUtil::IsGameActive() noexcept
{
    return GameUtil::IsWindowActive() && !GameUtil::IsMenuActive();
}

bool GameUtil::HasPlayerPed() noexcept
{
    if (!Samp::IsLoaded()) return false;

    const auto net_game = SAMP::pNetGame();
    if (net_game == nullptr) return false;

    const auto player_pool = net_game->GetPlayerPool();
    if (player_pool == nullptr) return false;

    const auto local_player = player_pool->GetLocalPlayer();
    if (local_player == nullptr) return false;

    return local_player->m_bIsActive;
}

bool GameUtil::IsPlayerVisible(const WORD player_id) noexcept
{
    if (!Samp::IsLoaded()) return false;

    const auto net_game = SAMP::pNetGame();
    if (net_game == nullptr) return false;

    const auto player_pool = net_game->GetPlayerPool();
    if (player_pool == nullptr) return false;

    const auto player = player_pool->GetPlayer(player_id);
    if (player == nullptr) return false;

    const auto player_ped = player->m_pPed;
    if (player_ped == nullptr) return false;

    const auto player_game_ped = player_ped->m_pGamePed;
    if (player_game_ped == nullptr) return false;

    return player_game_ped->GetIsOnScreen() && CWorld::GetIsLineOfSightClear
        (TheCamera.GetPosition(), player_game_ped->GetPosition(),
         true, false, false, true, true, true, true);
}

bool GameUtil::GetRadarRect(CRect& radar_rect) noexcept
{
    float screen_width  = 0;
    float screen_height = 0;

    if (!Render::GetScreenSize(screen_width, screen_height)) return false;

    auto mult_width   = *reinterpret_cast<float*>(0x859520); // 1.f / 640.f
    auto mult_height  = *reinterpret_cast<float*>(0x859524); // 1.f / 448.f
    auto radar_width  = *reinterpret_cast<float*>(0x866b74); // 76.f
    auto radar_height = *reinterpret_cast<float*>(0x866b78); // 94.f

#if defined(SAMP_R3)

    if (TheCamera.m_bWideScreenOn)
    {
        mult_width   = 0.00222f * screen_height / screen_width;
        mult_height  = 0.00242f;
        radar_width  = 82.f;
        radar_height = 96.f;
    }

#endif

    const float pos_y = (1.f - mult_height * 104.f) * screen_height;

    radar_rect.left   = (40.f - 4.f) * screen_width * mult_width;
    radar_rect.top    = pos_y - mult_height * screen_height * 4.f;
    radar_rect.right  = (40.f + 4.f + radar_height) * screen_width * mult_width;
    radar_rect.bottom = (4.f + radar_width) * mult_height * screen_height + pos_y;

    return true;
}

#define ApplyPatch(base, offset, bytes)                                \
{                                                                      \
    const LPVOID address = (LPVOID)((base) + (offset));                \
    const Memory::UnprotectScope scope { address, sizeof(bytes) - 1 }; \
    std::memcpy(address, bytes, sizeof(bytes) - 1);                    \
}

void GameUtil::DisableAntiCheat(const DWORD base_addr) noexcept
{
#if defined(SAMP_R1)
    ApplyPatch(base_addr, 0x298116, "\xB8\x45\x00\x00\x00\xC2\x1C\x00");
    ApplyPatch(base_addr, 0x286923, "\xB8\x45\x00\x00\x00\xC2\x1C\x00");
    ApplyPatch(base_addr, 0x99250,  "\xC3");
#elif defined(SAMP_R3)
    ApplyPatch(base_addr, 0xC4DC0,  "\xB8\x45\x00\x00\x00\xC2\x1C\x00");
    ApplyPatch(base_addr, 0x9D1A0,  "\xC3");
#else
#error Unknown SAMP version
#endif
}

#undef ApplyPatch

std::string GameUtil::GetSampvoiceDirectory()
{
    static const char kSVDirRelativePath[] = "\\" "GTA San Andreas User Files"
                                             "\\" "sampvoice";

    char buffer[MAX_PATH];

    if (const auto result = SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS | CSIDL_FLAG_CREATE,
        NULL, SHGFP_TYPE_CURRENT, buffer); FAILED(result))
    {
        Logger::LogToFile("[err:gameutil] : failed to get 'MyDocuments' directory (code:%ld)", result);
        return {};
    }

    const size_t my_documents_length = std::strlen(buffer);

    std::memcpy(buffer + my_documents_length, kSVDirRelativePath, sizeof(kSVDirRelativePath));

    if (const auto code = SHCreateDirectoryEx(NULL, buffer, NULL);
        code != ERROR_ALREADY_EXISTS && code != ERROR_FILE_EXISTS && code != ERROR_SUCCESS)
    {
        Logger::LogToFile("[err:gameutil] : failed to create plugin directory (code:%d)", code);
        return {};
    }

    return { buffer, my_documents_length + (sizeof(kSVDirRelativePath) - 1) };
}
