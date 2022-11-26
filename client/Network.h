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

struct Network {

    Network() = delete;
    ~Network() = delete;
    Network(const Network&) = delete;
    Network(Network&&) = delete;
    Network& operator=(const Network&) = delete;
    Network& operator=(Network&&) = delete;

public:

    static constexpr BYTE          kRaknetPacketId     = 222;
    static constexpr int           kRaknetConnectRcpId = 25;
    static constexpr DWORD         kMaxVoicePacketSize = 1400;
    static constexpr DWORD         kMaxVoiceDataSize   = kMaxVoicePacketSize - sizeof(VoicePacket);
    static constexpr DWORD         kRecvBufferSize     = 1 * 1024 * 1024;
    static constexpr DWORD         kSendBufferSize     = 64 * 1024;
    static constexpr Timer::time_t kKeepAliveInterval  = 2000;

private:

    using ConnectCallback    = std::function<void(const std::string&, WORD)>;
    using SvConnectCallback  = std::function<void(SV::ConnectPacket&)>;
    using SvInitCallback     = std::function<bool(const SV::PluginInitPacket&)>;
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

    static bool Init(const AddressesBase& addr_base) noexcept;
    static void Free() noexcept;

    static bool SendControlPacket(WORD packet, LPCVOID data = nullptr, WORD size = 0) noexcept;
    static bool SendVoicePacket(LPCVOID data, WORD size) noexcept;
    static void EndSequence() noexcept;
    static bool ReceiveControlPacket(ControlPacketContainer& buffer) noexcept;
    static bool ReceiveVoicePacket(VoicePacketContainer& buffer) noexcept;

public:

    static void SetConnectCallback(ConnectCallback&& callback) noexcept;
    static void SetSvConnectCallback(SvConnectCallback&& callback) noexcept;
    static void SetSvInitCallback(SvInitCallback&& callback) noexcept;
    static void SetDisconnectCallback(DisconnectCallback&& callback) noexcept;

private:

    static void VoiceThread() noexcept;

    static void OnRaknetConnect(PCCH ip, WORD port) noexcept;
    static bool OnRaknetRpc(int id, BitStream& parameters) noexcept;
    static bool OnRaknetReceive(Packet& packet) noexcept;
    static void OnRaknetDisconnect() noexcept;

private:

    static bool _init_status;

    static SOCKET      _socket_handle;
    static int         _connect_status;
    static std::thread _voice_thread;
    static std::string _server_ip;
    static DWORD       _server_key;

    static ConnectCallback    _connect_callback;
    static SvConnectCallback  _sv_connect_callback;
    static SvInitCallback     _sv_init_callback;
    static DisconnectCallback _disconnect_callback;

    static SPSCQueue<ControlPacketContainer> _control_queue;
    static SPSCQueue<VoicePacketContainer>   _voice_queue;

    static VoicePacketContainer _input_voice_packet;
    static VoicePacketContainer _output_voice_packet;

};
