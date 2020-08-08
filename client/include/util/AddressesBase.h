/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <memory>

#include <Windows.h>

struct AddressesBase {

	const DWORD baseAddr = NULL;

	const DWORD rcInitAddr = NULL;

	const DWORD bassInitCallAddr = NULL;
	const DWORD bassSetConfigAddr = NULL;

	const DWORD sampInitAddr = NULL;
	const DWORD sampDestructAddr = NULL;
	const DWORD sampOpenChatFunc = NULL;
	const DWORD sampSwitchModeFunc = NULL;
	const DWORD sampOpenScoreboardFunc = NULL;
	const DWORD sampCreatePlayerInPoolFunc = NULL;
	const DWORD sampDeletePlayerFromPoolFunc = NULL;
	const DWORD sampSpawnLocalPlayerFunc = NULL;
	const DWORD sampDrawLabelFunc = NULL;

	AddressesBase() = delete;

	AddressesBase& operator=(const AddressesBase& object) = delete;
	AddressesBase& operator=(AddressesBase&& object) = delete;

	AddressesBase(const DWORD baseAddr);

};

typedef std::shared_ptr<AddressesBase> AddressesBasePtr;
#define MakeAddressesBase std::make_shared<AddressesBase>
