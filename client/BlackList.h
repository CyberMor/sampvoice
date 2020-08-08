/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <list>
#include <string>

#include <util/Memory.hpp>
#include <util/AddressesBase.h>
#include <samp/CPlayerPool.h>

#include "Header.h"

class BlackList {
public:

	struct LockedPlayer {

		const std::string playerName;
		WORD playerId = SV::NonePlayer;

		LockedPlayer(
			const std::string& playerName,
			const WORD playerId
		) :
			playerName(playerName),
			playerId(playerId)
		{}

	};

private:

	static bool initStatus;

	static std::list<LockedPlayer> blackList;

	static Memory::JumpHookPtr createPlayerInPoolHook;
	static Memory::JumpHookPtr deletePlayerInPoolHook;

	static BOOL __thiscall CreatePlayerInPoolHook(SAMP::CPlayerPool* _this, SAMP::ID nId, const char* szName, BOOL bIsNPC);
	static BOOL __thiscall DeletePlayerFromPoolHook(SAMP::CPlayerPool* _this, SAMP::ID nId, int nReason);

public:

	static bool Init(const AddressesBase& addrBase);

	static bool Load(const std::string& filePath);
	static bool Save(const std::string& filePath);

	static void LockPlayer(const WORD playerId);
	static void UnlockPlayer(const WORD playerId);
	static void UnlockPlayer(const std::string& playerName);

	static const std::list<LockedPlayer>& RequestBlackList();

	static bool IsPlayerBlocked(const WORD playerId);
	static bool IsPlayerBlocked(const std::string& playerName);

	static void Free();

};
