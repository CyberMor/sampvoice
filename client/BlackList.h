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

struct BlackList {

    BlackList() = delete;
    ~BlackList() = delete;
    BlackList(const BlackList&) = delete;
    BlackList(BlackList&&) = delete;
    BlackList& operator=(const BlackList&) = delete;
    BlackList& operator=(BlackList&&) = delete;

public:

    struct LockedPlayer {

        LockedPlayer() = delete;
        ~LockedPlayer() noexcept = default;
        LockedPlayer(const LockedPlayer&) = default;
        LockedPlayer(LockedPlayer&&) noexcept = default;
        LockedPlayer& operator=(const LockedPlayer&) = default;
        LockedPlayer& operator=(LockedPlayer&&) noexcept = default;

    public:

        LockedPlayer(std::string player_name, WORD player_id) noexcept;

    public:

        std::string player_name;
        WORD        player_id = SV::kNonePlayer;

    };

public:

    static bool Init(const AddressesBase& addr_base) noexcept;
    static void Free() noexcept;

    static bool Load(const std::string& path);
    static bool Save(const std::string& path);

    static void LockPlayer(WORD player_id);
    static void UnlockPlayer(WORD player_id);
    static void UnlockPlayer(const std::string& player_name);

    static const std::list<LockedPlayer>& RequestBlackList() noexcept;

    static bool IsPlayerBlocked(WORD player_id) noexcept;
    static bool IsPlayerBlocked(const std::string& player_name) noexcept;

private:

    static BOOL __thiscall CreatePlayerInPoolHook(SAMP::CPlayerPool* _this,
        SAMP::ID nId, const char* szName, BOOL bIsNPC) noexcept;

    static BOOL __thiscall DeletePlayerFromPoolHook(SAMP::CPlayerPool* _this,
        SAMP::ID nId, int nReason) noexcept;

private:

    static bool _init_status;

    static std::list<LockedPlayer> _black_list;

    static Memory::JumpHook _create_player_in_pool_hook;
    static Memory::JumpHook _delete_player_in_pool_hook;

};
