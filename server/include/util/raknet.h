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

class RakNet {

    RakNet() = delete;
    ~RakNet() = delete;
    RakNet(const RakNet&) = delete;
    RakNet(RakNet&&) = delete;
    RakNet& operator=(const RakNet&) = delete;
    RakNet& operator=(RakNet&&) = delete;

private:

    using ConnectCallback = std::function<void(uint16_t, RPCParameters&)>;
    using PacketCallback = std::function<bool(uint16_t, Packet&)>;
    using DisconnectCallback = std::function<void(uint16_t)>;

public:

    static bool Init(const void* serverBaseAddr) noexcept;
    static void Free() noexcept;

    static bool IsLoaded() noexcept;
    static void Process() noexcept;

    static bool SendRPC(uint8_t rpcId, uint16_t playerId, const void* dataPtr, int dataSize);
    static bool SendPacket(uint8_t packetId, uint16_t playerId, const void* dataPtr, int dataSize);
    static bool KickPlayer(uint16_t playerId) noexcept;

    static std::size_t AddConnectCallback(ConnectCallback callback) noexcept;
    static std::size_t AddPacketCallback(PacketCallback callback) noexcept;
    static std::size_t AddDisconnectCallback(DisconnectCallback callback) noexcept;
    static void RemoveConnectCallback(std::size_t callback) noexcept;
    static void RemovePacketCallback(std::size_t callback) noexcept;
    static void RemoveDisconnectCallback(std::size_t callback) noexcept;

private:

    static void* GetRakServerInterfaceHook() noexcept;
    static void ConnectHook(RPCParameters* parameters) noexcept;
    static void THISCALL RegisterRpcHook(void* _this, uint8_t* rpcNumber, RPCFunction rpcHandler) noexcept;
    static int THISCALL DisconnectHook(void* _this, int playerId, int reason) noexcept;
    static Packet* THISCALL ReceiveHook(void* _this) noexcept;

private:

    static bool initStatus;
    static bool loadStatus;

    static void* pRakServerInterface;

    static std::vector<ConnectCallback> connectCallbacks;
    static std::vector<PacketCallback> packetCallbacks;
    static std::vector<DisconnectCallback> disconnectCallbacks;

    static std::array<bool, MAX_PLAYERS> playerStatus;

    static RPCFunction origConnectHandler;

    static Memory::JumpHookPtr hookDisconnect;
    static Memory::JumpHookPtr hookGetRakServerInterface;

private:

    struct SendRpcInfo {

        SendRpcInfo() noexcept = default;
        SendRpcInfo(const SendRpcInfo&) = default;
        SendRpcInfo(SendRpcInfo&&) noexcept = default;
        SendRpcInfo& operator=(const SendRpcInfo&) = default;
        SendRpcInfo& operator=(SendRpcInfo&&) noexcept = default;

    private:

        using BitStreamPtr = std::unique_ptr<BitStream>;

    public:

        explicit SendRpcInfo(BitStreamPtr bitStream, uint16_t playerId, uint8_t rpcId) noexcept
            : bitStream(std::move(bitStream)), playerId(playerId), rpcId(rpcId) {}

        ~SendRpcInfo() noexcept = default;

    public:

        BitStreamPtr bitStream { nullptr };
        uint16_t playerId { NULL };
        uint8_t rpcId { NULL };

    };

    struct SendPacketInfo {

        SendPacketInfo() noexcept = default;
        SendPacketInfo(const SendPacketInfo&) = default;
        SendPacketInfo(SendPacketInfo&&) noexcept = default;
        SendPacketInfo& operator=(const SendPacketInfo&) = default;
        SendPacketInfo& operator=(SendPacketInfo&&) noexcept = default;

    private:

        using BitStreamPtr = std::unique_ptr<BitStream>;

    public:

        explicit SendPacketInfo(BitStreamPtr bitStream, uint16_t playerId) noexcept
            : bitStream(std::move(bitStream)), playerId(playerId) {}

        ~SendPacketInfo() noexcept = default;

    public:

        BitStreamPtr bitStream { nullptr };
        uint16_t playerId { NULL };

    };

    static std::shared_mutex rpcQueueMutex;
    static MPMCQueue<SendRpcInfo> rpcQueue;

    static std::shared_mutex packetQueueMutex;
    static MPMCQueue<SendPacketInfo> packetQueue;

    static std::shared_mutex kickQueueMutex;
    static MPMCQueue<uint16_t> kickQueue;

public:

    static bool         Start                (uint16_t          allowedPlayers,
                                              uint32_t          depreciated,
                                              int32_t           threadSleepTimer,
                                              uint16_t          port,
                                              const char*       host)             noexcept;

    static bool         Send                 (BitStream*        parameters,
                                              PacketPriority    priority,
                                              PacketReliability reliability,
                                              uint32_t          orderingChannel,
                                              PlayerID          playerId,
                                              bool              broadcast)        noexcept;

    static Packet*      Receive              (void)                               noexcept;

    static void         Kick                 (PlayerID          playerId)         noexcept;

    static void         DeallocatePacket     (Packet*           packet)           noexcept;

    static void         SetAllowedPlayers    (uint16_t          numberAllowed)    noexcept;

    static int          GetLastPing          (PlayerID          playerId)         noexcept;

    static void         RegisterRpc          (const uint8_t*    rpcIdPointer,
                                              RPCFunction       rpcHandler)       noexcept;

    static void         UnregisterRpc        (const uint8_t*    rpcIdPointer)     noexcept;

    static bool         Rpc                  (const uint8_t*    rpcIdPointer,
                                              BitStream*        parameters,
                                              PacketPriority    priority,
                                              PacketReliability reliability,
                                              uint32_t          orderingChannel,
                                              PlayerID          playerId,
                                              bool              broadcast,
                                              bool              shiftTimestamp)   noexcept;

    static const char*  GetLocalIp           (uint32_t          index)            noexcept;

    static PlayerID     GetInternalId        (void)                               noexcept;

    static int          GetIndexFromPlayerId (PlayerID          playerId)         noexcept;

    static PlayerID     GetPlayerIdFromIndex (int32_t           index)            noexcept;

    static void         AddToBanList         (const char*       ip,
                                              uint32_t          milliseconds)     noexcept;

    static void         RemoveFromBanList    (const char*       ip)               noexcept;

    static void         ClearBanList         (void)                               noexcept;

    static void         SetTimeoutTime       (RakNetTime        timeMs,
                                              PlayerID          target)           noexcept;

private:

    using StartFuncType                 = bool        (THISCALL*)(void*             _this,
                                                                  uint16_t          allowedPlayers,
                                                                  uint32_t          depreciated,
                                                                  int32_t           threadSleepTimer,
                                                                  uint16_t          port,
                                                                  const char*       host);

    using SendFuncType                  = bool        (THISCALL*)(void*             _this,
                                                                  BitStream*        parameters,
                                                                  PacketPriority    priority,
                                                                  PacketReliability reliability,
                                                                  uint32_t          orderingChannel,
                                                                  PlayerID          playerId,
                                                                  bool              broadcast);

    using ReceiveFuncType               = Packet*     (THISCALL*)(void*             _this);

    using KickFuncType                  = void        (THISCALL*)(void*             _this,
                                                                  PlayerID          playerId);

    using DeallocatePacketFuncType      = void        (THISCALL*)(void*             _this,
                                                                  Packet*           packet);

    using SetAllowedPlayersFuncType     = void        (THISCALL*)(void*             _this,
                                                                  uint16_t          numberAllowed);

    using GetLastPingFuncType           = int         (THISCALL*)(void*             _this,
                                                                  PlayerID          playerId);

    using RegisterAsRpcFuncType         = void        (THISCALL*)(void*             _this,
                                                                  const uint8_t*    rpcIdPointer,
                                                                  RPCFunction       rpcHandler);

    using UnregisterAsRpcFuncType       = void        (THISCALL*)(void*             _this,
                                                                  const uint8_t*    rpcIdPointer);

    using RpcFuncType                   = bool        (THISCALL*)(void*             _this,
                                                                  const uint8_t*    rpcIdPointer,
                                                                  BitStream*        parameters,
                                                                  PacketPriority    priority,
                                                                  PacketReliability reliability,
                                                                  uint32_t          orderingChannel,
                                                                  PlayerID          playerId,
                                                                  bool              broadcast,
                                                                  bool              shiftTimestamp);

    using GetLocalIpFuncType            = const char* (THISCALL*)(void*             _this,
                                                                  uint32_t          index);

    using GetInternalIdFuncType         = PlayerID    (THISCALL*)(void*             _this);

    using GetIndexFromPlayerIdFuncType  = int         (THISCALL*)(void*             _this,
                                                                  PlayerID          playerId);

    using GetPlayerIdFromIndexFuncType  = PlayerID    (THISCALL*)(void*             _this,
                                                                  int32_t           index);

    using AddToBanListFuncType          = void        (THISCALL*)(void*             _this,
                                                                  const char*       ip,
                                                                  uint32_t          milliseconds);

    using RemoveFromBanListFuncType     = void        (THISCALL*)(void*             _this,
                                                                  const char*       ip);

    using ClearBanListFuncType          = void        (THISCALL*)(void*             _this);

    using SetTimeoutTimeFuncType        = void        (THISCALL*)(void*             _this,
                                                                  RakNetTime        timeMs,
                                                                  PlayerID          target);

private:

    static StartFuncType                startFunc;
    static SendFuncType                 sendFunc;
    static ReceiveFuncType              receiveFunc;
    static KickFuncType                 kickFunc;
    static DeallocatePacketFuncType     deallocatePacketFunc;
    static SetAllowedPlayersFuncType    setAllowedPlayersFunc;
    static GetLastPingFuncType          getLastPingFunc;
    static RegisterAsRpcFuncType        registerAsRpcFunc;
    static UnregisterAsRpcFuncType      unregisterAsRpcFunc;
    static RpcFuncType                  rpcFunc;
    static GetLocalIpFuncType           getLocalIpFunc;
    static GetInternalIdFuncType        getInternalIdFunc;
    static GetIndexFromPlayerIdFuncType getIndexFromPlayerIdFunc;
    static GetPlayerIdFromIndexFuncType getPlayerIdFromIndexFunc;
    static AddToBanListFuncType         addToBanListFunc;
    static RemoveFromBanListFuncType    removeFromBanListFunc;
    static ClearBanListFuncType         clearBanListFunc;
    static SetTimeoutTimeFuncType       setTimeoutTimeFunc;

};
