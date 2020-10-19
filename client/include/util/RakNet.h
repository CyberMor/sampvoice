/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cstdint>
#include <functional>
#include <vector>

#include <Windows.h>

#include <raknet/bitstream.h>
#include <raknet/rakclient.h>

#include "Memory.hpp"
#include "AddressesBase.h"

class RakNet {

    RakNet() = delete;
    ~RakNet() = delete;
    RakNet(const RakNet&) = delete;
    RakNet(RakNet&&) = delete;
    RakNet& operator=(const RakNet&) = delete;
    RakNet& operator=(RakNet&&) = delete;

private:

    using ConnectCallback = std::function<void(const char*, WORD)>;
    using ReceiveCallback = std::function<bool(Packet&)>;
    using SendCallback = std::function<bool(BitStream&)>;
    using RpcCallback = std::function<bool(int, BitStream&)>;
    using DisconnectCallback = std::function<void()>;

public:

    static bool Init(const AddressesBase& addrBase) noexcept;
    static bool IsInited() noexcept;
    static bool IsLoaded() noexcept;
    static void Free() noexcept;

    static bool IsConnected() noexcept;

    static bool Send(BitStream* bitStream) noexcept;

public:

    static std::size_t AddConnectCallback(ConnectCallback callback) noexcept;
    static std::size_t AddReceiveCallback(ReceiveCallback callback) noexcept;
    static std::size_t AddSendCallback(SendCallback callback) noexcept;
    static std::size_t AddRpcCallback(RpcCallback callback) noexcept;
    static std::size_t AddDisconnectCallback(DisconnectCallback callback) noexcept;

    static void RemoveConnectCallback(std::size_t callback) noexcept;
    static void RemoveReceiveCallback(std::size_t callback) noexcept;
    static void RemoveSendCallback(std::size_t callback) noexcept;
    static void RemoveRpcCallback(std::size_t callback) noexcept;
    static void RemoveDisconnectCallback(std::size_t callback) noexcept;

private:

    class RakClientHookInterface : public RakClientInterface {

        RakClientHookInterface() = delete;
        RakClientHookInterface(const RakClientHookInterface&) = delete;
        RakClientHookInterface(RakClientHookInterface&&) = delete;
        RakClientHookInterface& operator=(const RakClientHookInterface&) = delete;
        RakClientHookInterface& operator=(RakClientHookInterface&&) = delete;

    public:

        explicit RakClientHookInterface(RakClientInterface* pOrigInterface) noexcept;

        ~RakClientHookInterface() noexcept = default;

    public:

        bool RPC(int* rpcIdPointer, BitStream* parameters, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp) noexcept override;
        bool Send(BitStream* bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel) noexcept override;
        Packet* Receive() noexcept override;
        bool Connect(const char* hostIp, uint16_t serverPort, uint16_t clientPort, uint32_t depreciated, int threadSleepTimer) noexcept override;
        void Disconnect(uint32_t blockDuration, uint8_t orderingChannel) noexcept override;
        void InitializeSecurity(const char* privateKeyP, const char* privateKeyQ) noexcept override;
        void SetPassword(const char* password) noexcept override;
        bool HasPassword() noexcept override;
        bool Send(const char* dataPointer, int dataLength, PacketPriority priority, PacketReliability reliability, char orderingChannel) noexcept override;
        void DeallocatePacket(Packet* packetPointer) noexcept override;
        void PingServer() noexcept override;
        void PingServer(const char* hostIp, uint16_t serverPort, uint16_t clientPort, bool onlyReplyOnAcceptingConnections) noexcept override;
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
        bool RPC(int* rpcIdPointer, const char* dataPointer, uint32_t bitLength, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp) noexcept override;
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
        bool RPC_(int* rpcIdPointer, BitStream* bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp, NetworkID networkId) noexcept override;

    private:

        RakClientInterface* pOrigInterface { nullptr };

    };

private:

    static void HookSampDestruct() noexcept;
    static void HookRaknetInit() noexcept;

private:

    static bool initStatus;
    static bool loadStatus;

    static bool connectStatus;

    static std::vector<ConnectCallback> connectCallbacks;
    static std::vector<ReceiveCallback> receiveCallbacks;
    static std::vector<SendCallback> sendCallbacks;
    static std::vector<RpcCallback> rpcCallbacks;
    static std::vector<DisconnectCallback> disconnectCallbacks;

    static RakClientInterface* pRakClientInterface;
    static RakClientInterface** ppRakClientInterface;

    static Memory::JumpHookPtr rakClientInitHook;
    static Memory::JumpHookPtr sampDestructHook;

};
