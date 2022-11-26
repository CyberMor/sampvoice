/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <memory>
#include <cstdint>
#include <functional>
#include <shared_mutex>
#include <vector>
#include <array>

#include <MPMCQueue.h>
#include <raknet/bitstream.h>
#include <raknet/networktypes.h>
#include <ysf/structs.h>

#include "memory.hpp"

struct RakNet {

    RakNet() = delete;
    ~RakNet() = delete;
    RakNet(const RakNet&) = delete;
    RakNet(RakNet&&) = delete;
    RakNet& operator=(const RakNet&) = delete;
    RakNet& operator=(RakNet&&) = delete;

private:

    using ConnectCallback    = std::function<void(uint16_t, RPCParameters&)>;
    using PacketCallback     = std::function<bool(uint16_t, Packet&)>;
    using DisconnectCallback = std::function<void(uint16_t)>;

public:

    static bool Init(const void* server_base_addr) noexcept;
    static void Free() noexcept;

    static bool IsLoaded() noexcept;
    static void Process() noexcept;

    static bool SendRPC(uint8_t rpc_id, uint16_t player_id, const void* data, int size);
    static bool SendPacket(uint8_t packet_id, uint16_t player_id, const void* data, int size);
    static bool KickPlayer(uint16_t player_id) noexcept;

public:

    static void SetConnectCallback(ConnectCallback&& callback) noexcept;
    static void SetPacketCallback(PacketCallback&& callback) noexcept;
    static void SetDisconnectCallback(DisconnectCallback&& callback) noexcept;

private:

    static void* GetRakServerInterfaceHook() noexcept;
    static void ConnectHook(RPCParameters* parameters) noexcept;
    static void THISCALL RegisterRpcHook(void* _this, uint8_t* rpc_number, RPCFunction rpc_handler) noexcept;
    static int THISCALL DisconnectHook(void* _this, int player_id, int reason) noexcept;
    static Packet* THISCALL ReceiveHook(void* _this) noexcept;

private:

    static bool                          _init_status;
    static bool                          _load_status;

    static void*                         _rak_server_interface;

    static ConnectCallback               _connect_callback;
    static PacketCallback                _packet_callback;
    static DisconnectCallback            _disconnect_callback;

    static std::array<bool, MAX_PLAYERS> _player_status;

    static RPCFunction                   _orig_connect_handler;

    static Memory::JumpHook              _hook_disconnect;
    static Memory::JumpHook              _hook_get_rak_server_interface;

private:

    struct SendRpcInfo {

        SendRpcInfo() noexcept = default;
        ~SendRpcInfo() noexcept = default;
        SendRpcInfo(const SendRpcInfo&) = default;
        SendRpcInfo(SendRpcInfo&&) noexcept = default;
        SendRpcInfo& operator=(const SendRpcInfo&) = default;
        SendRpcInfo& operator=(SendRpcInfo&&) noexcept = default;

    public:

        SendRpcInfo(std::unique_ptr<BitStream>&& bit_stream, const uint16_t player_id, const uint8_t rpc_id) noexcept
            : bit_stream { std::move(bit_stream) }
            , player_id  { player_id }
            , rpc_id     { rpc_id }
        {}

    public:

        std::unique_ptr<BitStream> bit_stream = nullptr;
        uint16_t                   player_id  = 0;
        uint8_t                    rpc_id     = 0;

    };

    struct SendPacketInfo {

        SendPacketInfo() noexcept = default;
        ~SendPacketInfo() noexcept = default;
        SendPacketInfo(const SendPacketInfo&) = default;
        SendPacketInfo(SendPacketInfo&&) noexcept = default;
        SendPacketInfo& operator=(const SendPacketInfo&) = default;
        SendPacketInfo& operator=(SendPacketInfo&&) noexcept = default;

    public:

        SendPacketInfo(std::unique_ptr<BitStream>&& bit_stream, const uint16_t player_id) noexcept
            : bit_stream { std::move(bit_stream) }
            , player_id  { player_id }
        {}

    public:

        std::unique_ptr<BitStream> bit_stream = nullptr;
        uint16_t                   player_id  = 0;

    };

    static std::shared_mutex         _rpc_queue_mutex;
    static MPMCQueue<SendRpcInfo>    _rpc_queue;

    static std::shared_mutex         _packet_queue_mutex;
    static MPMCQueue<SendPacketInfo> _packet_queue;

    static std::shared_mutex         _kick_queue_mutex;
    static MPMCQueue<uint16_t>       _kick_queue;

public:

    static bool         Start                (uint16_t          allowed_players,
                                              uint32_t          depreciated,
                                              int32_t           thread_sleep_timer,
                                              uint16_t          port,
                                              const char*       host)             noexcept;

    static bool         Send                 (BitStream*        parameters,
                                              PacketPriority    priority,
                                              PacketReliability reliability,
                                              uint32_t          ordering_channel,
                                              PlayerID          player_id,
                                              bool              broadcast)        noexcept;

    static Packet*      Receive              (void)                               noexcept;

    static void         Kick                 (PlayerID          player_id)        noexcept;

    static void         DeallocatePacket     (Packet*           packet)           noexcept;

    static void         SetAllowedPlayers    (uint16_t          number_allowed)   noexcept;

    static int          GetLastPing          (PlayerID          player_id)        noexcept;

    static void         RegisterRpc          (const uint8_t*    rpc_id_pointer,
                                              RPCFunction       rpc_handler)      noexcept;

    static void         UnregisterRpc        (const uint8_t*    rpc_id_pointer)   noexcept;

    static bool         Rpc                  (const uint8_t*    rpc_id_pointer,
                                              BitStream*        parameters,
                                              PacketPriority    priority,
                                              PacketReliability reliability,
                                              uint32_t          ordering_channel,
                                              PlayerID          player_id,
                                              bool              broadcast,
                                              bool              shift_timestamp)  noexcept;

    static const char*  GetLocalIp           (uint32_t          index)            noexcept;

    static PlayerID     GetInternalId        (void)                               noexcept;

    static int          GetIndexFromPlayerId (PlayerID          player_id)        noexcept;

    static PlayerID     GetPlayerIdFromIndex (int32_t           index)            noexcept;

    static void         AddToBanList         (const char*       ip,
                                              uint32_t          milliseconds)     noexcept;

    static void         RemoveFromBanList    (const char*       ip)               noexcept;

    static void         ClearBanList         (void)                               noexcept;

    static void         SetTimeoutTime       (RakNetTime        time_ms,
                                              PlayerID          target)           noexcept;

private:

    using StartFuncType                 = bool        (THISCALL*)(void*             _this,
                                                                  uint16_t          allowed_players,
                                                                  uint32_t          depreciated,
                                                                  int32_t           thread_sleep_timer,
                                                                  uint16_t          port,
                                                                  const char*       host);

    using SendFuncType                  = bool        (THISCALL*)(void*             _this,
                                                                  BitStream*        parameters,
                                                                  PacketPriority    priority,
                                                                  PacketReliability reliability,
                                                                  uint32_t          ordering_channel,
                                                                  PlayerID          player_id,
                                                                  bool              broadcast);

    using ReceiveFuncType               = Packet*     (THISCALL*)(void*             _this);

    using KickFuncType                  = void        (THISCALL*)(void*             _this,
                                                                  PlayerID          player_id);

    using DeallocatePacketFuncType      = void        (THISCALL*)(void*             _this,
                                                                  Packet*           packet);

    using SetAllowedPlayersFuncType     = void        (THISCALL*)(void*             _this,
                                                                  uint16_t          number_allowed);

    using GetLastPingFuncType           = int         (THISCALL*)(void*             _this,
                                                                  PlayerID          player_id);

    using RegisterAsRpcFuncType         = void        (THISCALL*)(void*             _this,
                                                                  const uint8_t*    rpc_id_pointer,
                                                                  RPCFunction       rpc_handler);

    using UnregisterAsRpcFuncType       = void        (THISCALL*)(void*             _this,
                                                                  const uint8_t*    rpc_id_pointer);

    using RpcFuncType                   = bool        (THISCALL*)(void*             _this,
                                                                  const uint8_t*    rpc_id_pointer,
                                                                  BitStream*        parameters,
                                                                  PacketPriority    priority,
                                                                  PacketReliability reliability,
                                                                  uint32_t          ordering_channel,
                                                                  PlayerID          player_id,
                                                                  bool              broadcast,
                                                                  bool              shift_timestamp);

    using GetLocalIpFuncType            = const char* (THISCALL*)(void*             _this,
                                                                  uint32_t          index);

    using GetInternalIdFuncType         = PlayerID    (THISCALL*)(void*             _this);

    using GetIndexFromPlayerIdFuncType  = int         (THISCALL*)(void*             _this,
                                                                  PlayerID          player_id);

    using GetPlayerIdFromIndexFuncType  = PlayerID    (THISCALL*)(void*             _this,
                                                                  int32_t           index);

    using AddToBanListFuncType          = void        (THISCALL*)(void*             _this,
                                                                  const char*       ip,
                                                                  uint32_t          milliseconds);

    using RemoveFromBanListFuncType     = void        (THISCALL*)(void*             _this,
                                                                  const char*       ip);

    using ClearBanListFuncType          = void        (THISCALL*)(void*             _this);

    using SetTimeoutTimeFuncType        = void        (THISCALL*)(void*             _this,
                                                                  RakNetTime        time_ms,
                                                                  PlayerID          target);

private:

    static StartFuncType                _func_start;
    static SendFuncType                 _func_send;
    static ReceiveFuncType              _func_receive;
    static KickFuncType                 _func_kick;
    static DeallocatePacketFuncType     _func_deallocate_packet;
    static SetAllowedPlayersFuncType    _func_set_allowed_players;
    static GetLastPingFuncType          _func_get_last_ping;
    static RegisterAsRpcFuncType        _func_register_as_rpc;
    static UnregisterAsRpcFuncType      _func_unregister_as_rpc;
    static RpcFuncType                  _func_rpc;
    static GetLocalIpFuncType           _func_get_local_ip;
    static GetInternalIdFuncType        _func_get_internal_id;
    static GetIndexFromPlayerIdFuncType _func_get_index_from_player_id;
    static GetPlayerIdFromIndexFuncType _func_get_player_id_from_index;
    static AddToBanListFuncType         _func_add_to_ban_list;
    static RemoveFromBanListFuncType    _func_remove_from_ban_list;
    static ClearBanListFuncType         _func_clear_ban_list;
    static SetTimeoutTimeFuncType       _func_set_timeout_time;

};
