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
public:

	static bool IsKeyDown(const BYTE keyId);

	static bool IsMenuActive();
	static bool IsWindowActive();
	static bool IsGameActive();

	static bool HasPlayerPed();

	static bool IsPlayerVisible(const WORD playerId);

	static bool GetRadarRect(CRect& radarRect);

	static void DisableAntiCheat(const AddressesBase& addrBase);

};
