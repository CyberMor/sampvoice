/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "raknet.h"

#include "logger.h"

namespace
{
    constexpr uint8_t kConnectRaknetRcpId = 25;

    constexpr const char* kGetRakServerInterfaceFuncPattern =
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

    constexpr const char* kGetRakServerInterfaceFuncMask =
#ifdef _WIN32
        "xxxxxxxxxxxxxxxx????x????xxxxxxxxxxx?xxxxxx"
#else
        "xx????xx?xx?x????xxxxxx????xxxx?xx?xxxx"
#endif
        ;

    constexpr auto kOnPlayerDisconnectAddr =
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

bool RakNet::Init(const void* const server_base_addr) noexcept
{
    if (_init_status) return false;

    Logger::Log("[dbg:raknet:init] : module initializing...");

    void*  module_addr;
    size_t module_size;

    if (!Memory::GetModuleInfo(server_base_addr, module_addr, module_size))
    {
        Logger::Log("[err:raknet:init] : failed to get module info");
        return false;
    }

    const auto function_ptr = Memory::Scanner(module_addr, module_size).Find
        (kGetRakServerInterfaceFuncPattern, kGetRakServerInterfaceFuncMask);
    if (function_ptr == nullptr)
    {
        Logger::Log("[err:raknet:init] : failed to find 'GetRakServerInterface' function address");
        return false;
    }

    _hook_get_rak_server_interface = { (void*)((size_t)(function_ptr) - 7),
        (void*)(GetRakServerInterfaceHook) };
    _hook_disconnect = { (void*)(kOnPlayerDisconnectAddr),
        (void*)(DisconnectHook) };

    _load_status = false;

    Logger::Log("[dbg:raknet:init] : module initialized");

    _init_status = true;

    return true;
}

void RakNet::Free() noexcept
{
    if (_init_status)
    {
        Logger::Log("[dbg:raknet:free] : module releasing...");

        if (_load_status)
        {
            const auto vtable = *static_cast<void***>(_rak_server_interface);

            if (_orig_connect_handler != nullptr)
            {
                const uint8_t rpc_connect_id = kConnectRaknetRcpId;
                UnregisterRpc(&rpc_connect_id);
                RegisterRpc(&rpc_connect_id, _orig_connect_handler);
            }

            _orig_connect_handler = nullptr;

            {
                const Memory::UnprotectScope<sizeof(*vtable)> scope { vtable + FunctionOffset::RegisterRpc };
                vtable[FunctionOffset::RegisterRpc] = reinterpret_cast<void*>(_func_rpc);
            } {
                const Memory::UnprotectScope<sizeof(*vtable)> scope { vtable + FunctionOffset::Receive };
                vtable[FunctionOffset::Receive] = reinterpret_cast<void*>(_func_receive);
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

        _load_status = false;

        for (uint16_t player_id = 0; player_id < MAX_PLAYERS; ++player_id)
        {
            if (_player_status[player_id])
            {
                if (_disconnect_callback != nullptr)
                {
                    _disconnect_callback(player_id);
                }
            }

            _player_status[player_id] = false;
        }

        _connect_callback    = nullptr;
        _packet_callback     = nullptr;
        _disconnect_callback = nullptr;

        _hook_disconnect               = {};
        _hook_get_rak_server_interface = {};

        Logger::Log("[dbg:raknet:free] : module released");

        _init_status = false;
    }
}

bool RakNet::IsLoaded() noexcept
{
    return _load_status;
}

void RakNet::Process() noexcept
{
    // Rpc's sending...
    {
        const std::unique_lock lock { _rpc_queue_mutex };

        for (SendRpcInfo send_rpc_info; _rpc_queue.try_pop(send_rpc_info);)
        {
            Rpc(&send_rpc_info.rpc_id, send_rpc_info.bit_stream.get(), PacketPriority::MEDIUM_PRIORITY,
                PacketReliability::RELIABLE_ORDERED, '\0', GetPlayerIdFromIndex(send_rpc_info.player_id),
                send_rpc_info.player_id == 0xffff, false);
        }
    }

    // Packets sending...
    {
        const std::unique_lock lock { _packet_queue_mutex };

        for (SendPacketInfo send_packet_info; _packet_queue.try_pop(send_packet_info);)
        {
            Send(send_packet_info.bit_stream.get(), PacketPriority::MEDIUM_PRIORITY,
                PacketReliability::RELIABLE_ORDERED, '\0',
                GetPlayerIdFromIndex(send_packet_info.player_id),
                send_packet_info.player_id == 0xffff);
        }
    }

    // Kicking players...
    {
        const std::unique_lock lock { _kick_queue_mutex };

        for (uint16_t kick_player_id; _kick_queue.try_pop(kick_player_id);)
        {
            Kick(GetPlayerIdFromIndex(kick_player_id));
        }
    }
}

bool RakNet::SendRPC(const uint8_t rpc_id, const uint16_t player_id, const void* const data, const int size)
{
    auto bit_stream = std::make_unique<BitStream>((uint8_t*)(data), size, true);

    const std::shared_lock lock { _rpc_queue_mutex };

    return _rpc_queue.try_emplace(std::move(bit_stream), player_id, rpc_id);
}

bool RakNet::SendPacket(const uint8_t packet_id, const uint16_t player_id, const void* const data, const int size)
{
    auto bit_stream = std::make_unique<BitStream>(sizeof(packet_id) + size);

    bit_stream->Write(packet_id);
    bit_stream->Write(static_cast<const char*>(data), size);

    const std::shared_lock lock { _packet_queue_mutex };

    return _packet_queue.try_emplace(std::move(bit_stream), player_id);
}

bool RakNet::KickPlayer(const uint16_t player_id) noexcept
{
    const std::shared_lock lock { _kick_queue_mutex };

    return _kick_queue.try_emplace(player_id);
}

// ----------------------------------------------------------------------------

void RakNet::SetConnectCallback(ConnectCallback&& callback) noexcept
{
    if (_init_status) _connect_callback = std::move(callback);
}

void RakNet::SetPacketCallback(PacketCallback&& callback) noexcept
{
    if (_init_status) _packet_callback = std::move(callback);
}

void RakNet::SetDisconnectCallback(DisconnectCallback&& callback) noexcept
{
    if (_init_status) _disconnect_callback = std::move(callback);
}

// ----------------------------------------------------------------------------

void* RakNet::GetRakServerInterfaceHook() noexcept
{
    _hook_get_rak_server_interface.Disable();
    const auto orig_interface = reinterpret_cast<void*(*)()>(_hook_get_rak_server_interface.GetPatch().GetAddr())();
    _hook_get_rak_server_interface.Enable();

    if (orig_interface != nullptr)
    {
        const auto vtable = *static_cast<void***>(orig_interface);

        _func_start                    = reinterpret_cast<StartFuncType>                (vtable[FunctionOffset::Start]);
        _func_send                     = reinterpret_cast<SendFuncType>                 (vtable[FunctionOffset::Send]);
        _func_receive                  = reinterpret_cast<ReceiveFuncType>              (vtable[FunctionOffset::Receive]);
        _func_kick                     = reinterpret_cast<KickFuncType>                 (vtable[FunctionOffset::Kick]);
        _func_deallocate_packet        = reinterpret_cast<DeallocatePacketFuncType>     (vtable[FunctionOffset::DeallocatePacket]);
        _func_set_allowed_players      = reinterpret_cast<SetAllowedPlayersFuncType>    (vtable[FunctionOffset::SetAllowedPlayers]);
        _func_get_last_ping            = reinterpret_cast<GetLastPingFuncType>          (vtable[FunctionOffset::GetLastPing]);
        _func_register_as_rpc          = reinterpret_cast<RegisterAsRpcFuncType>        (vtable[FunctionOffset::RegisterRpc]);
        _func_unregister_as_rpc        = reinterpret_cast<UnregisterAsRpcFuncType>      (vtable[FunctionOffset::UnregisterRpc]);
        _func_rpc                      = reinterpret_cast<RpcFuncType>                  (vtable[FunctionOffset::Rpc]);
        _func_get_local_ip             = reinterpret_cast<GetLocalIpFuncType>           (vtable[FunctionOffset::GetLocalIp]);
        _func_get_internal_id          = reinterpret_cast<GetInternalIdFuncType>        (vtable[FunctionOffset::GetInternalId]);
        _func_get_index_from_player_id = reinterpret_cast<GetIndexFromPlayerIdFuncType> (vtable[FunctionOffset::GetIndexFromPlayerId]);
        _func_get_player_id_from_index = reinterpret_cast<GetPlayerIdFromIndexFuncType> (vtable[FunctionOffset::GetPlayerIdFromIndex]);
        _func_add_to_ban_list          = reinterpret_cast<AddToBanListFuncType>         (vtable[FunctionOffset::AddBan]);
        _func_remove_from_ban_list     = reinterpret_cast<RemoveFromBanListFuncType>    (vtable[FunctionOffset::RemoveBan]);
        _func_clear_ban_list           = reinterpret_cast<ClearBanListFuncType>         (vtable[FunctionOffset::ClearBan]);
        _func_set_timeout_time         = reinterpret_cast<SetTimeoutTimeFuncType>       (vtable[FunctionOffset::SetTimeout]);

        {
            const Memory::UnprotectScope<sizeof(*vtable)> scope { vtable + FunctionOffset::RegisterRpc };
            vtable[FunctionOffset::RegisterRpc] = reinterpret_cast<void*>(RegisterRpcHook);
        } {
            const Memory::UnprotectScope<sizeof(*vtable)> scope { vtable + FunctionOffset::Receive };
            vtable[FunctionOffset::Receive] = reinterpret_cast<void*>(ReceiveHook);
        }

        _rak_server_interface = orig_interface;

        _load_status = true;
    }

    return orig_interface;
}

void RakNet::ConnectHook(RPCParameters* const rpc) noexcept
{
    if (const auto sender = GetIndexFromPlayerId(rpc->sender); sender >= 0 && sender < MAX_PLAYERS)
    {
        if (_player_status[sender])
        {
            if (_disconnect_callback != nullptr)
            {
                _disconnect_callback(sender);
            }
        }

        _player_status[sender] = true;

        if (_connect_callback != nullptr)
        {
            _connect_callback(sender, *rpc);
        }
    }

    _orig_connect_handler(rpc);
}

void THISCALL RakNet::RegisterRpcHook(void* const _this, uint8_t* const rpc_id_ptr, RPCFunction rpc_handler) noexcept
{
    if (*rpc_id_ptr == kConnectRaknetRcpId)
    {
        _orig_connect_handler = rpc_handler;
        rpc_handler = ConnectHook;
    }

    RegisterRpc(rpc_id_ptr, rpc_handler);
}

int THISCALL RakNet::DisconnectHook(void* const _this, const int player_id, const int reason) noexcept
{
    if (player_id >= 0 && player_id < MAX_PLAYERS)
    {
        if (_player_status[player_id])
        {
            if (_disconnect_callback != nullptr)
            {
                _disconnect_callback(player_id);
            }
        }

        _player_status[player_id] = false;
    }

    _hook_disconnect.Disable();
    const auto result = reinterpret_cast<int(THISCALL*)(void*, int, int)>
        (_hook_disconnect.GetPatch().GetAddr())(_this, player_id, reason);
    _hook_disconnect.Enable();

    return result;
}

Packet* THISCALL RakNet::ReceiveHook(void* const _this) noexcept
{
    Packet* packet = Receive();

    if (_packet_callback != nullptr)
    {
        while (packet != nullptr)
        {
            if (_packet_callback(packet->playerIndex, *packet))
                break;

            DeallocatePacket(packet);
            packet = Receive();
        }
    }

    return packet;
}

// ----------------------------------------------------------------------------

bool RakNet::_init_status = false;
bool RakNet::_load_status = false;

void* RakNet::_rak_server_interface = nullptr;

RakNet::ConnectCallback    RakNet::_connect_callback = nullptr;
RakNet::PacketCallback     RakNet::_packet_callback = nullptr;
RakNet::DisconnectCallback RakNet::_disconnect_callback = nullptr;

std::array<bool, MAX_PLAYERS> RakNet::_player_status;

RPCFunction RakNet::_orig_connect_handler = nullptr;

Memory::JumpHook RakNet::_hook_disconnect;
Memory::JumpHook RakNet::_hook_get_rak_server_interface;

// ----------------------------------------------------------------------------

std::shared_mutex              RakNet::_rpc_queue_mutex;
MPMCQueue<RakNet::SendRpcInfo> RakNet::_rpc_queue { 16 * MAX_PLAYERS };

std::shared_mutex                 RakNet::_packet_queue_mutex;
MPMCQueue<RakNet::SendPacketInfo> RakNet::_packet_queue { 16 * MAX_PLAYERS };

std::shared_mutex   RakNet::_kick_queue_mutex;
MPMCQueue<uint16_t> RakNet::_kick_queue { MAX_PLAYERS };

// ----------------------------------------------------------------------------

bool RakNet::Start(const uint16_t allowed_players, const uint32_t depreciated,
                   const int32_t thread_sleep_timer, const uint16_t port,
                   const char* const host) noexcept
{
    return _load_status && _func_start != nullptr &&
        _func_start(_rak_server_interface, allowed_players, depreciated,
            thread_sleep_timer, port, host);
}

bool RakNet::Send(BitStream* const parameters, const PacketPriority priority,
                  const PacketReliability reliability, const uint32_t ordering_channel,
                  const PlayerID player_id, const bool broadcast) noexcept
{
    return _load_status && _func_send != nullptr &&
        _func_send(_rak_server_interface, parameters, priority, reliability,
            ordering_channel, player_id, broadcast);
}

Packet* RakNet::Receive() noexcept
{
    return _load_status && _func_receive != nullptr ?
        _func_receive(_rak_server_interface) : nullptr;
}

void RakNet::Kick(const PlayerID player_id) noexcept
{
    if (_load_status && _func_kick != nullptr)
        _func_kick(_rak_server_interface, player_id);
}

void RakNet::DeallocatePacket(Packet* const packet) noexcept
{
    if (_load_status && _func_deallocate_packet != nullptr)
        _func_deallocate_packet(_rak_server_interface, packet);
}

void RakNet::SetAllowedPlayers(const uint16_t number_allowed) noexcept
{
    if (_load_status && _func_set_allowed_players != nullptr)
        _func_set_allowed_players(_rak_server_interface, number_allowed);
}

int RakNet::GetLastPing(const PlayerID player_id) noexcept
{
    return _load_status && _func_get_last_ping != nullptr ?
        _func_get_last_ping(_rak_server_interface, player_id) : -1;
}

void RakNet::RegisterRpc(const uint8_t* const rpc_id_ptr, const RPCFunction rpc_handler) noexcept
{
    if (_load_status && _func_register_as_rpc != nullptr)
        _func_register_as_rpc(_rak_server_interface, rpc_id_ptr, rpc_handler);
}

void RakNet::UnregisterRpc(const uint8_t* const rpc_id_ptr) noexcept
{
    if (_load_status && _func_unregister_as_rpc != nullptr)
        _func_unregister_as_rpc(_rak_server_interface, rpc_id_ptr);
}

bool RakNet::Rpc(const uint8_t* const rpc_id_ptr, BitStream* const parameters,
                 const PacketPriority priority, const PacketReliability reliability,
                 const uint32_t ordering_channel, const PlayerID player_id,
                 const bool broadcast, const bool shift_timestamp) noexcept
{
    return _load_status && _func_rpc != nullptr &&
        _func_rpc(_rak_server_interface, rpc_id_ptr, parameters, priority,
            reliability, ordering_channel, player_id, broadcast, shift_timestamp);
}

const char* RakNet::GetLocalIp(const uint32_t index) noexcept
{
    return _load_status && _func_get_local_ip != nullptr ?
        _func_get_local_ip(_rak_server_interface, index) : nullptr;
}

PlayerID RakNet::GetInternalId() noexcept
{
    return _load_status && _func_get_internal_id != nullptr ?
        _func_get_internal_id(_rak_server_interface) : UNASSIGNED_PLAYER_ID;
}

int RakNet::GetIndexFromPlayerId(const PlayerID player_id) noexcept
{
    return _load_status && _func_get_index_from_player_id != nullptr ?
        _func_get_index_from_player_id(_rak_server_interface, player_id) : -1;
}

PlayerID RakNet::GetPlayerIdFromIndex(const int32_t index) noexcept
{
    return _load_status && _func_get_player_id_from_index != nullptr ?
        _func_get_player_id_from_index(_rak_server_interface, index) : UNASSIGNED_PLAYER_ID;
}

void RakNet::AddToBanList(const char* const ip, const uint32_t milliseconds) noexcept
{
    if (_load_status && _func_add_to_ban_list != nullptr)
        _func_add_to_ban_list(_rak_server_interface, ip, milliseconds);
}

void RakNet::RemoveFromBanList(const char* const ip) noexcept
{
    if (_load_status && _func_remove_from_ban_list != nullptr)
        _func_remove_from_ban_list(_rak_server_interface, ip);
}

void RakNet::ClearBanList() noexcept
{
    if (_load_status && _func_clear_ban_list != nullptr)
        _func_clear_ban_list(_rak_server_interface);
}

void RakNet::SetTimeoutTime(const RakNetTime time_ms, const PlayerID target) noexcept
{
    if (_load_status && _func_set_timeout_time != nullptr)
        _func_set_timeout_time(_rak_server_interface, time_ms, target);
}

// ----------------------------------------------------------------------------

RakNet::StartFuncType                RakNet::_func_start                    = nullptr;
RakNet::SendFuncType                 RakNet::_func_send                     = nullptr;
RakNet::ReceiveFuncType              RakNet::_func_receive                  = nullptr;
RakNet::KickFuncType                 RakNet::_func_kick                     = nullptr;
RakNet::DeallocatePacketFuncType     RakNet::_func_deallocate_packet        = nullptr;
RakNet::SetAllowedPlayersFuncType    RakNet::_func_set_allowed_players      = nullptr;
RakNet::GetLastPingFuncType          RakNet::_func_get_last_ping            = nullptr;
RakNet::RegisterAsRpcFuncType        RakNet::_func_register_as_rpc          = nullptr;
RakNet::UnregisterAsRpcFuncType      RakNet::_func_unregister_as_rpc        = nullptr;
RakNet::RpcFuncType                  RakNet::_func_rpc                      = nullptr;
RakNet::GetLocalIpFuncType           RakNet::_func_get_local_ip             = nullptr;
RakNet::GetInternalIdFuncType        RakNet::_func_get_internal_id          = nullptr;
RakNet::GetIndexFromPlayerIdFuncType RakNet::_func_get_index_from_player_id = nullptr;
RakNet::GetPlayerIdFromIndexFuncType RakNet::_func_get_player_id_from_index = nullptr;
RakNet::AddToBanListFuncType         RakNet::_func_add_to_ban_list          = nullptr;
RakNet::RemoveFromBanListFuncType    RakNet::_func_remove_from_ban_list     = nullptr;
RakNet::ClearBanListFuncType         RakNet::_func_clear_ban_list           = nullptr;
RakNet::SetTimeoutTimeFuncType       RakNet::_func_set_timeout_time         = nullptr;
