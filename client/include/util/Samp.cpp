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

bool Samp::Init(const AddressesBase& addrBase) noexcept
{
    if (Samp::initStatus)
        return false;

    Logger::LogToFile("[dbg:samp:init] : module initializing...");

    try
    {
        Samp::hookSampFree = MakeJumpHook(addrBase.GetSampDestructAddr(), Samp::HookSampFree);
        Samp::hookSampInit = MakeJumpHook(addrBase.GetSampInitAddr(), Samp::HookSampInit);
    }
    catch (const std::exception& exception)
    {
        Logger::LogToFile("[err:samp:init] : failed to create function hooks");
        Samp::hookSampInit.reset();
        Samp::hookSampFree.reset();
        return false;
    }

    SAMP::InitSamp(addrBase.GetBaseAddr());

    Samp::loadCallbacks.clear();
    Samp::exitCallbacks.clear();

    Samp::loadStatus = false;

    Logger::LogToFile("[dbg:samp:init] : module initialized");

    Samp::initStatus = true;

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
    if (!Samp::initStatus)
        return;

    Logger::LogToFile("[dbg:samp:free] : module releasing...");

    Samp::hookSampInit.reset();
    Samp::hookSampFree.reset();

    if (Samp::loadStatus)
    {
        for (const auto& exitCallback : Samp::exitCallbacks)
        {
            if (exitCallback != nullptr)
                exitCallback();
        }
    }

    Samp::loadStatus = false;

    Samp::loadCallbacks.clear();
    Samp::exitCallbacks.clear();

    Logger::LogToFile("[dbg:samp:free] : module released");

    Samp::initStatus = false;
}

void Samp::AddClientCommand(const char* const cmdName, const SAMP::CMDPROC cmdHandler) noexcept
{
    constexpr int kMaxCommands = MAX_CLIENT_CMDS - 1;
    constexpr size_t kMaxCommandLength = 30;

    if (cmdName == nullptr || *cmdName == '\0' || cmdHandler == nullptr)
        return;

    if (!Samp::loadStatus)
        return;

    if (const auto pInputBox = SAMP::pInputBox();
        pInputBox != nullptr && pInputBox->m_nCommandCount < kMaxCommands &&
        std::strlen(cmdName) <= kMaxCommandLength)
    {
        Logger::LogToFile("[dbg:samp:addclientcommand] : command '%s' adding...", cmdName);
        pInputBox->AddCommand(cmdName, cmdHandler);
    }
}

void Samp::AddMessageToChat(const D3DCOLOR color, const char* const message) noexcept
{
    if (message == nullptr || *message == '\0')
        return;

    if (!Samp::loadStatus)
        return;

    if (const auto pChat = SAMP::pChat(); pChat != nullptr)
    {
        pChat->AddEntry(SAMP::ChatEntry::CHAT_TYPE_DEBUG,
                        message, nullptr, color, NULL);
    }
}

void Samp::ToggleSampCursor(const int mode) noexcept
{
    if (!Samp::loadStatus)
        return;

    if (const auto pInputBox = SAMP::pInputBox();
        pInputBox == nullptr || pInputBox->m_bEnabled == TRUE)
        return;

    if (const auto pScoreboard = SAMP::pScoreboard();
        pScoreboard == nullptr || pScoreboard->m_bIsEnabled == TRUE)
        return;

    if (const auto pGame = SAMP::pGame();
        pGame != nullptr && pGame->IsMenuVisible() == FALSE)
    {
        pGame->SetCursorMode(mode, mode == 0 ? TRUE : FALSE);
        if (mode == 0) pGame->ProcessInputEnabling();
    }
}

std::size_t Samp::AddLoadCallback(LoadCallback callback) noexcept
{
    if (!Samp::initStatus) return -1;

    for (std::size_t i { 0 }; i < Samp::loadCallbacks.size(); ++i)
    {
        if (Samp::loadCallbacks[i] == nullptr)
        {
            Samp::loadCallbacks[i] = std::move(callback);
            return i;
        }
    }

    Samp::loadCallbacks.emplace_back(std::move(callback));
    return Samp::loadCallbacks.size() - 1;
}

std::size_t Samp::AddExitCallback(ExitCallback callback) noexcept
{
    if (!Samp::initStatus) return -1;

    for (std::size_t i { 0 }; i < Samp::exitCallbacks.size(); ++i)
    {
        if (Samp::exitCallbacks[i] == nullptr)
        {
            Samp::exitCallbacks[i] = std::move(callback);
            return i;
        }
    }

    Samp::exitCallbacks.emplace_back(std::move(callback));
    return Samp::exitCallbacks.size() - 1;
}

void Samp::RemoveLoadCallback(const std::size_t callback) noexcept
{
    if (!Samp::initStatus) return;

    if (callback >= Samp::loadCallbacks.size())
        return;

    Samp::loadCallbacks[callback] = nullptr;
}

void Samp::RemoveExitCallback(const std::size_t callback) noexcept
{
    if (!Samp::initStatus) return;

    if (callback >= Samp::exitCallbacks.size())
        return;

    Samp::exitCallbacks[callback] = nullptr;
}

void __declspec(naked) Samp::HookSampInit() noexcept
{
    static LPVOID retAddr { nullptr };

    __asm {
        pushad
        mov ebp, esp
        sub esp, __LOCAL_SIZE
    }

    Logger::LogToFile("[dbg:samp:load] : module loading...");

    retAddr = Samp::hookSampInit->GetPatch().memAddr;
    Samp::hookSampInit.reset();

    for (const auto& loadCallback : Samp::loadCallbacks)
    {
        if (loadCallback != nullptr)
            loadCallback();
    }

    Samp::loadStatus = true;

    Logger::LogToFile("[dbg:samp:load] : module loaded");

    __asm {
        mov esp, ebp
        popad
        jmp retAddr
    }
}

void __declspec(naked) Samp::HookSampFree() noexcept
{
    static LPVOID retAddr { nullptr };

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

std::vector<Samp::LoadCallback> Samp::loadCallbacks;
std::vector<Samp::ExitCallback> Samp::exitCallbacks;

Memory::JumpHookPtr Samp::hookSampInit { nullptr };
Memory::JumpHookPtr Samp::hookSampFree { nullptr };
