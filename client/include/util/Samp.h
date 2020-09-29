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

    using InitHandlerType = std::function<void()>;
    using ExitHandlerType = std::function<void()>;

public:

    static bool Init(const AddressesBase& addrBase,
                     InitHandlerType&& initHandler,
                     ExitHandlerType&& exitHandler) noexcept;
    static bool IsInited() noexcept;
    static bool IsLoaded() noexcept;
    static void Free() noexcept;

    static void AddClientCommand(const char* cmdName, SAMP::CMDPROC cmdHandler) noexcept;
    static void AddMessageToChat(D3DCOLOR color, const char* message) noexcept;
    static void ToggleSampCursor(int mode) noexcept;

private:

    static void HookFuncSampInit() noexcept;
    static void HookFuncSampFree() noexcept;

private:

    static bool initStatus;
    static bool loadStatus;

    static InitHandlerType initHandler;
    static ExitHandlerType exitHandler;

    static Memory::JumpHookPtr hookSampInit;
    static Memory::JumpHookPtr hookSampFree;

};
