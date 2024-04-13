/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cassert>
#include <cstring>

#include <system/file.hpp>
#include <memory/structures/list.hpp>
#include <memory/hacks/jump_hook.hpp>
#include <other/utils.hpp>

#include <samp/CPlayerPool.h>
#include <samp/CNetGame.h>
#include <util/addresses.hpp>
#include <util/logger.hpp>

struct BlackList {

    BlackList(const BlackList&) = delete;
    BlackList(BlackList&&) = delete;
    BlackList& operator=(const BlackList&) = delete;
    BlackList& operator=(BlackList&&) = delete;

private:

    BlackList() noexcept = default;
    ~BlackList() noexcept = default;

public:

    static BlackList& Instance() noexcept
    {
        static BlackList instance;
        return instance;
    }

public:

    struct LockedPlayer {

        LockedPlayer() = delete;
        ~LockedPlayer() noexcept = default;
        LockedPlayer(const LockedPlayer&) noexcept = default;
        LockedPlayer(LockedPlayer&&) noexcept = default;
        LockedPlayer& operator=(const LockedPlayer&) noexcept = default;
        LockedPlayer& operator=(LockedPlayer&&) noexcept = default;

    public:

        LockedPlayer(const cstr_t name, const uword_t id = None<uword_t>) noexcept
            : id { id }
        {
            utils::string::copy(name, this->name, sizeof(this->name));
        }

    public:

        uword_t id;
        char    name[32];

    };

public:

    bool Initialize() noexcept
    {
        if (!_create_player_in_pool_hook.Initialize
            (Addresses::Instance().CreatePlayerInPoolFunction(), CreatePlayerInPoolHook) ||
            !_delete_player_from_pool_hook.Initialize
            (Addresses::Instance().DeletePlayerFromPoolFunction(), DeletePlayerFromPoolHook))
        {
            Logger::Instance().LogToFile("[sv:err:blacklist:initialize] "
                "failed to initialize hooks");
            _create_player_in_pool_hook.Deinitialize();
            _delete_player_from_pool_hook.Deinitialize();
            return false;
        }

        return true;
    }

    void Deinitialize() noexcept
    {
        _create_player_in_pool_hook.Deinitialize();
        _delete_player_from_pool_hook.Deinitialize();

        _locked_players.Clear();
    }

public:

    bool Save(const cstr_t path) const noexcept
    {
        assert(path != nullptr);

        File file { path, "wt" };
        if (file.Invalid())
        {
            Logger::Instance().LogToFile("[sv:err:blacklist:save] failed to open file (%s)", path);
            return false;
        }

        ForwardForEach(player, _locked_players)
        {
            if (!file.PutLine(player->name, '\n'))
            {
                Logger::Instance().LogToFile("[sv:err:blacklist:save] failed to write string to file", path);
                return false;
            }
        }

        return true;
    }

    bool Load(const cstr_t path) noexcept
    {
        assert(path != nullptr);

        File file { path, "rt" };
        if (file.Invalid())
        {
            Logger::Instance().LogToFile("[sv:err:blacklist:load] failed to open file (%s)", path);
            return false;
        }

        _locked_players.Clear();

        SAMP::CPlayerPool* pPlayerPool = nullptr;

        if (const auto pNetGame = SAMP::pNetGame(); pNetGame != nullptr)
            pPlayerPool = pNetGame->GetPlayerPool();

        while (true)
        {
            char name[sizeof(LockedPlayer::name)];

            if (const int result = file.GetLine(name, '\n'); result <= 0)
            {
                switch (result)
                {
                    case -1: Logger::Instance().LogToFile("[sv:err:blacklist:load] file descriptor error"); break;
                    case -2: Logger::Instance().LogToFile("[sv:err:blacklist:load] too long player name"); break;
                }

                return result == 0;
            }

            uword_t id = None<uword_t>;

            if (pPlayerPool != nullptr) for (uword_t i = 0; i != MAX_PLAYERS; ++i)
                if (const auto pPlayerName = pPlayerPool->GetName(i); pPlayerName != nullptr)
                    if (std::strcmp(pPlayerName, name) == 0) { id = i; break; }

            const auto pointer = _locked_players.Construct(name, id);
            if (pointer == nullptr)
            {
                Logger::Instance().LogToFile("[sv:err:blacklist:load] failed to allocate memory for player name");
                return false;
            }

            _locked_players.PushBack(pointer);
        }
    }

public:

    bool LockPlayer(const uword_t id) noexcept
    {
        assert(id < MAX_PLAYERS);

        const auto pNetGame = SAMP::pNetGame();
        if (pNetGame == nullptr) return false;

        const auto pPlayerPool = pNetGame->GetPlayerPool();
        if (pPlayerPool == nullptr) return false;

        const auto pPlayerName = pPlayerPool->GetName(id);
        if (pPlayerName == nullptr) return false;

        if (std::strlen(pPlayerName) >= sizeof(LockedPlayer::name))
        {
            Logger::Instance().LogToFile("[sv:err:blacklist:lockplayer] "
                "too long player name '%s' (must be no more than %u)",
                pPlayerName, static_cast<uint_t>(sizeof(LockedPlayer::name) - 1));
            return false;
        }

        ForwardForEach(iterator, _locked_players)
        {
            if (std::strcmp(pPlayerName, iterator->name) == 0)
                return false;
        }

        Logger::Instance().LogToFile("[sv:dbg:blacklist:lockplayer] "
            "adding player (id:%hu;name:%s) to blacklist...", id, pPlayerName);

        const auto pointer = _locked_players.Construct(pPlayerName, id);
        if (pointer == nullptr)
        {
            Logger::Instance().LogToFile("[sv:err:blacklist:lockplayer] "
                "failed to allocate memory for player name");
            return false;
        }

        _locked_players.PushBack(pointer);

        return true;
    }

    bool LockPlayer(const cstr_t name) noexcept
    {
        assert(name != nullptr);

        if (std::strlen(name) >= sizeof(LockedPlayer::name))
        {
            Logger::Instance().LogToFile("[sv:err:blacklist:lockplayer] "
                "too long player name '%s' (must be no more than %u)",
                name, static_cast<uint_t>(sizeof(LockedPlayer::name) - 1));
            return false;
        }

        ForwardForEach(iterator, _locked_players)
        {
            if (std::strcmp(name, iterator->name) == 0)
                return false;
        }

        uword_t id = None<uword_t>;

        if (const auto pNetGame = SAMP::pNetGame(); pNetGame != nullptr)
        {
            if (const auto pPlayerPool = pNetGame->GetPlayerPool(); pPlayerPool != nullptr)
            {
                for (uword_t i = 0; i != MAX_PLAYERS; ++i)
                {
                    if (const auto pPlayerName = pPlayerPool->GetName(i); pPlayerName != nullptr)
                        if (std::strcmp(pPlayerName, name) == 0) { id = i; break; }
                }
            }
        }

        Logger::Instance().LogToFile("[sv:dbg:blacklist:lockplayer] "
            "adding player (id:%hu;name:%s) to blacklist...", id, name);

        const auto pointer = _locked_players.Construct(name, id);
        if (pointer == nullptr)
        {
            Logger::Instance().LogToFile("[sv:err:blacklist:lockplayer] "
                "failed to allocate memory for player name");
            return false;
        }

        _locked_players.PushBack(pointer);

        return true;
    }

public:

    void UnlockPlayer(const uword_t id) noexcept
    {
        assert(id < MAX_PLAYERS);

        Logger::Instance().LogToFile("[sv:dbg:blacklist:unlockplayer] "
            "removing player (%hu) from blacklist...", id);

        ListRemove(iterator, _locked_players, id == iterator->id);
    }

    void UnlockPlayer(const cstr_t name) noexcept
    {
        assert(name != nullptr);

        Logger::Instance().LogToFile("[sv:dbg:blacklist:unlockplayer] "
            "removing player (%s) from blacklist...", name);

        ListRemove(iterator, _locked_players, std::strcmp(name, iterator->name) == 0);
    }

public:

    bool IsPlayerBlocked(const uword_t id) const noexcept
    {
        assert(id < MAX_PLAYERS);

        ForwardForEach(iterator, _locked_players)
            if (iterator->id == id) return true;

        return false;
    }

    bool IsPlayerBlocked(const cstr_t name) const noexcept
    {
        assert(name != nullptr);

        ForwardForEach(iterator, _locked_players)
            if (std::strcmp(iterator->name, name) == 0)
                return true;

        return false;
    }

public:

    const List<LockedPlayer>& LockedPlayers() const noexcept
    {
        return _locked_players;
    }

private:

    static BOOL __thiscall CreatePlayerInPoolHook(SAMP::CPlayerPool* const _this,
        const SAMP::ID nId, const cstr_t szName, const BOOL bIsNPC) noexcept
    {
        Instance()._create_player_in_pool_hook.Disable();
        const BOOL result = static_cast<BOOL(__thiscall*)(SAMP::CPlayerPool*, SAMP::ID, cstr_t, BOOL)>
            (Instance()._create_player_in_pool_hook.Address())(_this, nId, szName, bIsNPC);
        Instance()._create_player_in_pool_hook.Enable();

        ForwardForEach(iterator, Instance()._locked_players)
        {
            if (std::strcmp(iterator->name, szName) == 0)
                { iterator->id = nId; break; }
        }

        return result;
    }

    static BOOL __thiscall DeletePlayerFromPoolHook(SAMP::CPlayerPool* const _this,
        const SAMP::ID nId, const int nReason) noexcept
    {
        ForwardForEach(iterator, Instance()._locked_players)
        {
            if (iterator->id == nId) { iterator->id = None<uword_t>; break; }
        }

        Instance()._delete_player_from_pool_hook.Disable();
        const BOOL result = static_cast<BOOL(__thiscall*)(SAMP::CPlayerPool*, SAMP::ID, int)>
            (Instance()._delete_player_from_pool_hook.Address())(_this, nId, nReason);
        Instance()._delete_player_from_pool_hook.Enable();

        return result;
    }

private:

    JumpHook _create_player_in_pool_hook;
    JumpHook _delete_player_from_pool_hook;

private:

    List<LockedPlayer> _locked_players;

};
