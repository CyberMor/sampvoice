/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <array>
#include <atomic>
#include <shared_mutex>
#include <cstdint>

#include <ysf/structs.h>

#include "PlayerInfo.h"

class PlayerStore {

    PlayerStore() = delete;
    ~PlayerStore() = delete;
    PlayerStore(const PlayerStore&) = delete;
    PlayerStore(PlayerStore&&) = delete;
    PlayerStore& operator=(const PlayerStore&) = delete;
    PlayerStore& operator=(PlayerStore&&) = delete;

public:

    static void AddPlayerToStore(uint16_t playerId, uint8_t version, bool microStatus);
    static void RemovePlayerFromStore(uint16_t playerId);

    static void ClearStore();

    static bool IsPlayerConnected(uint16_t playerId) noexcept;
    static bool IsPlayerHasPlugin(uint16_t playerId) noexcept;

    static PlayerInfo* RequestPlayerWithSharedAccess(uint16_t playerId) noexcept;
    static void ReleasePlayerWithSharedAccess(uint16_t playerId) noexcept;

    static PlayerInfo* RequestPlayerWithUniqueAccess(uint16_t playerId) noexcept;
    static void ReleasePlayerWithUniqueAccess(uint16_t playerId) noexcept;

private:

    static std::array<std::shared_mutex, MAX_PLAYERS> playerMutex;
    static std::array<std::atomic<PlayerInfo*>, MAX_PLAYERS> playerInfo;

};
