/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <thread>
#include <functional>
#include <string>

#include <WinSock2.h>

#include <SPSCQueue.h>
#include <raknet/bitstream.h>
#include <raknet/rakclient.h>
#include <util/AddressesBase.h>

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

    static constexpr BYTE RaknetPacketId = 222;
    static constexpr int RaknetConnectRcpId = 25;
    static constexpr DWORD MaxVoicePacketSize = 1400;
    static constexpr DWORD MaxVoiceDataSize = MaxVoicePacketSize - sizeof(VoicePacket);
    static constexpr DWORD RecvBufferSize = 2 * 1024 * 1024;
    static constexpr DWORD SendBufferSize = 64 * 1024;
    static constexpr int64_t KeepAliveInterval = 2000;

private:

    using ConnectHandlerType = std::function<void(const std::string&, WORD)>;
    using PluginConnectHandlerType = std::function<void(SV::ConnectPacket&)>;
    using PluginInitHandlerType = std::function<bool(const SV::PluginInitPacket&)>;
    using DisconnectHandlerType = std::function<void()>;

    struct ConnectionStatusType
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

    static bool Init(const AddressesBase& addrBase,
                     ConnectHandlerType connectHandler,
                     PluginConnectHandlerType pluginConnectHandler,
                     PluginInitHandlerType pluginInitHandler,
                     DisconnectHandlerType disconnectHandler);
    static void Free();

    static bool SendControlPacket(WORD packet, LPCVOID dataAddr = nullptr,
                                  WORD dataSize = NULL);

    static bool SendVoicePacket(LPCVOID dataAddr, WORD dataSize);
    static void EndSequence();

    static ControlPacketContainerPtr ReceiveControlPacket();
    static VoicePacketContainerPtr ReceiveVoicePacket();

private:

    static void VoiceThread();

private:

    static void OnRaknetConnect(PCCH serverIp, WORD serverPort);
    static bool OnRaknetSendRpc(int rpcId, BitStream* rpcParameters);
    static bool OnRaknetReceivePacket(Packet* packet);
    static void OnRaknetDisconnect() noexcept;

private:

    static bool initStatus;

    static SOCKET socketHandle;
    static int connectionStatus;

    static ConnectHandlerType connectHandler;
    static PluginConnectHandlerType pluginConnectHandler;
    static PluginInitHandlerType pluginInitHandler;
    static DisconnectHandlerType disconnectHandler;

    static SPSCQueue<ControlPacketContainerPtr> controlQueue;
    static SPSCQueue<VoicePacketContainerPtr> voiceQueue;

    static std::thread voiceThread;
    static std::string serverIp;

    static VoicePacketContainer inputVoicePacket;
    static VoicePacketContainer outputVoicePacket;

    static DWORD serverKey;
    static DWORD packetId;

};
