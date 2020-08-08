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
private:

	static std::array<std::shared_mutex, MAX_PLAYERS> playerMutex;
	static std::array<std::atomic<PlayerInfo*>, MAX_PLAYERS> playerInfo;

public:

	static void AddPlayerToStore(const uint16_t playerId, const uint8_t version, const bool microStatus);
	static void RemovePlayerFromStore(const uint16_t playerId);

	static void ClearStore();

	static bool IsPlayerConnected(const uint16_t playerId);
	static bool IsPlayerHasPlugin(const uint16_t playerId);

	static PlayerInfo* RequestPlayerWithSharedAccess(const uint16_t playerId);
	static void ReleasePlayerWithSharedAccess(const uint16_t playerId);

	static PlayerInfo* RequestPlayerWithUniqueAccess(const uint16_t playerId);
	static void ReleasePlayerWithUniqueAccess(const uint16_t playerId);

};
