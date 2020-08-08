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
#include <shared_mutex>
#include <functional>

#ifdef _WIN32

#include <WinSock2.h>

#else

#include <sys/socket.h>
#include <netinet/in.h>

#define SOCKET int

#endif

#include <SPSCQueue.h>
#include <util/raknet.h>

#include "ControlPacket.h"
#include "VoicePacket.h"
#include "Header.h"

class Network {
public:

	using ConnectHandlerType = std::function<void(const uint16_t playerId, const SV::ConnectPacket& packet)>;
	using PlayerInitHandlerType = std::function<void(const uint16_t playerId, SV::PluginInitPacket& packet)>;
	using DisconnectHandlerType = std::function<void(const uint16_t playerId)>;

	static constexpr uint8_t RaknetPacketId = 222;
	static constexpr uint32_t MaxVoicePacketSize = 1400;
	static constexpr uint32_t MaxVoiceDataSize = MaxVoicePacketSize - sizeof(VoicePacket);
	static constexpr uint32_t SendBufferSize = 16 * 1024 * 1024;
	static constexpr uint32_t RecvBufferSize = 32 * 1024 * 1024;
	static constexpr int64_t KeepAliveInterval = 10000;

private:

	static bool initStatus;
	static bool bindStatus;

	static SOCKET socketHandle;
	static uint16_t serverPort;

private:

	static std::array<std::atomic_bool, MAX_PLAYERS> playerStatusTable;
	static std::array<std::shared_ptr<sockaddr_in>, MAX_PLAYERS> playerAddrTable;
	static std::array<uint64_t, MAX_PLAYERS> playerKeyTable;

	static std::shared_mutex playerKeyToPlayerIdTableMutex;
	static std::map<uint64_t, uint16_t> playerKeyToPlayerIdTable;

private:

	struct ControlPacketInfo {
	private:

		using PacketPtr = ControlPacketContainerPtr;

	public:

		PacketPtr packet;
		uint16_t sender;

		ControlPacketInfo() :
			packet(nullptr),
			sender(NULL)
		{}

		ControlPacketInfo(
			const PacketPtr& packet,
			const uint16_t sender
		) noexcept :
			packet(packet),
			sender(sender)
		{}

	};

	static ConnectHandlerType connectHandler;
	static PlayerInitHandlerType playerInitHandler;
	static DisconnectHandlerType disconnectHandler;

	static SPSCQueue<ControlPacketInfo> controlQueue;

	static bool ConnectHandler(const uint16_t playerId, const RPCParameters* rpc);
	static bool PacketHandler(const uint16_t playerId, const Packet* packet);
	static void DisconnectHandler(const uint16_t playerId);

public:

	static bool Init(
		const void* serverBaseAddress,
		const ConnectHandlerType& connectHandler,
		const PlayerInitHandlerType& playerInitHandler,
		const DisconnectHandlerType& disconnectHandler
	);

	static bool Bind();

	static void Process(const int64_t curTime);

	static bool SendControlPacket(const uint16_t playerId, const ControlPacket& controlPacket);
	static bool SendVoicePacket(const uint16_t playerId, const VoicePacket& voicePacket);

	static ControlPacketContainerPtr ReceiveControlPacket(uint16_t& sender);
	static VoicePacketContainerPtr ReceiveVoicePacket();

	static void Free();

};
