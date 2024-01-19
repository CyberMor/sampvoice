/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <functional>

#include <d3d9.h>

#include <memory/hacks/jump_hook.hpp>

#include <samp/CChat.h>
#include <samp/CInput.h>
#include <samp/CGameSA.h>
#include <samp/CScoreboard.h>
#include <samp/Commands.h>

#include "addresses.hpp"
#include "logger.hpp"

struct Samp {

    Samp(const Samp&) = delete;
    Samp(Samp&&) = delete;
    Samp& operator=(const Samp&) = delete;
    Samp& operator=(Samp&&) = delete;

private:

    Samp() noexcept = default;
    ~Samp() noexcept = default;

public:

    static Samp& Instance() noexcept
    {
        static Samp instance;
        return instance;
    }

public:

    bool Initialize() noexcept
    {
        Logger::Instance().LogToFile("[dbg:samp:initialize] module initializing...");

        if (!_samp_destruction_hook.Initialize(Addresses::Instance().SampDestruction(), SampDestructionHook) ||
            !_samp_initialization_hook.Initialize(Addresses::Instance().SampInitialization(), SampInitializationHook))
        {
            Logger::Instance().LogToFile("[err:samp:initialize] failed to initialize hooks");
            _samp_initialization_hook.Deinitialize();
            _samp_destruction_hook.Deinitialize();
            return false;
        }

        SAMP::InitSamp(reinterpret_cast<DWORD>(Addresses::Instance().Base()));

        Logger::Instance().LogToFile("[dbg:samp:initialize] module initialized");

        return true;
    }

    void Deinitialize() noexcept
    {
        Logger::Instance().LogToFile("[dbg:samp:deinitialize] module releasing...");

        _samp_initialization_hook.Deinitialize();
        _samp_destruction_hook.Deinitialize();

        if (_is_loaded)
        {
            if (OnExit != nullptr)
                OnExit();

            _is_loaded = false;
        }

        Logger::Instance().LogToFile("[dbg:samp:deinitialize] module released");
    }

public:

    bool IsLoaded() const noexcept
    {
        return _is_loaded;
    }

public:

    void AddClientCommand(const cstr_t name, const SAMP::CMDPROC handler) noexcept
    {
        if (name != nullptr && *name != '\0' && handler != nullptr)
        {
            if (_is_loaded)
            {
                constexpr int    kMaxCommands      = MAX_CLIENT_CMDS - 1;
                constexpr size_t kMaxCommandLength = 30;

                if (const auto pInputBox = SAMP::pInputBox(); pInputBox != nullptr &&
                    pInputBox->m_nCommandCount < kMaxCommands && std::strlen(name) <= kMaxCommandLength)
                {
                    Logger::Instance().LogToFile("[dbg:samp:addclientcommand] command '%s' adding...", name);
                    pInputBox->AddCommand(name, handler);
                }
            }
        }
    }

    void AddMessageToChat(const D3DCOLOR color, const cstr_t message) noexcept
    {
        if (message != nullptr && *message != '\0')
        {
            if (_is_loaded)
            {
                if (const auto pChat = SAMP::pChat(); pChat != nullptr)
                {
                    pChat->AddEntry(SAMP::ChatEntry::CHAT_TYPE_CHAT, message, nullptr, color, NULL);
                }
            }
        }
    }

    void ToggleSampCursor(const int mode) noexcept
    {
        if (_is_loaded)
        {
            if (const auto pInputBox = SAMP::pInputBox(); pInputBox != nullptr &&
                pInputBox->m_bEnabled == FALSE)
            {
                if (const auto pScoreboard = SAMP::pScoreboard(); pScoreboard != nullptr &&
                    pScoreboard->m_bIsEnabled == FALSE)
                {
                    if (const auto pGame = SAMP::pGame(); pGame != nullptr &&
                        pGame->IsMenuVisible() == FALSE)
                    {
                        pGame->SetCursorMode(mode, mode == 0 ? TRUE : FALSE);
                        if (mode == 0) pGame->ProcessInputEnabling();
                    }
                }
            }
        }
    }

public:

    bool IsChatActive() const noexcept
    {
        if (_is_loaded)
        {
            if (const auto pInputBox = SAMP::pInputBox(); pInputBox != nullptr)
                return pInputBox->m_bEnabled != FALSE;
        }

        return false;
    }

private:

    static void SampInitializationHook() noexcept;
    static void SampDestructionHook() noexcept;

private:

    volatile bool _is_loaded = false;

public:

    std::function<void()> OnLoad;
    std::function<void()> OnExit;

private:

    JumpHook _samp_initialization_hook;
    JumpHook _samp_destruction_hook;

};

void __declspec(naked) Samp::SampInitializationHook() noexcept
{
    static LPVOID return_address;

    __asm
    {
        pushad
        mov ebp, esp
        sub esp, __LOCAL_SIZE
    }

    Logger::Instance().LogToFile("[dbg:samp:load] module loading...");

    return_address = Instance()._samp_initialization_hook.Address();
    Instance()._samp_initialization_hook.Deinitialize();

    if (Instance().OnLoad != nullptr)
        Instance().OnLoad();

    Instance()._is_loaded = true;

    Logger::Instance().LogToFile("[dbg:samp:load] module loaded");

    __asm
    {
        mov esp, ebp
        popad
        jmp return_address
    }
}

void __declspec(naked) Samp::SampDestructionHook() noexcept
{
    static LPVOID return_address;

    __asm
    {
        pushad
        mov ebp, esp
        sub esp, __LOCAL_SIZE
    }

    return_address = Instance()._samp_destruction_hook.Address();
    Instance().Deinitialize();

    __asm
    {
        mov esp, ebp
        popad
        jmp return_address
    }
}
