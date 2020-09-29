/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <array>
#include <atomic>
#include <cstdint>
#include <functional>
#include <shared_mutex>

#ifdef _WIN32
#include <WinSock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#ifndef _WIN32
#define SOCKET int
#endif

#include <SPSCQueue.h>
#include <util/raknet.h>

#include "ControlPacket.h"
#include "VoicePacket.h"
#include "Header.h"

class Network {

    using ConnectHandlerType = std::function<void(uint16_t, const SV::ConnectPacket&)>;
    using PlayerInitHandlerType = std::function<void(uint16_t, SV::PluginInitPacket&)>;
    using DisconnectHandlerType = std::function<void(uint16_t)>;

    static constexpr uint8_t RaknetPacketId = 222;
    static constexpr uint32_t MaxVoicePacketSize = 1400;
    static constexpr uint32_t MaxVoiceDataSize = MaxVoicePacketSize - sizeof(VoicePacket);
    static constexpr uint32_t SendBufferSize = 16 * 1024 * 1024;
    static constexpr uint32_t RecvBufferSize = 32 * 1024 * 1024;
    static constexpr int64_t KeepAliveInterval = 10000;

public:

    static bool Init(const void* serverBaseAddress,
                     ConnectHandlerType&& connectHandler,
                     PlayerInitHandlerType&& playerInitHandler,
                     DisconnectHandlerType&& disconnectHandler);
    static void Free();

    static bool Bind() noexcept;

    static void Process(int64_t curTime) noexcept;

    static bool SendControlPacket(uint16_t playerId, const ControlPacket& controlPacket);
    static bool SendVoicePacket(uint16_t playerId, const VoicePacket& voicePacket);
    static ControlPacketContainerPtr ReceiveControlPacket(uint16_t& sender) noexcept;
    static VoicePacketContainerPtr ReceiveVoicePacket();

private:

    static bool ConnectHandler(uint16_t playerId, RPCParameters* rpc);
    static bool PacketHandler(uint16_t playerId, Packet* packet);
    static void DisconnectHandler(uint16_t playerId);

private:

    static bool initStatus;
    static bool bindStatus;

    static SOCKET socketHandle;
    static uint16_t serverPort;

    static ConnectHandlerType connectHandler;
    static PlayerInitHandlerType playerInitHandler;
    static DisconnectHandlerType disconnectHandler;

    static std::array<std::atomic_bool, MAX_PLAYERS> playerStatusTable;
    static std::array<std::shared_ptr<sockaddr_in>, MAX_PLAYERS> playerAddrTable;
    static std::array<uint64_t, MAX_PLAYERS> playerKeyTable;

    static std::shared_mutex playerKeyToPlayerIdTableMutex;
    static std::map<uint64_t, uint16_t> playerKeyToPlayerIdTable;

private:

    struct ControlPacketInfo {

        ControlPacketInfo() noexcept = default;
        ControlPacketInfo(const ControlPacketInfo&) = default;
        ControlPacketInfo(ControlPacketInfo&&) noexcept = default;
        ControlPacketInfo& operator=(const ControlPacketInfo&) = default;
        ControlPacketInfo& operator=(ControlPacketInfo&&) noexcept = default;

    private:

        using PacketPtr = ControlPacketContainerPtr;

    public:

        explicit ControlPacketInfo(PacketPtr&& packet, uint16_t sender) noexcept
            : packet(std::move(packet)), sender(sender) {}

        ~ControlPacketInfo() noexcept = default;

    public:

        PacketPtr packet { nullptr };
        uint16_t sender { NULL };

    };

    static SPSCQueue<ControlPacketInfo> controlQueue;

};
