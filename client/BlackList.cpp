/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "BlackList.h"

#include <fstream>

#include <samp/CNetGame.h>
#include <util/Logger.h>

BlackList::LockedPlayer::LockedPlayer(std::string player_name, const WORD player_id) noexcept
    : player_name { std::move(player_name) }
    , player_id   { player_id }
{}

bool BlackList::Load(const std::string& path)
{
    std::fstream file { path, std::ios::in };
    if (!file || !file.is_open()) return false;

    _black_list.clear();

    {
        std::string nickname;

        while (std::getline(file, nickname) && !nickname.empty())
        {
            WORD player_id = SV::kNonePlayer;

            if (const auto net_game = SAMP::pNetGame(); net_game != nullptr)
            {
                if (const auto player_pool = net_game->GetPlayerPool(); player_pool != nullptr)
                {
                    for (WORD i = 0; i < MAX_PLAYERS; ++i)
                    {
                        if (const auto player_name = player_pool->GetName(i); player_name != nullptr)
                        {
                            if (nickname == player_name)
                            {
                                player_id = i;
                                break;
                            }
                        }
                    }
                }
            }

            _black_list.emplace_back(std::move(nickname), player_id);
        }
    }

    return true;
}

bool BlackList::Save(const std::string& path)
{
    std::fstream file { path, std::ios::out | std::ios::trunc };
    if (!file || !file.is_open()) return false;

    for (const auto& player_info : _black_list)
    {
        file << player_info.player_name << std::endl;
    }

    return true;
}

bool BlackList::Init(const AddressesBase& addr_base) noexcept
{
    if (_init_status) return false;

    Logger::LogToFile("[sv:dbg:blacklist:init] : module initializing...");

    _delete_player_in_pool_hook = { (LPVOID)(addr_base.GetDeletePlayerFromPoolFunc()), &DeletePlayerFromPoolHook };
    _create_player_in_pool_hook = { (LPVOID)(addr_base.GetCreatePlayerInPoolFunc()), &CreatePlayerInPoolHook };

    Logger::LogToFile("[sv:dbg:blacklist:init] : module initialized");

    _init_status = true;

    return true;
}

void BlackList::Free() noexcept
{
    if (_init_status)
    {
        Logger::LogToFile("[sv:dbg:blacklist:free] : module releasing...");

        _create_player_in_pool_hook = {};
        _delete_player_in_pool_hook = {};

        _black_list.clear();

        Logger::LogToFile("[sv:dbg:blacklist:free] : module released");

        _init_status = false;
    }
}

void BlackList::LockPlayer(const WORD player_id)
{
    if (player_id != SV::kNonePlayer)
    {
        if (const auto net_game = SAMP::pNetGame(); net_game != nullptr)
        {
            if (const auto player_pool = net_game->GetPlayerPool(); player_pool != nullptr)
            {
                if (const auto player_name = player_pool->GetName(player_id); player_name != nullptr)
                {
                    for (const auto& player_info : _black_list)
                    {
                        if (player_info.player_name == player_name) return;
                    }

                    Logger::LogToFile("[sv:dbg:blacklist:lockplayer] : adding player "
                        "(id:%hu;nick:%s) to blacklist...", player_id, player_name);

                    _black_list.emplace_front(player_name, player_id);
                }
            }
        }
    }
}

void BlackList::UnlockPlayer(const WORD player_id)
{
    Logger::LogToFile("[sv:dbg:blacklist:unlockplayer] : removing "
        "player (%hu) from blacklist...", player_id);

    _black_list.remove_if([&player_id](const auto& object)
    {
        return player_id == object.player_id;
    });
}

void BlackList::UnlockPlayer(const std::string& player_name)
{
    Logger::LogToFile("[sv:dbg:blacklist:unlockplayer] : removing "
        "player (%s) from blacklist...", player_name.c_str());

    _black_list.remove_if([&player_name](const auto& object)
    {
        return player_name == object.player_name;
    });
}

const std::list<BlackList::LockedPlayer>& BlackList::RequestBlackList() noexcept
{
    return _black_list;
}

bool BlackList::IsPlayerBlocked(const WORD player_id) noexcept
{
    if (player_id != SV::kNonePlayer)
    {
        for (const auto& player_info : _black_list)
        {
            if (player_info.player_id == player_id)
            {
                return true;
            }
        }
    }

    return false;
}

bool BlackList::IsPlayerBlocked(const std::string& player_name) noexcept
{
    for (const auto& player_info : _black_list)
    {
        if (player_info.player_name == player_name)
        {
            return true;
        }
    }

    return false;
}

BOOL __thiscall BlackList::CreatePlayerInPoolHook(SAMP::CPlayerPool* const _this,
    const SAMP::ID nId, const char* const szName, const BOOL bIsNPC) noexcept
{
    _create_player_in_pool_hook->Disable();
    const auto result = static_cast<BOOL(__thiscall*)(SAMP::CPlayerPool*, SAMP::ID, const char*, BOOL)>
        (_create_player_in_pool_hook->GetPatch().GetAddr())(_this, nId, szName, bIsNPC);
    _create_player_in_pool_hook->Enable();

    if (result == TRUE && bIsNPC == FALSE)
    {
        for (auto& player_info : _black_list)
        {
            if (player_info.player_name == szName)
            {
                player_info.player_id = nId;
                break;
            }
        }
    }

    return result;
}

BOOL __thiscall BlackList::DeletePlayerFromPoolHook(SAMP::CPlayerPool* const _this,
    const SAMP::ID nId, const int nReason) noexcept
{
    for (auto& player_info : _black_list)
    {
        if (player_info.player_id == nId)
        {
            player_info.player_id = SV::kNonePlayer;
            break;
        }
    }

    _delete_player_in_pool_hook->Disable();
    const auto result = static_cast<BOOL(__thiscall*)(SAMP::CPlayerPool*, SAMP::ID, int)>
        (_delete_player_in_pool_hook->GetPatch().GetAddr())(_this, nId, nReason);
    _delete_player_in_pool_hook->Enable();

    return result;
}

bool BlackList::_init_status = false;

std::list<BlackList::LockedPlayer> BlackList::_black_list;

Memory::JumpHook BlackList::_create_player_in_pool_hook;
Memory::JumpHook BlackList::_delete_player_in_pool_hook;
