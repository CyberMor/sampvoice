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

struct RakNet {

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

    static bool Init(const AddressesBase& addr_base) noexcept;
    static bool IsInited() noexcept;
    static bool IsLoaded() noexcept;
    static void Free() noexcept;

    static bool IsConnected() noexcept;

    static bool Send(BitStream* bit_stream) noexcept;

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

    interface RakClientHookInterface : public RakClientInterface {

        RakClientHookInterface() = delete;
        ~RakClientHookInterface() noexcept = default;
        RakClientHookInterface(const RakClientHookInterface&) = delete;
        RakClientHookInterface(RakClientHookInterface&&) = delete;
        RakClientHookInterface& operator=(const RakClientHookInterface&) = delete;
        RakClientHookInterface& operator=(RakClientHookInterface&&) = delete;

    public:

        RakClientHookInterface(RakClientInterface* orig_interface) noexcept;

    public:

        bool RPC(int*, BitStream*, PacketPriority, PacketReliability, char, bool) noexcept override;
        bool Send(BitStream*, PacketPriority, PacketReliability, char) noexcept override;
        Packet* Receive() noexcept override;
        bool Connect(const char*, uint16_t, uint16_t, uint32_t, int) noexcept override;
        void Disconnect(uint32_t, uint8_t) noexcept override;
        void InitializeSecurity(const char*, const char*) noexcept override;
        void SetPassword(const char*) noexcept override;
        bool HasPassword() noexcept override;
        bool Send(const char*, int, PacketPriority, PacketReliability, char) noexcept override;
        void DeallocatePacket(Packet*) noexcept override;
        void PingServer() noexcept override;
        void PingServer(const char*, uint16_t, uint16_t, bool) noexcept override;
        int GetAveragePing() noexcept override;
        int GetLastPing() noexcept override;
        int GetLowestPing() noexcept override;
        int GetPlayerPing(PlayerID) noexcept override;
        void StartOccasionalPing() noexcept override;
        void StopOccasionalPing() noexcept override;
        bool IsConnected() noexcept override;
        uint32_t GetSynchronizedRandomInteger() noexcept override;
        bool GenerateCompressionLayer(uint32_t[256], bool) noexcept override;
        bool DeleteCompressionLayer(bool) noexcept override;
        void RegisterAsRemoteProcedureCall(int*, RPCFunction) noexcept override;
        void RegisterClassMemberRPC(int*, void*) noexcept override;
        void UnregisterAsRemoteProcedureCall(int*) noexcept override;
        bool RPC(int*, const char*, uint32_t, PacketPriority, PacketReliability, char, bool) noexcept override;
        void SetTrackFrequencyTable(bool) noexcept override;
        bool GetSendFrequencyTable(uint32_t[256]) noexcept override;
        float GetCompressionRatio() noexcept override;
        float GetDecompressionRatio() noexcept override;
        void AttachPlugin(void*) noexcept override;
        void DetachPlugin(void*) noexcept override;
        BitStream* GetStaticServerData() noexcept override;
        void SetStaticServerData(const char*, int) noexcept override;
        BitStream* GetStaticClientData(PlayerID) noexcept override;
        void SetStaticClientData(PlayerID, const char*, int) noexcept override;
        void SendStaticClientDataToServer() noexcept override;
        PlayerID GetServerID() noexcept override;
        PlayerID GetPlayerID() noexcept override;
        PlayerID GetInternalID() noexcept override;
        const char* PlayerIDToDottedIP(PlayerID) noexcept override;
        void PushBackPacket(Packet*, bool) noexcept override;
        void SetRouterInterface(void*) noexcept override;
        void RemoveRouterInterface(void*) noexcept override;
        void SetTimeoutTime(RakNetTime) noexcept override;
        bool SetMTUSize(int) noexcept override;
        int GetMTUSize() noexcept override;
        void AllowConnectionResponseIPMigration(bool) noexcept override;
        void AdvertiseSystem(const char*, uint16_t, const char*, int) noexcept override;
        RakNetStatisticsStruct* GetStatistics() noexcept override;
        void ApplyNetworkSimulator(double, uint16_t, uint16_t) noexcept override;
        bool IsNetworkSimulatorActive() noexcept override;
        PlayerIndex GetPlayerIndex() noexcept override;
        bool RPC_(int*, BitStream*, PacketPriority, PacketReliability, char, bool, NetworkID) noexcept override;

    private:

        RakClientInterface* _orig_interface = nullptr;

    };

private:

    static void HookSampDestruct() noexcept;
    static void HookRaknetInit() noexcept;

private:

    static bool _init_status;
    static bool _load_status;

    static bool _connect_status;

    static std::vector<ConnectCallback>    _connect_callbacks;
    static std::vector<ReceiveCallback>    _receive_callbacks;
    static std::vector<SendCallback>       _send_callbacks;
    static std::vector<RpcCallback>        _rpc_callbacks;
    static std::vector<DisconnectCallback> _disconnect_callbacks;

    static RakClientInterface*  _rak_client_interface;
    static RakClientInterface** _rak_client_interface_ptr;

    static Memory::JumpHook _rak_client_init_hook;
    static Memory::JumpHook _samp_destruct_hook;

};
