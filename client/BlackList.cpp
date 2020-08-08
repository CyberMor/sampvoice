/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "BlackList.h"

#include <fstream>

#include <samp/CNetGame.h>
#include <util/Logger.h>

bool BlackList::initStatus(false);

std::list<BlackList::LockedPlayer> BlackList::blackList;

Memory::JumpHookPtr BlackList::createPlayerInPoolHook(nullptr);
Memory::JumpHookPtr BlackList::deletePlayerInPoolHook(nullptr);

BOOL __thiscall BlackList::CreatePlayerInPoolHook(SAMP::CPlayerPool* _this, SAMP::ID nId, const char* szName, BOOL bIsNPC) {

	BlackList::createPlayerInPoolHook->Disable();

	const auto retStatus = ((BOOL(__thiscall*)(SAMP::CPlayerPool*, SAMP::ID, const char*, BOOL))
		(BlackList::createPlayerInPoolHook->GetPatch().memAddr))(_this, nId, szName, bIsNPC);

	BlackList::createPlayerInPoolHook->Enable();

	if (retStatus && !bIsNPC) {

		for (auto& playerInfo : BlackList::blackList)
			if (playerInfo.playerName == szName) {

				playerInfo.playerId = nId;
				break;

			}

	}

	return retStatus;

}

BOOL __thiscall BlackList::DeletePlayerFromPoolHook(SAMP::CPlayerPool* _this, SAMP::ID nId, int nReason) {

	for (auto& playerInfo : BlackList::blackList)
		if (playerInfo.playerId == nId) {

			playerInfo.playerId = SV::NonePlayer;
			break;

		}

	BlackList::deletePlayerInPoolHook->Disable();

	const BOOL retStatus = ((BOOL(__thiscall*)(SAMP::CPlayerPool*, SAMP::ID, int))
		(BlackList::deletePlayerInPoolHook->GetPatch().memAddr))(_this, nId, nReason);

	BlackList::deletePlayerInPoolHook->Enable();

	return retStatus;

}

bool BlackList::Load(const std::string& filePath) {

	std::ifstream blackListFile(filePath, std::ifstream::in);

	if (!blackListFile || !blackListFile.is_open()) return false;

	BlackList::blackList.clear();

	std::string nickName;

	while (std::getline(blackListFile, nickName) && !nickName.empty()) {

		WORD playerId = SV::NonePlayer;

		const auto pNetGame = SAMP::pNetGame();

		if (pNetGame) {

			const auto pPlayerPool = pNetGame->GetPlayerPool();

			if (pPlayerPool) for (WORD iPlayerId = 0; iPlayerId < MAX_PLAYERS; ++iPlayerId) {

				const char* playerName = pPlayerPool->GetName(iPlayerId);

				if (playerName && nickName == playerName) {

					playerId = iPlayerId;
					break;

				}

			}

		}

		BlackList::blackList.emplace_back(nickName, playerId);

	}

	return true;

}

bool BlackList::Save(const std::string& filePath) {

	std::ofstream blackListFile(filePath, std::ofstream::out | std::ofstream::trunc);

	if (!blackListFile || !blackListFile.is_open()) return false;

	for (const auto& playerInfo : BlackList::blackList)
		blackListFile << playerInfo.playerName << std::endl;

	return true;

}

bool BlackList::Init(const AddressesBase& addrBase) {

	if (BlackList::initStatus) return false;

	Logger::LogToFile("[sv:dbg:blacklist:init] : module initializing...");

	if (!(BlackList::deletePlayerInPoolHook = MakeJumpHook(addrBase.sampDeletePlayerFromPoolFunc, &BlackList::DeletePlayerFromPoolHook))) {

		Logger::LogToFile("[sv:err:blacklist:init] : failed to create 'DeletePlayerFromPool' function hook");

		BlackList::createPlayerInPoolHook.reset();

		return false;

	}

	if (!(BlackList::createPlayerInPoolHook = MakeJumpHook(addrBase.sampCreatePlayerInPoolFunc, &BlackList::CreatePlayerInPoolHook))) {

		Logger::LogToFile("[sv:err:blacklist:init] : failed to create 'CreatePlayerInPool' function hook");

		return false;

	}

	Logger::LogToFile("[sv:dbg:blacklist:init] : module initialized");

	return BlackList::initStatus = true;

}

void BlackList::LockPlayer(const WORD playerId) {

	if (playerId == SV::NonePlayer) return;

	const auto pNetGame = SAMP::pNetGame();
	if (!pNetGame) return;

	const auto pPlayerPool = pNetGame->GetPlayerPool();
	if (!pPlayerPool) return;

	Logger::LogToFile("[sv:dbg:blacklist:lockplayer] : adding player (%hu) to blacklist...", playerId);

	if (const auto playerName = pPlayerPool->GetName(playerId)) {

		for (const auto& playerInfo : BlackList::blackList)
			if (playerInfo.playerName == playerName) return;

		BlackList::blackList.emplace_front(playerName, playerId);

	}

}

void BlackList::UnlockPlayer(const WORD playerId) {

	Logger::LogToFile("[sv:dbg:blacklist:unlockplayer] : removing player (%hu) from blacklist...", playerId);

	BlackList::blackList.remove_if([&playerId] (const BlackList::LockedPlayer& object) { return playerId == object.playerId; });

}

void BlackList::UnlockPlayer(const std::string& playerName) {

	Logger::LogToFile("[sv:dbg:blacklist:unlockplayer] : removing player (%s) from blacklist...", playerName.c_str());

	BlackList::blackList.remove_if([&playerName] (const BlackList::LockedPlayer& object) { return playerName == object.playerName; });

}

const std::list<BlackList::LockedPlayer>& BlackList::RequestBlackList() {

	return BlackList::blackList;

}

bool BlackList::IsPlayerBlocked(const WORD playerId) {

	if (playerId == SV::NonePlayer) return false;

	for (const auto& playerInfo : BlackList::blackList)
		if (playerInfo.playerId == playerId) return true;

	return false;

}

bool BlackList::IsPlayerBlocked(const std::string& playerName) {

	for (const auto& playerInfo : BlackList::blackList)
		if (playerInfo.playerName == playerName) return true;

	return false;

}

void BlackList::Free() {

	if (!BlackList::initStatus) return;

	Logger::LogToFile("[sv:dbg:blacklist:free] : module releasing...");

	BlackList::deletePlayerInPoolHook.reset();
	BlackList::createPlayerInPoolHook.reset();

	BlackList::blackList.clear();

	Logger::LogToFile("[sv:dbg:blacklist:free] : module released");

	BlackList::initStatus = false;

}
