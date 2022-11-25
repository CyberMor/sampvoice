/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cstdint>
#include <functional>
#include <vector>

#include <d3d9.h>

#include <samp/Commands.h>

#include "Memory.hpp"
#include "AddressesBase.h"

struct Samp {

    Samp() = delete;
    ~Samp() = delete;
    Samp(const Samp&) = delete;
    Samp(Samp&&) = delete;
    Samp& operator=(const Samp&) = delete;
    Samp& operator=(Samp&&) = delete;

private:

    using LoadCallback = std::function<void()>;
    using ExitCallback = std::function<void()>;

public:

    static bool Init(const AddressesBase& addr_base) noexcept;
    static bool IsInited() noexcept;
    static bool IsLoaded() noexcept;
    static void Free() noexcept;

    static void AddClientCommand(const char* name, SAMP::CMDPROC handler) noexcept;
    static void AddMessageToChat(D3DCOLOR color, const char* message) noexcept;
    static void ToggleSampCursor(int mode) noexcept;

public:

    static std::size_t AddLoadCallback(LoadCallback callback) noexcept;
    static std::size_t AddExitCallback(ExitCallback callback) noexcept;

    static void RemoveLoadCallback(std::size_t callback) noexcept;
    static void RemoveExitCallback(std::size_t callback) noexcept;

private:

    static void HookSampInit() noexcept;
    static void HookSampFree() noexcept;

private:

    static bool _init_status;
    static bool _load_status;

    static std::vector<LoadCallback> _load_callbacks;
    static std::vector<ExitCallback> _exit_callbacks;

    static Memory::JumpHook _hook_samp_init;
    static Memory::JumpHook _hook_samp_free;

};
