/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <functional>

#include <d3d9.h>

#include <samp/Commands.h>

#include "Memory.hpp"
#include "AddressesBase.h"

class Samp {
public:

	using InitHandlerType = std::function<void()>;
	using ExitHandlerType = std::function<void()>;

private:

	static bool initStatus;
	static bool loadStatus;

	static InitHandlerType initHandler;
	static ExitHandlerType exitHandler;

	static Memory::JumpHookPtr hookSampInit;
	static Memory::JumpHookPtr hookSampFree;

	static void HookFuncSampInit();
	static void HookFuncSampFree();

public:

	static bool Init(
		const AddressesBase& addrBase,
		const InitHandlerType& initHandler,
		const ExitHandlerType& exitHandler
	);

	static bool IsInited();
	static bool IsLoaded();

	static void AddClientCommand(
		const char* cmdName,
		SAMP::CMDPROC cmdHandler
	);

	static void AddMessageToChat(
		const D3DCOLOR color,
		const char* message
	);

	static void ToggleSampCursor(const int mode);

	static void Free();

};
