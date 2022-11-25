/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <string>

#include <Windows.h>

#include <game/CRect.h>

#include "AddressesBase.h"

struct GameUtil {

    GameUtil() = delete;
    ~GameUtil() = delete;
    GameUtil(const GameUtil&) = delete;
    GameUtil(GameUtil&&) = delete;
    GameUtil& operator=(const GameUtil&) = delete;
    GameUtil& operator=(GameUtil&&) = delete;

public:

    static bool IsKeyPressed(int key_id) noexcept;
    static bool IsMenuActive() noexcept;
    static bool IsWindowActive() noexcept;
    static bool IsGameActive() noexcept;
    static bool HasPlayerPed() noexcept;
    static bool IsPlayerVisible(WORD player_id) noexcept;
    static bool GetRadarRect(CRect& radar_rect) noexcept;
    static void DisableAntiCheat(DWORD base_addr) noexcept;
    static std::string GetSampvoiceDirectory();

};
