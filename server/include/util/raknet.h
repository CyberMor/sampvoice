/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <shared_mutex>
#include <memory>
#include <mutex>

#include <MPMCQueue.h>
#include <raknet/bitstream.h>
#include <raknet/networktypes.h>
#include <ysf/structs.h>

#include "memory.hpp"

class RakNet {
public:

	using ConnectHandlerType = std::function<bool(const uint16_t playerId, const RPCParameters* rpc)>;
	using PacketHandlerType = std::function<bool(const uint16_t playerId, const Packet* packet)>;
	using DisconnectHandlerType = std::function<void(const uint16_t playerId)>;

private:

	using StartFuncType = bool(THISCALL*)(void*, unsigned short, unsigned int, int, unsigned short, const char*);
	using SendFuncType = bool(THISCALL*)(void*, BitStream*, PacketPriority, PacketReliability, unsigned, PlayerID, bool);
	using ReceiveFuncType = Packet*(THISCALL*)(void*);
	using KickFuncType = void(THISCALL*)(void*, PlayerID);
	using DeallocatePacketFuncType = void(THISCALL*)(void*, Packet*);
	using SetAllowedPlayersFuncType = void(THISCALL*)(void*, unsigned short);
	using GetLastPingFuncType = int(THISCALL*)(void*, const PlayerID);
	using RegisterAsRpcFuncType = void(THISCALL*)(void*, uint8_t*, RPCFunction);
	using UnregisterAsRpcFuncType = void(THISCALL*)(void*, uint8_t*);
	using RpcFuncType = bool(THISCALL*)(void*, uint8_t*, BitStream*, PacketPriority, PacketReliability, unsigned, PlayerID, bool, bool);
	using GetLocalIpFuncType = const char*(THISCALL*)(void*, unsigned int);
	using GetInternalIdFuncType = PlayerID(THISCALL*)(void*);
	using GetIndexFromPlayerIdFuncType = int(THISCALL*)(void*, PlayerID);
	using GetPlayerIdFromIndexFuncType = PlayerID(THISCALL*)(void*, int);
	using AddToBanListFuncType = void(THISCALL*)(void*, const char*, unsigned int);
	using RemoveFromBanListFuncType = void(THISCALL*)(void*, const char*);
	using ClearBanListFuncType = void(THISCALL*)(void*);
	using SetTimeoutTimeFuncType = void(THISCALL*)(void*, RakNetTime, const PlayerID);

	static StartFuncType startFunc;
	static SendFuncType sendFunc;
	static ReceiveFuncType receiveFunc;
	static KickFuncType kickFunc;
	static DeallocatePacketFuncType deallocatePacketFunc;
	static SetAllowedPlayersFuncType setAllowedPlayersFunc;
	static GetLastPingFuncType getLastPingFunc;
	static RegisterAsRpcFuncType registerAsRpcFunc;
	static UnregisterAsRpcFuncType unregisterAsRpcFunc;
	static RpcFuncType rpcFunc;
	static GetLocalIpFuncType getLocalIpFunc;
	static GetInternalIdFuncType getInternalIdFunc;
	static GetIndexFromPlayerIdFuncType getIndexFromPlayerIdFunc;
	static GetPlayerIdFromIndexFuncType getPlayerIdFromIndexFunc;
	static AddToBanListFuncType addToBanListFunc;
	static RemoveFromBanListFuncType removeFromBanListFunc;
	static ClearBanListFuncType clearBanListFunc;
	static SetTimeoutTimeFuncType setTimeoutTimeFunc;

private:
	
	static bool initStatus;
	static bool loadStatus;

	static void* pRakServerInterface;

public:

	static bool Start(
		unsigned short allowedPlayers,
		unsigned int depreciated,
		int threadSleepTimer,
		unsigned short port,
		const char* forceHostAddress
	);

	static bool Send(
		BitStream* parameters,
		PacketPriority priority,
		PacketReliability reliability,
		unsigned orderingChannel,
		PlayerID playerId,
		bool broadcast
	);

	static Packet* Receive();
	static void Kick(PlayerID playerId);
	static void DeallocatePacket(Packet* packet);
	static void SetAllowedPlayers(unsigned short numberAllowed);
	static int GetLastPing(const PlayerID playerId);
	static void RegisterRpc(uint8_t* rpcIdPointer, RPCFunction rpcHandler);
	static void UnregisterRpc(uint8_t* rpcIdPointer);

	static bool Rpc(
		uint8_t* rpcIdPointer,
		BitStream* parameters,
		PacketPriority priority,
		PacketReliability reliability,
		unsigned orderingChannel,
		PlayerID playerId,
		bool broadcast,
		bool shiftTimestamp
	);

	static const char* GetLocalIp(unsigned int index);
	static PlayerID GetInternalId();
	static int GetIndexFromPlayerId(PlayerID playerId);
	static PlayerID GetPlayerIdFromIndex(int index);
	static void AddToBanList(const char* ip, unsigned int milliseconds);
	static void RemoveFromBanList(const char* ip);
	static void ClearBanList();
	static void SetTimeoutTime(RakNetTime timeMs, const PlayerID target);

private:

	static ConnectHandlerType connectHandler;
	static DisconnectHandlerType disconnectHandler;
	static PacketHandlerType packetHandler;

	static std::array<bool, MAX_PLAYERS> playerStatus;

	static RPCFunction origConnectHandler;

	static void ConnectHook(RPCParameters* parameters);

	static Memory::JumpHookPtr disconnectHook;

	static int THISCALL DisconnectHook(
		void* _this, int playerId, int reason
	);

	static Packet* THISCALL ReceiveHook(void* _this);

	static void THISCALL RegisterRpcHook(
		void* _this, uint8_t* rpcNumber,
		RPCFunction rpcHandler
	);

private:

	struct SendRpcInfo {
	private:

		using BitStreamPtr = std::shared_ptr<BitStream>;

	public:

		BitStreamPtr bitStream;
		uint16_t playerId;
		uint8_t rpcId;

		SendRpcInfo() :
			bitStream(nullptr),
			playerId(NULL),
			rpcId(NULL)
		{}

		SendRpcInfo(
			const BitStreamPtr& bitStream,
			const uint16_t playerId,
			const uint8_t rpcId
		) noexcept :
			bitStream(bitStream),
			playerId(playerId),
			rpcId(rpcId)
		{}

	};

	struct SendPacketInfo {
	private:

		using BitStreamPtr = std::shared_ptr<BitStream>;

	public:

		BitStreamPtr bitStream;
		uint16_t playerId;

		SendPacketInfo() :
			bitStream(nullptr),
			playerId(NULL)
		{}

		SendPacketInfo(
			const BitStreamPtr& bitStream,
			const uint16_t playerId
		) noexcept :
			bitStream(bitStream),
			playerId(playerId)
		{}

	};

	static std::shared_mutex rpcQueueMutex;
	static MPMCQueue<SendRpcInfo> rpcQueue;

	static std::shared_mutex packetQueueMutex;
	static MPMCQueue<SendPacketInfo> packetQueue;

	static std::shared_mutex kickQueueMutex;
	static MPMCQueue<uint16_t> kickQueue;

private:

	static Memory::JumpHookPtr hookGetRakServerInterface;

	static void* GetRakServerInterfaceHook();

public:

	static bool Init(
		const void* serverBaseAddr,
		const ConnectHandlerType& connectHandler,
		const DisconnectHandlerType& disconnectHandler,
		const PacketHandlerType& packetHandler
	);

	static bool IsLoaded();

	static void Process();

	static bool SendRPC(const uint8_t rpcId, const uint16_t playerId, const void* dataPtr, const int dataSize);
	static bool SendPacket(const uint8_t packetId, const uint16_t playerId, const void* dataPtr, const int dataSize);
	static bool KickPlayer(const uint16_t playerId);

	static void Free();

};
