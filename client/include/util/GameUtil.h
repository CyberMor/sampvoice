/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <Windows.h>

#include <game/CRect.h>

#include "AddressesBase.h"

class GameUtil {

    GameUtil() = delete;
    ~GameUtil() = delete;
    GameUtil(const GameUtil&) = delete;
    GameUtil(GameUtil&&) = delete;
    GameUtil& operator=(const GameUtil&) = delete;
    GameUtil& operator=(GameUtil&&) = delete;

public:

    static bool IsKeyPressed(int keyId) noexcept;
    static bool IsMenuActive() noexcept;
    static bool IsWindowActive() noexcept;
    static bool IsGameActive() noexcept;
    static bool HasPlayerPed() noexcept;
    static bool IsPlayerVisible(WORD playerId) noexcept;
    static bool GetRadarRect(CRect& radarRect) noexcept;
    static void DisableAntiCheat(const AddressesBase& addrBase);

};
