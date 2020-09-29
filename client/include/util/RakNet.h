/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cstdint>
#include <functional>

#include <raknet/bitstream.h>
#include <raknet/rakclient.h>

#include "Memory.hpp"
#include "AddressesBase.h"
#include "Logger.h"

class RakNet {

    using ConnectHandlerType = std::function<void(const char* ip, uint16_t host)>;
    using PacketIncomingHandlerType = std::function<bool(Packet* packet)>;
    using PacketOutcomingHandlerType = std::function<bool(BitStream* data)>;
    using RpcOutcomingHandlerType = std::function<bool(int rpc, BitStream* data)>;
    using DisconnectHandlerType = std::function<void()>;

public:

    static bool Init(
        const AddressesBase& addrBase,
        ConnectHandlerType&& connectHandler,
        PacketIncomingHandlerType&& packetIncomingHandler,
        PacketOutcomingHandlerType&& packetOutcomingHandler,
        RpcOutcomingHandlerType&& rpcOutcomingHandler,
        DisconnectHandlerType&& disconnectHandler
    ) noexcept;

    static bool IsInited() noexcept;
    static bool IsLoaded() noexcept;
    static bool IsConnected() noexcept;

    static bool Send(BitStream* bitStream) noexcept;

    static void Free() noexcept;

private:

    class RakClientHookInterface : public RakClientInterface {
    public:

        explicit RakClientHookInterface(RakClientInterface* pOrigInterface) noexcept;

        // RakClient Interface methods...
        // ------------------------------------------------

        bool RPC(
            int* rpcIdPointer,
            BitStream* parameters,
            PacketPriority priority,
            PacketReliability reliability,
            char orderingChannel,
            bool shiftTimestamp
        ) noexcept override;

        bool Send(
            BitStream* bitStream,
            PacketPriority priority,
            PacketReliability reliability,
            char orderingChannel
        ) noexcept override;

        Packet* Receive() noexcept override;

        bool Connect(
            const char* hostIp,
            uint16_t serverPort,
            uint16_t clientPort,
            uint32_t depreciated,
            int threadSleepTimer
        ) noexcept override;

        void Disconnect(uint32_t blockDuration, uint8_t orderingChannel) noexcept override;
        void InitializeSecurity(const char* privateKeyP, const char* privateKeyQ) noexcept override;
        void SetPassword(const char* password) noexcept override;
        bool HasPassword() noexcept override;

        bool Send(
            const char* dataPointer,
            int dataLength,
            PacketPriority priority,
            PacketReliability reliability,
            char orderingChannel
        ) noexcept override;

        void DeallocatePacket(Packet* packetPointer) noexcept override;
        void PingServer() noexcept override;

        void PingServer(
            const char* hostIp,
            uint16_t serverPort,
            uint16_t clientPort,
            bool onlyReplyOnAcceptingConnections
        ) noexcept override;

        int GetAveragePing() noexcept override;
        int GetLastPing() noexcept override;
        int GetLowestPing() noexcept override;
        int GetPlayerPing(PlayerID playerId) noexcept override;
        void StartOccasionalPing() noexcept override;
        void StopOccasionalPing() noexcept override;
        bool IsConnected() noexcept override;
        uint32_t GetSynchronizedRandomInteger() noexcept override;
        bool GenerateCompressionLayer(uint32_t inputFrequencyTable[256], bool inputLayer) noexcept override;
        bool DeleteCompressionLayer(bool inputLayer) noexcept override;
        void RegisterAsRemoteProcedureCall(int* rpcIdPointer, RPCFunction rpcHandler) noexcept override;
        void RegisterClassMemberRPC(int* rpcIdPointer, void* rpcHandler) noexcept override;
        void UnregisterAsRemoteProcedureCall(int* rpcIdPointer) noexcept override;

        bool RPC(
            int* rpcIdPointer,
            const char* dataPointer,
            uint32_t bitLength,
            PacketPriority priority,
            PacketReliability reliability,
            char orderingChannel,
            bool shiftTimestamp
        ) noexcept override;

        void SetTrackFrequencyTable(bool trackFrequencyTable) noexcept override;
        bool GetSendFrequencyTable(uint32_t outputFrequencyTable[256]) noexcept override;
        float GetCompressionRatio() noexcept override;
        float GetDecompressionRatio() noexcept override;
        void AttachPlugin(void* messageHandler) noexcept override;
        void DetachPlugin(void* messageHandler) noexcept override;
        BitStream* GetStaticServerData() noexcept override;
        void SetStaticServerData(const char* dataPointer, int dataLength) noexcept override;
        BitStream* GetStaticClientData(PlayerID playerId) noexcept override;
        void SetStaticClientData(PlayerID playerId, const char* dataPointer, int dataLength) noexcept override;
        void SendStaticClientDataToServer() noexcept override;
        PlayerID GetServerID() noexcept override;
        PlayerID GetPlayerID() noexcept override;
        PlayerID GetInternalID() noexcept override;
        const char* PlayerIDToDottedIP(PlayerID playerId) noexcept override;
        void PushBackPacket(Packet* packetPointer, bool pushAtHead) noexcept override;
        void SetRouterInterface(void* routerInterface) noexcept override;
        void RemoveRouterInterface(void* routerInterface) noexcept override;
        void SetTimeoutTime(RakNetTime timeMs) noexcept override;
        bool SetMTUSize(int mtuSize) noexcept override;
        int GetMTUSize() noexcept override;
        void AllowConnectionResponseIPMigration(bool allowConnectionResponseIpMigration) noexcept override;
        void AdvertiseSystem(const char* hostIp, uint16_t hostPort, const char* dataPointer, int dataLength) noexcept override;
        RakNetStatisticsStruct* GetStatistics() noexcept override;
        void ApplyNetworkSimulator(double maxSendBps, uint16_t minExtraPing, uint16_t extraPingVariance) noexcept override;
        bool IsNetworkSimulatorActive() noexcept override;
        PlayerIndex GetPlayerIndex() noexcept override;

        bool RPC_(
            int* rpcIdPointer,
            BitStream* bitStream,
            PacketPriority priority,
            PacketReliability reliability,
            char orderingChannel,
            bool shiftTimestamp,
            NetworkID networkId
        ) noexcept override;

    private:

        RakClientInterface* pOrigInterface { nullptr };

    };

private:

    static void SampDestructHookFunc() noexcept;
    static void RakClientInitHookFunc() noexcept;

private:

    static bool initStatus;
    static bool loadStatus;
    static bool connectStatus;

    static ConnectHandlerType connectHandler;
    static PacketIncomingHandlerType packetIncomingHandler;
    static PacketOutcomingHandlerType packetOutcomingHandler;
    static RpcOutcomingHandlerType rpcOutcomingHandler;
    static DisconnectHandlerType disconnectHandler;

    static RakClientInterface* pRakClientInterface;
    static RakClientInterface** ppRakClientInterface;

    static Memory::JumpHookPtr rakClientInitHook;
    static Memory::JumpHookPtr sampDestructHook;

};
