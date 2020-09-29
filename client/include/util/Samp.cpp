/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Samp.h"

#include <samp/CChat.h>
#include <samp/CInput.h>
#include <samp/CGameSA.h>
#include <samp/CScoreboard.h>

#include "Logger.h"

bool Samp::Init(const AddressesBase& addrBase,
                InitHandlerType&& initHandler,
                ExitHandlerType&& exitHandler) noexcept
{
    if (Samp::initStatus) return false;

    Logger::LogToFile("[dbg:samp:init] : module initializing...");

    if (!(Samp::hookSampFree = MakeJumpHook(addrBase.GetSampDestructAddr(), Samp::HookFuncSampFree)))
    {
        Logger::LogToFile("[err:samp:init] : failed to create 'SampFree' function hook");
        return false;
    }

    if (!(Samp::hookSampInit = MakeJumpHook(addrBase.GetSampInitAddr(), Samp::HookFuncSampInit)))
    {
        Logger::LogToFile("[err:samp:init] : failed to create 'SampInit' function hook");
        Samp::hookSampFree.reset();
        return false;
    }

    SAMP::InitSamp(addrBase.GetBaseAddr());

    Samp::initHandler = std::move(initHandler);
    Samp::exitHandler = std::move(exitHandler);

    Samp::loadStatus = false;
    Samp::initStatus = true;

    Logger::LogToFile("[dbg:samp:init] : module initialized");

    return true;
}

bool Samp::IsInited() noexcept
{
    return Samp::initStatus;
}

bool Samp::IsLoaded() noexcept
{
    return Samp::loadStatus;
}

void Samp::Free() noexcept
{
    if (!Samp::initStatus) return;

    Logger::LogToFile("[dbg:samp:free] : module releasing...");

    Samp::hookSampInit.reset();
    Samp::hookSampFree.reset();

    if (Samp::loadStatus && Samp::exitHandler)
        Samp::exitHandler();

    Samp::loadStatus = false;

    Samp::initHandler = nullptr;
    Samp::exitHandler = nullptr;

    Logger::LogToFile("[dbg:samp:free] : module released");

    Samp::initStatus = false;
}

void Samp::AddClientCommand(const char* cmdName, SAMP::CMDPROC cmdHandler) noexcept
{
    if (!cmdName || !*cmdName || !cmdHandler) return;

    if (!Samp::loadStatus) return;

    const auto pInputBox = SAMP::pInputBox();
    if (!pInputBox || pInputBox->m_nCommandCount >= MAX_CLIENT_CMDS - 1 ||
        std::strlen(cmdName) > 30) return;

    Logger::LogToFile("[dbg:samp:addclientcommand] : command '%s' adding...", cmdName);

    pInputBox->AddCommand(cmdName, cmdHandler);
}

void Samp::AddMessageToChat(const D3DCOLOR color, const char* message) noexcept
{
    if (!message || !*message) return;

    if (!Samp::loadStatus) return;

    const auto pChat = SAMP::pChat();
    if (!pChat) return;

    pChat->AddEntry(SAMP::ChatEntry::CHAT_TYPE_DEBUG,
                    message, nullptr, color, NULL);
}

void Samp::ToggleSampCursor(const int mode) noexcept
{
    if (!Samp::loadStatus) return;

    const auto pGame = SAMP::pGame();
    const auto pInputBox = SAMP::pInputBox();
    const auto pScoreboard = SAMP::pScoreboard();
    if (!pGame || !pInputBox || !pScoreboard) return;
    if (pGame->IsMenuVisible() || pInputBox->m_bEnabled ||
        pScoreboard->m_bIsEnabled) return;

    pGame->SetCursorMode(mode, !mode);
    if (!mode) pGame->ProcessInputEnabling();
}

void __declspec(naked) Samp::HookFuncSampInit() noexcept
{
    static void* retAddr { nullptr };

    __asm {
        pushad
        mov ebp, esp
        sub esp, __LOCAL_SIZE
    }

    Logger::LogToFile("[dbg:samp:hookinit] : module loading...");

    retAddr = Samp::hookSampInit->GetPatch().memAddr;
    Samp::hookSampInit.reset();

    Samp::loadStatus = true;

    Logger::LogToFile("[dbg:samp:hookinit] : module loaded");

    if (Samp::initHandler) Samp::initHandler();

    __asm {
        mov esp, ebp
        popad
        jmp retAddr
    }
}

void __declspec(naked) Samp::HookFuncSampFree() noexcept
{
    static void* retAddr { nullptr };

    __asm {
        pushad
        mov ebp, esp
        sub esp, __LOCAL_SIZE
    }

    retAddr = Samp::hookSampFree->GetPatch().memAddr;
    Samp::hookSampFree.reset();

    Samp::Free();

    __asm {
        mov esp, ebp
        popad
        jmp retAddr
    }
}

bool Samp::initStatus { false };
bool Samp::loadStatus { false };

Samp::InitHandlerType Samp::initHandler { nullptr };
Samp::ExitHandlerType Samp::exitHandler { nullptr };

Memory::JumpHookPtr Samp::hookSampInit { nullptr };
Memory::JumpHookPtr Samp::hookSampFree { nullptr };
