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
public:

	using ConnectHandlerType         = std::function<void(const char*, uint16_t)>;
	using PacketIncomingHandlerType  = std::function<bool(Packet*)>;
	using PacketOutcomingHandlerType = std::function<bool(BitStream*)>;
	using RpcOutcomingHandlerType    = std::function<bool(int, BitStream*)>;
	using DisconnectHandlerType      = std::function<void()>;

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

	class RakClientHookInterface : public RakClientInterface {
	private:

		RakClientInterface* const pOrigInterface;

	public:

		RakClientHookInterface(RakClientInterface* pOrigInterface);

		bool RPC(
			int* rpcIdPointer,
			BitStream* parameters,
			PacketPriority priority,
			PacketReliability reliability,
			char orderingChannel,
			bool shiftTimestamp
		);

		bool Send(
			BitStream* bitStream,
			PacketPriority priority,
			PacketReliability reliability,
			char orderingChannel
		);

		Packet* Receive();

		bool Connect(
			const char* hostIp,
			unsigned short serverPort,
			unsigned short clientPort,
			unsigned int depreciated,
			int threadSleepTimer
		);

		void Disconnect(
			unsigned int blockDuration,
			unsigned char orderingChannel
		);

		void InitializeSecurity(
			const char* privateKeyP,
			const char* privateKeyQ
		);

		void SetPassword(const char* password);

		bool HasPassword();

		bool Send(
			const char* dataPointer,
			const int dataLength,
			PacketPriority priority,
			PacketReliability reliability,
			char orderingChannel
		);

		void DeallocatePacket(Packet* packetPointer);

		void PingServer();

		void PingServer(
			const char* hostIp,
			unsigned short serverPort,
			unsigned short clientPort,
			bool onlyReplyOnAcceptingConnections
		);

		int GetAveragePing();

		int GetLastPing();

		int GetLowestPing();

		int GetPlayerPing(PlayerID playerId);

		void StartOccasionalPing();

		void StopOccasionalPing();

		bool IsConnected();

		unsigned int GetSynchronizedRandomInteger();

		bool GenerateCompressionLayer(unsigned int inputFrequencyTable[256], bool inputLayer);

		bool DeleteCompressionLayer(bool inputLayer);

		void RegisterAsRemoteProcedureCall(int* rpcIdPointer, RPCFunction rpcHandler);

		void RegisterClassMemberRPC(int* rpcIdPointer, void* rpcHandler);

		void UnregisterAsRemoteProcedureCall(int* rpcIdPointer);

		bool RPC(
			int* rpcIdPointer,
			const char* dataPointer,
			unsigned int bitLength,
			PacketPriority priority,
			PacketReliability reliability,
			char orderingChannel,
			bool shiftTimestamp
		);

		void SetTrackFrequencyTable(bool trackFrequencyTable);

		bool GetSendFrequencyTable(unsigned int outputFrequencyTable[256]);

		float GetCompressionRatio();
		float GetDecompressionRatio();

		void AttachPlugin(void* messageHandler);
		void DetachPlugin(void* messageHandler);

		BitStream* GetStaticServerData();

		void SetStaticServerData(
			const char* dataPointer,
			const int dataLength
		);

		BitStream* GetStaticClientData(PlayerID playerId);

		void SetStaticClientData(
			const PlayerID playerId,
			const char* dataPointer,
			const int dataLength
		);

		void SendStaticClientDataToServer();

		PlayerID GetServerID();

		PlayerID GetPlayerID();

		PlayerID GetInternalID();

		const char* PlayerIDToDottedIP(PlayerID playerId);

		void PushBackPacket(Packet* packetPointer, bool pushAtHead);

		void SetRouterInterface(void* routerInterface);

		void RemoveRouterInterface(void* routerInterface);

		void SetTimeoutTime(RakNetTime timeMs);

		bool SetMTUSize(int mtuSize);

		int GetMTUSize();

		void AllowConnectionResponseIPMigration(bool allowConnectionResponseIpMigration);

		void AdvertiseSystem(
			const char* hostIp,
			unsigned short hostPort,
			const char* dataPointer,
			int dataLength
		);

		RakNetStatisticsStruct* GetStatistics();

		void ApplyNetworkSimulator(
			double maxSendBps,
			unsigned short minExtraPing,
			unsigned short extraPingVariance
		);

		bool IsNetworkSimulatorActive();

		PlayerIndex GetPlayerIndex();

		bool RPC_(
			int* rpcIdPointer,
			BitStream* bitStream,
			PacketPriority priority,
			PacketReliability reliability,
			char orderingChannel,
			bool shiftTimestamp,
			NetworkID networkId
		);

	};

	static void SampDestructHookFunc();
	static RakClientHookInterface* MakeHookInterface();
	static void RakClientInitHookFunc();

public:

	static bool Init(
		const AddressesBase& addrBase,
		const ConnectHandlerType& connectHandler,
		const PacketIncomingHandlerType& packetIncomingHandler,
		const PacketOutcomingHandlerType& packetOutcomingHandler,
		const RpcOutcomingHandlerType& rpcOutcomingHandler,
		const DisconnectHandlerType& disconnectHandler
	);

	static bool IsInited();
	static bool IsLoaded();
	static bool IsConnected();

	static bool Send(BitStream* bitStream);

	static void Free();

};
