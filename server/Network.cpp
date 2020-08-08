/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Network.h"

#include <random>

#include <assert.h>

#ifdef _WIN32

#pragma comment(lib, "Ws2_32.lib")

#define GetNetError() WSAGetLastError()

#else

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#define GetNetError() errno
#define closesocket(sock) close(sock)
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

#endif

#include <util/logger.h>
#include <util/memory.hpp>

bool Network::initStatus(false);
bool Network::bindStatus(false);

SOCKET Network::socketHandle(NULL);
uint16_t Network::serverPort(NULL);

std::array<std::atomic_bool, MAX_PLAYERS> Network::playerStatusTable = {};
std::array<std::shared_ptr<sockaddr_in>, MAX_PLAYERS> Network::playerAddrTable = {};
std::array<uint64_t, MAX_PLAYERS> Network::playerKeyTable = {};

std::shared_mutex Network::playerKeyToPlayerIdTableMutex;
std::map<uint64_t, uint16_t> Network::playerKeyToPlayerIdTable;

Network::ConnectHandlerType Network::connectHandler(nullptr);
Network::PlayerInitHandlerType Network::playerInitHandler(nullptr);
Network::DisconnectHandlerType Network::disconnectHandler(nullptr);

SPSCQueue<Network::ControlPacketInfo> Network::controlQueue(32 * MAX_PLAYERS);

static inline uint64_t MakeQword(uint32_t ldword, uint32_t rdword) {

	return static_cast<uint64_t>(ldword) << 32 | static_cast<uint64_t>(rdword);

}

static inline uint32_t MakeBytesFromBits(const uint32_t bits) {

	return (bits >> 3) + (bits & 7 ? 1 : 0);

}

bool Network::ConnectHandler(const uint16_t playerId, const RPCParameters* rpc) {

	if (!Network::initStatus) return true;

	const auto rpcParametersLength = MakeBytesFromBits(rpc->numberOfBitsOfData);
	const auto connectStruct = (SV::ConnectPacket*)(Memory::Scanner(rpc->input, rpcParametersLength).Find(SV::SignaturePattern, SV::SignatureMask));

	const uint8_t* connectStructEndPointer = ((uint8_t*)(connectStruct)) + sizeof(*connectStruct);
	const uint8_t* connectPacketEndPointer = rpc->input + rpcParametersLength;

	if (!connectStruct || connectStructEndPointer > connectPacketEndPointer) return true;

	const uint32_t playerAddr = RakNet::GetPlayerIdFromIndex(playerId).binaryAddress;
	if (!playerAddr || playerAddr == UNASSIGNED_PLAYER_ID.binaryAddress) return true;

	if (Network::playerStatusTable[playerId].exchange(false) && Network::disconnectHandler)
		Network::disconnectHandler(playerId);

	Logger::Log("[sv:dbg:network:connect] : connecting player (%hu) with address (%s) ...", playerId, inet_ntoa(*(in_addr*)(&playerAddr)));

	std::mt19937 genRandomNumber(clock());
	uint32_t randomNumber; uint64_t playerKey;

	do playerKey = MakeQword(playerAddr, randomNumber = genRandomNumber());
	while (!randomNumber || Network::playerKeyToPlayerIdTable.find(playerKey) !=
		Network::playerKeyToPlayerIdTable.end());

	Logger::Log("[sv:dbg:network:connect] : player (%hu) assigned key (%llx)", playerId, playerKey);

	std::atomic_store(&Network::playerAddrTable[playerId], std::shared_ptr<sockaddr_in>(nullptr));

	{
		const std::unique_lock<std::shared_mutex> lock(Network::playerKeyToPlayerIdTableMutex);

		Network::playerKeyToPlayerIdTable.erase(Network::playerKeyTable[playerId]);
		Network::playerKeyToPlayerIdTable[playerKey] = playerId;
	}

	Network::playerKeyTable[playerId] = playerKey;

	if (Network::connectHandler) Network::connectHandler(playerId, *connectStruct);

	Network::playerStatusTable[playerId].store(true);

	ControlPacket* controlPacket = nullptr;

	PackAlloca(controlPacket, SV::ControlPacketType::serverInfo, sizeof(SV::ServerInfoPacket));
	PackGetStruct(controlPacket, SV::ServerInfoPacket)->serverPort = Network::serverPort;
	PackGetStruct(controlPacket, SV::ServerInfoPacket)->serverKey = randomNumber;

	if (!Network::SendControlPacket(playerId, *controlPacket))
		Logger::Log("[sv:err:network:connect] : failed to send server info packet to player (%hu)", playerId);

	return true;

}

void Network::DisconnectHandler(const uint16_t playerId) {

	if (!Network::initStatus) return;

	if (!Network::playerStatusTable[playerId].exchange(false)) return;

	Logger::Log("[sv:dbg:network:connect] : disconnecting player (%hu) ...", playerId);

	std::atomic_store(&Network::playerAddrTable[playerId], std::shared_ptr<sockaddr_in>(nullptr));

	{
		const std::unique_lock<std::shared_mutex> lock(Network::playerKeyToPlayerIdTableMutex);

		Network::playerKeyToPlayerIdTable.erase(Network::playerKeyTable[playerId]);
	}

	Network::playerKeyTable[playerId] = NULL;

	if (Network::disconnectHandler) Network::disconnectHandler(playerId);

}

bool Network::PacketHandler(const uint16_t playerId, const Packet* packet) {

	if (!Network::initStatus) return true;

	if (packet->length < sizeof(uint8_t) + sizeof(ControlPacket)) return true;
	if (*packet->data != RaknetPacketId) return true;

	const auto controlPacketPtr = (ControlPacket*)(packet->data + sizeof(uint8_t));
	const auto controlPacketSize = packet->length - sizeof(uint8_t);

	if (controlPacketSize != controlPacketPtr->GetFullSize()) return false;

	const auto controlPacket = MakeControlPacketContainer(controlPacketPtr, controlPacketSize);

	Network::controlQueue.emplace(controlPacket, playerId);

	return false;

}

bool Network::Init(
	const void* serverBaseAddress,
	const ConnectHandlerType& connectHandler,
	const PlayerInitHandlerType& playerInitHandler,
	const DisconnectHandlerType& disconnectHandler
) {

	assert(serverBaseAddress);

	if (Network::initStatus) return false;

	Logger::Log("[sv:dbg:network:init] : module initializing...");

	if (!RakNet::Init(serverBaseAddress, Network::ConnectHandler, Network::DisconnectHandler, Network::PacketHandler)) {

		Logger::Log("[sv:err:network:init] : failed to init raknet");
		return false;

	}

	Network::connectHandler = connectHandler;
	Network::playerInitHandler = playerInitHandler;
	Network::disconnectHandler = disconnectHandler;

	Logger::Log("[sv:dbg:network:init] : module initialized");

	return Network::initStatus = true;

}

bool Network::Bind() {

	if (!Network::initStatus) return false;

	if (Network::bindStatus) return true;

	if (!RakNet::IsLoaded()) return false;

#ifdef _WIN32

	if (const auto error = WSAStartup(MAKEWORD(2, 2), &WSADATA())) {

		Logger::Log("[sv:err:network:bind] : wsastartup error (code:%d)", error);
		return false;

	}

#endif

	if ((Network::socketHandle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET) {

		Logger::Log("[sv:err:network:bind] : socket error (code:%d)", GetNetError());
#ifdef _WIN32
		WSACleanup();
#endif
		return false;

	}

	{
		const auto sendBufferSize = SendBufferSize;
		const auto recvBufferSize = RecvBufferSize;

		if (setsockopt(Network::socketHandle, SOL_SOCKET, SO_SNDBUF, (char*)(&sendBufferSize), sizeof(sendBufferSize)) == SOCKET_ERROR ||
			setsockopt(Network::socketHandle, SOL_SOCKET, SO_RCVBUF, (char*)(&recvBufferSize), sizeof(recvBufferSize)) == SOCKET_ERROR
		) {

			Logger::Log("[sv:err:network:bind] : setsockopt error (code:%d)", GetNetError());
			closesocket(Network::socketHandle);
#ifdef _WIN32
			WSACleanup();
#endif
			return false;

		}
	}

	{
		sockaddr_in bindAddr = {};

		bindAddr.sin_family = AF_INET;
		bindAddr.sin_addr.s_addr = RakNet::GetInternalId().binaryAddress;
		bindAddr.sin_port = NULL;

		if (bind(Network::socketHandle, (sockaddr*)(&bindAddr), sizeof(bindAddr)) == SOCKET_ERROR) {

			Logger::Log("[sv:err:network:bind] : bind error (code:%d)", GetNetError());
			closesocket(Network::socketHandle);
#ifdef _WIN32
			WSACleanup();
#endif
			return false;

		}
	}

	{
		sockaddr_in hostAddr = {};
		int addrlen = sizeof(hostAddr);

		if (getsockname(Network::socketHandle, (sockaddr*)(&hostAddr), &addrlen) == SOCKET_ERROR) {

			Logger::Log("[sv:err:network:bind] : getsockname error (code:%d)", GetNetError());
			closesocket(Network::socketHandle);
#ifdef _WIN32
			WSACleanup();
#endif
			return false;

		}

		Network::serverPort = ntohs(hostAddr.sin_port);
	}

	Logger::Log("[sv:dbg:network:bind] : voice server running on port %hu", Network::serverPort);

	return Network::bindStatus = true;

}

void Network::Process(const int64_t curTime) {

	static int64_t lastTime = NULL;

	if (!Network::initStatus) return;

	RakNet::Process();

	if (!Network::bindStatus) return;

	if (curTime - lastTime >= KeepAliveInterval) {

		VoicePacket keepAlivePacket;

		keepAlivePacket.packet = SV::VoicePacketType::keepAlive;
		keepAlivePacket.length = NULL;
		keepAlivePacket.packid = NULL;
		keepAlivePacket.sender = NULL;
		keepAlivePacket.stream = NULL;
		keepAlivePacket.svrkey = NULL;
		keepAlivePacket.CalcHash();

		const auto playerPoolSize = pNetGame->pPlayerPool->dwPlayerPoolSize;
		const auto connectedPlayers = pNetGame->pPlayerPool->dwConnectedPlayers;

		if (connectedPlayers) for (uint16_t iPlayerId = 0; iPlayerId <= playerPoolSize; ++iPlayerId) {

			if (!Network::playerStatusTable[iPlayerId].load()) continue;
			
			const auto playerAddr = std::atomic_load(&Network::playerAddrTable[iPlayerId]);
			if (!playerAddr) continue;

			sendto(Network::socketHandle, (char*)(&keepAlivePacket), sizeof(keepAlivePacket),
				NULL, (sockaddr*)(playerAddr.get()), sizeof(*playerAddr));

		}

		lastTime = curTime;

	}

}

bool Network::SendControlPacket(const uint16_t playerId, const ControlPacket& controlPacket) {

	if (!Network::initStatus) return false;

	RakNet::SendPacket(RaknetPacketId, playerId, &controlPacket, controlPacket.GetFullSize());

	return true;

}

bool Network::SendVoicePacket(const uint16_t playerId, const VoicePacket& voicePacket) {

	if (!Network::bindStatus) return false;

	if (!Network::playerStatusTable[playerId]) return false;

	const auto playerAddr = std::atomic_load(&Network::playerAddrTable[playerId]);
	if (!playerAddr) return false;

	return sendto(Network::socketHandle, (char*)(&voicePacket),
		voicePacket.GetFullSize(), NULL, (sockaddr*)(playerAddr.get()),
		sizeof(*playerAddr)) == voicePacket.GetFullSize();

}

ControlPacketContainerPtr Network::ReceiveControlPacket(uint16_t& sender) {

	if (!Network::initStatus) return nullptr;
	if (Network::controlQueue.empty()) return nullptr;

	const auto packetInfo = *Network::controlQueue.front();
	Network::controlQueue.pop();

	sender = packetInfo.sender;
	return packetInfo.packet;

}

VoicePacketContainerPtr Network::ReceiveVoicePacket() {

	if (!Network::bindStatus) return nullptr;

	sockaddr_in playerAddr = {};
	int addrLen = sizeof(playerAddr);
	char packetBuffer[MaxVoicePacketSize];

	const int length = recvfrom(Network::socketHandle, packetBuffer, sizeof(packetBuffer), NULL, (sockaddr*)(&playerAddr), &addrLen);
	if (length < (int)(sizeof(VoicePacket))) return nullptr;

	const auto voicePacketPtr = (VoicePacket*)(packetBuffer);
	if (!voicePacketPtr->CheckHeader()) return nullptr;

	const auto voicePacketSize = voicePacketPtr->GetFullSize();
	if (length != voicePacketSize) return nullptr;

	const auto playerKey = MakeQword(playerAddr.sin_addr.s_addr, voicePacketPtr->svrkey);

	uint16_t playerId = SV::NonePlayer;

	{
		const std::shared_lock<std::shared_mutex> lock(Network::playerKeyToPlayerIdTableMutex);

		const auto iter = Network::playerKeyToPlayerIdTable.find(playerKey);
		if (iter == Network::playerKeyToPlayerIdTable.end()) return nullptr;

		playerId = iter->second;
	}

	if (!Network::playerStatusTable[playerId].load()) return nullptr;

	if (!std::atomic_load(&Network::playerAddrTable[playerId])) {
		
		auto playerAddrPtr = std::make_shared<sockaddr_in>(playerAddr);
		if (!playerAddrPtr) return nullptr;

		std::shared_ptr<sockaddr_in> expAddrPtr;
		if (std::atomic_compare_exchange_strong(&Network::playerAddrTable[playerId], &expAddrPtr, playerAddrPtr)) {

			Logger::Log("[sv:dbg:network:receive] : player (%hu) identified (port:%hu)", playerId, ntohs(playerAddr.sin_port));

			ControlPacket* controlPacket = nullptr;

			PackAlloca(controlPacket, SV::ControlPacketType::pluginInit, sizeof(SV::PluginInitPacket));

			if (Network::playerInitHandler) {

				Network::playerInitHandler(playerId, *PackGetStruct(controlPacket, SV::PluginInitPacket));

			} else {

				PackGetStruct(controlPacket, SV::PluginInitPacket)->bitrate = SV::DefaultBitrate;
				PackGetStruct(controlPacket, SV::PluginInitPacket)->mute = false;

			}

			if (!Network::SendControlPacket(playerId, *controlPacket))
				Logger::Log("[sv:err:network:receive] : failed to send player (%hu) plugin init packet", playerId);

		}

	}

	if (voicePacketPtr->packet == SV::VoicePacketType::keepAlive) return nullptr;

	const auto voicePacket = MakeVoicePacketContainer(voicePacketPtr, voicePacketSize);
	const auto voicePacketRef = *voicePacket;

	voicePacketRef->sender = playerId;
	voicePacketRef->svrkey = NULL;

	return voicePacket;

}

void Network::Free() {

	if (!Network::initStatus) return;

	Logger::Log("[sv:dbg:network:free] : module releasing...");

	if (Network::bindStatus) {

		closesocket(Network::socketHandle);

		Network::socketHandle = NULL;
		Network::serverPort = NULL;

#ifdef _WIN32
		WSACleanup();
#endif

		{
			const std::unique_lock<std::shared_mutex> lock(Network::playerKeyToPlayerIdTableMutex);

			Network::playerKeyToPlayerIdTable.clear();
		}

		for (uint16_t iPlayerId = 0; iPlayerId < MAX_PLAYERS; ++iPlayerId) {

			Network::playerStatusTable[iPlayerId].store(false);
			std::atomic_store(&Network::playerAddrTable[iPlayerId],
				std::shared_ptr<sockaddr_in>(nullptr));
		}

		while (!Network::controlQueue.empty())
			Network::controlQueue.pop();

		Network::bindStatus = false;

	}

	RakNet::Free();

	Network::connectHandler = nullptr;
	Network::playerInitHandler = nullptr;
	Network::disconnectHandler = nullptr;

	Logger::Log("[sv:dbg:network:free] : module released");

	Network::initStatus = false;

}
