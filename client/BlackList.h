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

    BlackList() = delete;
    ~BlackList() = delete;
    BlackList(const BlackList&) = delete;
    BlackList(BlackList&&) = delete;
    BlackList& operator=(const BlackList&) = delete;
    BlackList& operator=(BlackList&&) = delete;

public:

    struct LockedPlayer {

        LockedPlayer() = delete;
        LockedPlayer(const LockedPlayer&) = delete;
        LockedPlayer(LockedPlayer&&) = delete;
        LockedPlayer& operator=(const LockedPlayer&) = delete;
        LockedPlayer& operator=(LockedPlayer&&) = delete;

    public:

        explicit LockedPlayer(std::string playerName, WORD playerId) noexcept;

        ~LockedPlayer() noexcept = default;

    public:

        const std::string playerName;
        WORD playerId { SV::kNonePlayer };

    };

public:

    static bool Init(const AddressesBase& addrBase) noexcept;
    static void Free() noexcept;

    static bool Load(const std::string& filePath);
    static bool Save(const std::string& filePath);

    static void LockPlayer(WORD playerId);
    static void UnlockPlayer(WORD playerId);
    static void UnlockPlayer(const std::string& playerName);

    static const std::list<LockedPlayer>& RequestBlackList() noexcept;

    static bool IsPlayerBlocked(WORD playerId) noexcept;
    static bool IsPlayerBlocked(const std::string& playerName) noexcept;

private:

    static BOOL __thiscall CreatePlayerInPoolHook(SAMP::CPlayerPool* _this,
        SAMP::ID nId, const char* szName, BOOL bIsNPC) noexcept;

    static BOOL __thiscall DeletePlayerFromPoolHook(SAMP::CPlayerPool* _this,
        SAMP::ID nId, int nReason) noexcept;

private:

    static bool initStatus;

    static std::list<LockedPlayer> blackList;

    static Memory::JumpHookPtr createPlayerInPoolHook;
    static Memory::JumpHookPtr deletePlayerInPoolHook;

};
