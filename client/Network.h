/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cstdint>
#include <functional>
#include <thread>
#include <string>
#include <vector>

#include <WinSock2.h>

#include <SPSCQueue.h>
#include <raknet/bitstream.h>
#include <raknet/rakclient.h>
#include <util/AddressesBase.h>
#include <util/Timer.h>

#include "ControlPacket.h"
#include "VoicePacket.h"
#include "Header.h"

class Network {

    Network() = delete;
    ~Network() = delete;
    Network(const Network&) = delete;
    Network(Network&&) = delete;
    Network& operator=(const Network&) = delete;
    Network& operator=(Network&&) = delete;

public:

    static constexpr BYTE kRaknetPacketId = 222;
    static constexpr int kRaknetConnectRcpId = 25;
    static constexpr DWORD kMaxVoicePacketSize = 1400;
    static constexpr DWORD kMaxVoiceDataSize = kMaxVoicePacketSize - sizeof(VoicePacket);
    static constexpr DWORD kRecvBufferSize = 2 * 1024 * 1024;
    static constexpr DWORD kSendBufferSize = 64 * 1024;
    static constexpr Timer::time_t kKeepAliveInterval = 2000;

private:

    using ConnectCallback = std::function<void(const std::string&, WORD)>;
    using SvConnectCallback = std::function<void(SV::ConnectPacket&)>;
    using SvInitCallback = std::function<bool(const SV::PluginInitPacket&)>;
    using DisconnectCallback = std::function<void()>;

private:

    struct ConnectionStatus
    {
        enum
        {
            Disconnected,
            RNConnecting,
            SVConnecting,
            Connected
        };
    };

public:

    static bool Init(const AddressesBase& addrBase) noexcept;
    static void Free() noexcept;

    static bool SendControlPacket(WORD packet, LPCVOID dataAddr = nullptr, WORD dataSize = 0) noexcept;
    static bool SendVoicePacket(LPCVOID dataAddr, WORD dataSize) noexcept;
    static void EndSequence() noexcept;
    static ControlPacketContainerPtr ReceiveControlPacket() noexcept;
    static VoicePacketContainerPtr ReceiveVoicePacket() noexcept;

    static std::size_t AddConnectCallback(ConnectCallback callback) noexcept;
    static std::size_t AddSvConnectCallback(SvConnectCallback callback) noexcept;
    static std::size_t AddSvInitCallback(SvInitCallback callback) noexcept;
    static std::size_t AddDisconnectCallback(DisconnectCallback callback) noexcept;
    static void RemoveConnectCallback(std::size_t callback) noexcept;
    static void RemoveSvConnectCallback(std::size_t callback) noexcept;
    static void RemoveSvInitCallback(std::size_t callback) noexcept;
    static void RemoveDisconnectCallback(std::size_t callback) noexcept;

private:

    static void VoiceThread() noexcept;

    static void OnRaknetConnect(PCCH ip, WORD port) noexcept;
    static bool OnRaknetRpc(int id, BitStream& parameters) noexcept;
    static bool OnRaknetReceive(Packet& packet) noexcept;
    static void OnRaknetDisconnect() noexcept;

private:

    static bool initStatus;

    static SOCKET socketHandle;
    static int connectionStatus;
    static std::thread voiceThread;
    static std::string serverIp;
    static DWORD serverKey;

    static std::vector<ConnectCallback> connectCallbacks;
    static std::vector<SvConnectCallback> svConnectCallbacks;
    static std::vector<SvInitCallback> svInitCallbacks;
    static std::vector<DisconnectCallback> disconnectCallbacks;

    static SPSCQueue<ControlPacketContainerPtr> controlQueue;
    static SPSCQueue<VoicePacketContainerPtr> voiceQueue;

    static VoicePacketContainer inputVoicePacket;
    static VoicePacketContainer outputVoicePacket;

};
