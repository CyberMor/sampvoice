/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <functional>

#include <system/types.hpp>
#include <memory/types.hpp>
#include <memory/hacks/unprotect_scope.hpp>
#include <memory/hacks/jump_hook.hpp>
#include <memory/hacks/scanner.hpp>
#include <network/header.hpp>

#include <raknet/bitstream.h>
#include <raknet/networktypes.h>
#include <ysf/structs.h>

#include "logger.hpp"

namespace
{
    constexpr udword_t kConnectSignature = 0xDEADC0DE;
    constexpr  ubyte_t kConnectRpcId     = 25;
}

namespace
{
    constexpr const char kGetRakServerInterfacePattern[] =
#ifdef _WIN32
        "\x64\xA1\x00\x00\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x51"
        "\x68\x18\x0E\x00\x00\xE8\xFF\xFF\xFF\xFF\x83\xC4\x04\x89\x04"
        "\x24\x85\xC0\xC7\x44\x24\xFF\x00\x00\x00\x00\x74\x16"
#else
        "\x04\x24\xFF\xFF\xFF\xFF\x89\x75\xFF\x89\x5D\xFF\xE8\xFF\xFF"
        "\xFF\xFF\x89\x04\x24\x89\xC6\xE8\xFF\xFF\xFF\xFF\x89\xF0\x8B"
        "\x5D\xFF\x8B\x75\xFF\x89\xEC\x5D\xC3"
#endif
        ;

    constexpr const char kGetRakServerInterfaceMask[] =
#ifdef _WIN32
        "xxxxxxxxxxxxxxxx????x????xxxxxxxxxxx?xxxxxx"
#else
        "xx????xx?xx?x????xxxxxx????xxxx?xx?xxxx"
#endif
        ;

    constexpr auto kOnPlayerDisconnectAddress =
#ifdef _WIN32
        0x046D970
#else
        0x80A51D0
#endif
        ;

    struct FunctionOffset
    {
        enum
        {
#ifdef _WIN32
            Start                = 1,
            Send                 = 7,
            Receive              = 10,
            Kick                 = 11,
            DeallocatePacket     = 12,
            SetAllowedPlayers    = 13,
            GetLastPing          = 19,
            RegisterRpc          = 29,
            UnregisterRpc        = 31,
            Rpc                  = 32,
            GetLocalIp           = 52,
            GetInternalId        = 53,
            GetIndexFromPlayerId = 57,
            GetPlayerIdFromIndex = 58,
            AddBan               = 60,
            RemoveBan            = 61,
            ClearBan             = 62,
            SetTimeout           = 65
#else
            Start                = 2,
            Send                 = 9,
            Receive              = 11,
            Kick                 = 12,
            DeallocatePacket     = 13,
            SetAllowedPlayers    = 14,
            GetLastPing          = 20,
            RegisterRpc          = 30,
            UnregisterRpc        = 32,
            Rpc                  = 35,
            GetLocalIp           = 53,
            GetInternalId        = 54,
            GetIndexFromPlayerId = 58,
            GetPlayerIdFromIndex = 59,
            AddBan               = 61,
            RemoveBan            = 62,
            ClearBan             = 63,
            SetTimeout           = 65
#endif
        };
    };
}

struct RakNet {

    RakNet(const RakNet&) = delete;
    RakNet(RakNet&&) = delete;
    RakNet& operator=(const RakNet&) = delete;
    RakNet& operator=(RakNet&&) = delete;

private:

    RakNet() noexcept = default;
    ~RakNet() noexcept = default;

public:

    static RakNet& Instance() noexcept
    {
        static RakNet instance;
        return instance;
    }

public:

    bool Initialize(const cptr_t base) noexcept
    {
        Logger::Instance().Log("[dbg:raknet:initialize] : module initializing...");

        const auto [address, length] = GetModuleInfo(base);
        if (address == nullptr)
        {
            Logger::Instance().Log("[err:raknet:initialize] : failed to get module info");
            return false;
        }

        const ptr_t pointer = Scanner(address, length).Find(kGetRakServerInterfacePattern, kGetRakServerInterfaceMask);
        if (pointer == nullptr)
        {
            Logger::Instance().Log("[err:raknet:initialize] : failed to find 'GetRakServerInterface' function address");
            return false;
        }

        if (!_hook_get_rak_server_interface.Initialize(static_cast<adr_t>(pointer) - 7, GetRakServerInterfaceHook) ||
            !_hook_disconnect.Initialize(reinterpret_cast<ptr_t>(kOnPlayerDisconnectAddress), DisconnectHook))
        {
            Logger::Instance().Log("[err:raknet:initialize] : failed to initialize hooks");
            _hook_get_rak_server_interface.Deinitialize();
            _hook_disconnect.Deinitialize();
            return false;
        }

        Logger::Instance().Log("[dbg:raknet:initialize] : module initialized");

        return true;
    }

    void Deinitialize() noexcept
    {
        Logger::Instance().Log("[dbg:raknet:deinitialize] : module releasing...");

        if (_rak_server_interface != nullptr)
        {
            if (_orig_connect_handler != nullptr)
            {
                const auto rpc_connect_id = kConnectRpcId;
                UnregisterRpc(&rpc_connect_id);
                RegisterRpc(&rpc_connect_id, _orig_connect_handler);
                _orig_connect_handler = nullptr;
            }

            const auto vtable = *static_cast<ptr_t**>(_rak_server_interface);

            {
                const UnprotectScope<sizeof(*vtable)> scope { vtable + FunctionOffset::RegisterRpc };
                vtable[FunctionOffset::RegisterRpc] = reinterpret_cast<ptr_t>(_func_rpc);
            } {
                const UnprotectScope<sizeof(*vtable)> scope { vtable + FunctionOffset::Receive };
                vtable[FunctionOffset::Receive] = reinterpret_cast<ptr_t>(_func_receive);
            } {
                _func_start                    = nullptr;
                _func_send                     = nullptr;
                _func_receive                  = nullptr;
                _func_kick                     = nullptr;
                _func_deallocate_packet        = nullptr;
                _func_set_allowed_players      = nullptr;
                _func_get_last_ping            = nullptr;
                _func_register_as_rpc          = nullptr;
                _func_unregister_as_rpc        = nullptr;
                _func_rpc                      = nullptr;
                _func_get_local_ip             = nullptr;
                _func_get_index_from_player_id = nullptr;
                _func_get_player_id_from_index = nullptr;
                _func_add_to_ban_list          = nullptr;
                _func_remove_from_ban_list     = nullptr;
                _func_clear_ban_list           = nullptr;
                _func_set_timeout_time         = nullptr;
            }

            _rak_server_interface = nullptr;
        }

        std::fill(std::begin(_player_status), std::end(_player_status), false);

        _hook_get_rak_server_interface.Deinitialize();
        _hook_disconnect.Deinitialize();

        Logger::Instance().Log("[dbg:raknet:deinitialize] : module released");
    }

public:

    bool IsLoaded() const noexcept
    {
        return _rak_server_interface != nullptr;
    }

public:

    bool SendRpc(const ubyte_t rpc, const uword_t player, const adr_t data, const uint_t size) noexcept
    {
        return Rpc(&rpc, &BitStream(data, size, false),
            PacketPriority::MEDIUM_PRIORITY, PacketReliability::RELIABLE_ORDERED, 0,
            GetPlayerIdFromIndex(player), player == None<uword_t>, false);
    }

    bool SendPacket(const uword_t player, const adr_t packet, const uint_t length) noexcept
    {
        return Send(&BitStream(packet, length, false),
            PacketPriority::MEDIUM_PRIORITY, PacketReliability::RELIABLE_ORDERED, 0,
            GetPlayerIdFromIndex(player), player == None<uword_t>);
    }

    void KickPlayer(const uword_t player) noexcept
    {
        Kick(GetPlayerIdFromIndex(player));
    }

private:

    static ptr_t GetRakServerInterfaceHook() noexcept
    {
        Instance()._hook_get_rak_server_interface.Disable();
        const auto rak_server_interface = reinterpret_cast<ptr_t(*)()>(Instance()._hook_get_rak_server_interface.Address())();
        Instance()._hook_get_rak_server_interface.Enable();

        if (rak_server_interface != nullptr)
        {
            const auto vtable = *static_cast<ptr_t**>(rak_server_interface);

            Instance()._func_start                    = reinterpret_cast<decltype(RakNet::_func_start)>
                (vtable[FunctionOffset::Start]);
            Instance()._func_send                     = reinterpret_cast<decltype(RakNet::_func_send)>
                (vtable[FunctionOffset::Send]);
            Instance()._func_receive                  = reinterpret_cast<decltype(RakNet::_func_receive)>
                (vtable[FunctionOffset::Receive]);
            Instance()._func_kick                     = reinterpret_cast<decltype(RakNet::_func_kick)>
                (vtable[FunctionOffset::Kick]);
            Instance()._func_deallocate_packet        = reinterpret_cast<decltype(RakNet::_func_deallocate_packet)>
                (vtable[FunctionOffset::DeallocatePacket]);
            Instance()._func_set_allowed_players      = reinterpret_cast<decltype(RakNet::_func_set_allowed_players)>
                (vtable[FunctionOffset::SetAllowedPlayers]);
            Instance()._func_get_last_ping            = reinterpret_cast<decltype(RakNet::_func_get_last_ping)>
                (vtable[FunctionOffset::GetLastPing]);
            Instance()._func_register_as_rpc          = reinterpret_cast<decltype(RakNet::_func_register_as_rpc)>
                (vtable[FunctionOffset::RegisterRpc]);
            Instance()._func_unregister_as_rpc        = reinterpret_cast<decltype(RakNet::_func_unregister_as_rpc)>
                (vtable[FunctionOffset::UnregisterRpc]);
            Instance()._func_rpc                      = reinterpret_cast<decltype(RakNet::_func_rpc)>
                (vtable[FunctionOffset::Rpc]);
            Instance()._func_get_local_ip             = reinterpret_cast<decltype(RakNet::_func_get_local_ip)>
                (vtable[FunctionOffset::GetLocalIp]);
            Instance()._func_get_internal_id          = reinterpret_cast<decltype(RakNet::_func_get_internal_id)>
                (vtable[FunctionOffset::GetInternalId]);
            Instance()._func_get_index_from_player_id = reinterpret_cast<decltype(RakNet::_func_get_index_from_player_id)>
                (vtable[FunctionOffset::GetIndexFromPlayerId]);
            Instance()._func_get_player_id_from_index = reinterpret_cast<decltype(RakNet::_func_get_player_id_from_index)>
                (vtable[FunctionOffset::GetPlayerIdFromIndex]);
            Instance()._func_add_to_ban_list          = reinterpret_cast<decltype(RakNet::_func_add_to_ban_list)>
                (vtable[FunctionOffset::AddBan]);
            Instance()._func_remove_from_ban_list     = reinterpret_cast<decltype(RakNet::_func_remove_from_ban_list)>
                (vtable[FunctionOffset::RemoveBan]);
            Instance()._func_clear_ban_list           = reinterpret_cast<decltype(RakNet::_func_clear_ban_list)>
                (vtable[FunctionOffset::ClearBan]);
            Instance()._func_set_timeout_time         = reinterpret_cast<decltype(RakNet::_func_set_timeout_time)>
                (vtable[FunctionOffset::SetTimeout]);

            {
                const UnprotectScope<sizeof(*vtable)> scope { vtable + FunctionOffset::RegisterRpc };
                vtable[FunctionOffset::RegisterRpc] = reinterpret_cast<ptr_t>(RegisterRpcHook);
            } {
                const UnprotectScope<sizeof(*vtable)> scope { vtable + FunctionOffset::Receive };
                vtable[FunctionOffset::Receive] = reinterpret_cast<ptr_t>(ReceiveHook);
            }

            Instance()._rak_server_interface = rak_server_interface;
        }

        return rak_server_interface;
    }

    static void ConnectHook(RPCParameters* const rpc) noexcept
    {
        if (Instance().OnConnect != nullptr)
        {
            if (const int sender = Instance().GetIndexFromPlayerId(rpc->sender); sender >= 0 && sender < MAX_PLAYERS)
            {
                  ptr_t data = nullptr;
                ubyte_t size = 0;
                {
                    const  adr_t data_pointer = rpc->input;
                    const uint_t data_length = BITS_TO_BYTES(rpc->numberOfBitsOfData);

                    if (data_length >= sizeof(kConnectSignature) + sizeof(ubyte_t))
                    {
                        constexpr auto signature = (HostEndian != NetEndian ?
                            utils::ct::bswap(kConnectSignature) : kConnectSignature);

                        adr_t iterator = (data_pointer + data_length) - (sizeof(kConnectSignature) + sizeof(ubyte_t));
                        while (iterator >= data_pointer && *reinterpret_cast<decltype(signature)*>(iterator) != signature)
                            --iterator;

                        if (iterator >= data_pointer)
                        {
                            if (const ubyte_t length = *reinterpret_cast<const ubyte_t*>
                                    (iterator + sizeof(kConnectSignature));
                                length <= (data_pointer + data_length) -
                                    (iterator + (sizeof(kConnectSignature) + sizeof(ubyte_t))))
                            {
                                data = iterator + (sizeof(kConnectSignature) + sizeof(ubyte_t));
                                size = length;
                            }
                        }
                    }
                }

                Instance().OnConnect(sender, data, size);
            }
        }

        Instance()._orig_connect_handler(rpc);
    }
    
    static void THISCALL RegisterRpcHook(const ptr_t _this, const adr_t rpc_id_ptr, RPCFunction rpc_handler) noexcept
    {
        if (*rpc_id_ptr == kConnectRpcId)
        {
            Instance()._orig_connect_handler = rpc_handler;

            rpc_handler = ConnectHook;
        }

        Instance().RegisterRpc(rpc_id_ptr, rpc_handler);
    }

    static int THISCALL DisconnectHook(const ptr_t _this, const int player_id, const int reason) noexcept
    {
        if (player_id >= 0 && player_id < MAX_PLAYERS)
        {
            if (Instance().OnDisconnect != nullptr)
                Instance().OnDisconnect(player_id);
        }

        Instance()._hook_disconnect.Disable();
        const auto result = reinterpret_cast<int(THISCALL*)(ptr_t, int, int) noexcept>
            (Instance()._hook_disconnect.Address())(_this, player_id, reason);
        Instance()._hook_disconnect.Enable();

        return result;
    }

    static Packet* THISCALL ReceiveHook(const ptr_t _this) noexcept
    {
        Packet* packet = Instance().Receive();

        if (Instance().OnPacket != nullptr)
        {
            while (packet != nullptr && Instance().OnPacket
                (packet->playerIndex, packet->data, packet->length))
            {
                Instance().DeallocatePacket(packet);
                packet = Instance().Receive();
            }
        }

        return packet;
    }

public:

    bool Start(const uword_t allowed_players, const udword_t depreciated,
        const sdword_t thread_sleep_timer, const uword_t port,
        const cstr_t host) noexcept
    {
        assert(_rak_server_interface != nullptr);
        assert(_func_start != nullptr);

        return _func_start(_rak_server_interface, allowed_players, depreciated,
            thread_sleep_timer, port, host);
    }

    bool Send(BitStream* const parameters, const PacketPriority priority,
        const PacketReliability reliability, const udword_t ordering_channel,
        const PlayerID player_id, const bool broadcast) noexcept
    {
        assert(_rak_server_interface != nullptr);
        assert(_func_send != nullptr);

        return _func_send(_rak_server_interface, parameters, priority, reliability,
            ordering_channel, player_id, broadcast);
    }

    Packet* Receive() noexcept
    {
        assert(_rak_server_interface != nullptr);
        assert(_func_receive != nullptr);

        return _func_receive(_rak_server_interface);
    }

    void Kick(const PlayerID player_id) noexcept
    {
        assert(_rak_server_interface != nullptr);
        assert(_func_kick != nullptr);

        _func_kick(_rak_server_interface, player_id);
    }

    void DeallocatePacket(Packet* const packet) noexcept
    {
        assert(_rak_server_interface != nullptr);
        assert(_func_deallocate_packet != nullptr);

        _func_deallocate_packet(_rak_server_interface, packet);
    }

    void SetAllowedPlayers(const uword_t number_allowed) noexcept
    {
        assert(_rak_server_interface != nullptr);
        assert(_func_set_allowed_players != nullptr);

        _func_set_allowed_players(_rak_server_interface, number_allowed);
    }

    int GetLastPing(const PlayerID player_id) const noexcept
    {
        assert(_rak_server_interface != nullptr);
        assert(_func_get_last_ping != nullptr);

        return _func_get_last_ping(_rak_server_interface, player_id);
    }

    void RegisterRpc(const cadr_t rpc_id_ptr, const RPCFunction rpc_handler) noexcept
    {
        assert(_rak_server_interface != nullptr);
        assert(_func_register_as_rpc != nullptr);

        _func_register_as_rpc(_rak_server_interface, rpc_id_ptr, rpc_handler);
    }

    void UnregisterRpc(const cadr_t rpc_id_ptr) noexcept
    {
        assert(_rak_server_interface != nullptr);
        assert(_func_unregister_as_rpc != nullptr);

        _func_unregister_as_rpc(_rak_server_interface, rpc_id_ptr);
    }

    bool Rpc(const cadr_t rpc_id_ptr, BitStream* const parameters,
        const PacketPriority priority, const PacketReliability reliability,
        const udword_t ordering_channel, const PlayerID player_id,
        const bool broadcast, const bool shift_timestamp) noexcept
    {
        assert(_rak_server_interface != nullptr);
        assert(_func_rpc != nullptr);

        return _func_rpc(_rak_server_interface, rpc_id_ptr, parameters, priority, reliability,
            ordering_channel, player_id, broadcast, shift_timestamp);
    }

    cstr_t GetLocalIp(const udword_t index) const noexcept
    {
        assert(_rak_server_interface != nullptr);
        assert(_func_get_local_ip != nullptr);

        return _func_get_local_ip(_rak_server_interface, index);
    }

    PlayerID GetInternalId() const noexcept
    {
        assert(_rak_server_interface != nullptr);
        assert(_func_get_internal_id != nullptr);

        return _func_get_internal_id(_rak_server_interface);
    }

    int GetIndexFromPlayerId(const PlayerID player_id) const noexcept
    {
        assert(_rak_server_interface != nullptr);
        assert(_func_get_index_from_player_id != nullptr);

        return _func_get_index_from_player_id(_rak_server_interface, player_id);
    }

    PlayerID GetPlayerIdFromIndex(const sdword_t index) const noexcept
    {
        assert(_rak_server_interface != nullptr);
        assert(_func_get_player_id_from_index != nullptr);

        return _func_get_player_id_from_index(_rak_server_interface, index);
    }

    void AddToBanList(const cstr_t ip, const udword_t milliseconds) noexcept
    {
        assert(_rak_server_interface != nullptr);
        assert(_func_add_to_ban_list != nullptr);

        _func_add_to_ban_list(_rak_server_interface, ip, milliseconds);
    }

    void RemoveFromBanList(const cstr_t ip) noexcept
    {
        assert(_rak_server_interface != nullptr);
        assert(_func_remove_from_ban_list != nullptr);

        _func_remove_from_ban_list(_rak_server_interface, ip);
    }

    void ClearBanList() noexcept
    {
        assert(_rak_server_interface != nullptr);
        assert(_func_clear_ban_list != nullptr);

        _func_clear_ban_list(_rak_server_interface);
    }

    void SetTimeoutTime(const RakNetTime time_ms, const PlayerID target) noexcept
    {
        assert(_rak_server_interface != nullptr);
        assert(_func_set_timeout_time != nullptr);

        _func_set_timeout_time(_rak_server_interface, time_ms, target);
    }

private:

    ptr_t _rak_server_interface = nullptr;

public:

    std::function<void(uword_t, ptr_t, ubyte_t)> OnConnect;
    std::function<bool(uword_t, ptr_t,  uint_t)> OnPacket;
    std::function<void(uword_t)>                 OnDisconnect;

private:

    bool _player_status[MAX_PLAYERS] = {};

    RPCFunction _orig_connect_handler = nullptr;

    JumpHook _hook_disconnect;
    JumpHook _hook_get_rak_server_interface;

private:

    bool     (THISCALL* _func_start)
                (ptr_t, uword_t, udword_t, sdword_t, uword_t, cstr_t)
                    noexcept = nullptr;
    bool     (THISCALL* _func_send)
                (ptr_t, BitStream*, PacketPriority, PacketReliability, udword_t, PlayerID, bool)
                    noexcept = nullptr;
    Packet*  (THISCALL* _func_receive)
                (ptr_t)
                    noexcept = nullptr;
    void     (THISCALL* _func_kick)
                (ptr_t, PlayerID)
                    noexcept = nullptr;
    void     (THISCALL* _func_deallocate_packet)
                (ptr_t, Packet*)
                    noexcept = nullptr;
    void     (THISCALL* _func_set_allowed_players)
                (ptr_t, uword_t)
                    noexcept = nullptr;
    int      (THISCALL* _func_get_last_ping)
                (ptr_t, PlayerID)
                    noexcept = nullptr;
    void     (THISCALL* _func_register_as_rpc)
                (ptr_t, cadr_t, RPCFunction)
                    noexcept = nullptr;
    void     (THISCALL* _func_unregister_as_rpc)
                (ptr_t, cadr_t)
                    noexcept = nullptr;
    bool     (THISCALL* _func_rpc)
                (ptr_t, cadr_t, BitStream*, PacketPriority, PacketReliability, udword_t, PlayerID, bool, bool)
                    noexcept = nullptr;
    cstr_t   (THISCALL* _func_get_local_ip)
                (ptr_t, udword_t)
                    noexcept = nullptr;
    PlayerID (THISCALL* _func_get_internal_id)
                (ptr_t)
                    noexcept = nullptr;
    int      (THISCALL* _func_get_index_from_player_id)
                (ptr_t, PlayerID)
                    noexcept = nullptr;
    PlayerID (THISCALL* _func_get_player_id_from_index)
                (ptr_t, sdword_t)
                    noexcept = nullptr;
    void     (THISCALL* _func_add_to_ban_list)
                (ptr_t, cstr_t, udword_t)
                    noexcept = nullptr;
    void     (THISCALL* _func_remove_from_ban_list)
                (ptr_t, cstr_t)
                    noexcept = nullptr;
    void     (THISCALL* _func_clear_ban_list)
                (ptr_t)
                    noexcept = nullptr;
    void     (THISCALL* _func_set_timeout_time)
                (ptr_t, RakNetTime, PlayerID)
                    noexcept = nullptr;

};
