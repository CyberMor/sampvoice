#pragma once

#define RAKNET_MAX_PACKET	256

#include <stdint.h>
#include "bitstream.h"

class BitStream;

enum PacketPriority {
	SYSTEM_PRIORITY,
	HIGH_PRIORITY,
	MEDIUM_PRIORITY,
	LOW_PRIORITY,
	NUMBER_OF_PRIORITIES
};

enum PacketReliability {
	UNRELIABLE = 6,
	UNRELIABLE_SEQUENCED,
	RELIABLE,
	RELIABLE_ORDERED,
	RELIABLE_SEQUENCED
};

struct RPCParameters;

#define BITS_TO_BYTES(x) (((x)+7)>>3)
#define BYTES_TO_BITS(x) ((x)<<3)

typedef unsigned char MessageID;

typedef unsigned int RakNetTime;
typedef long long RakNetTimeNS;
typedef unsigned short PlayerIndex;
typedef unsigned char RPCIndex;
const int MAX_RPC_MAP_SIZE = ((RPCIndex)-1) - 1;

using RPCFunction = void(*)(RPCParameters *p);

#pragma pack(push, 1)

struct PlayerID {
	unsigned int binaryAddress;
	unsigned short port;
	PlayerID& operator = (const PlayerID& input) {
		binaryAddress = input.binaryAddress;
		port = input.port;
		return *this;
	}
};

const PlayerID UNASSIGNED_PLAYER_ID = { 0xFFFFFFFF, 0xFFFF };

struct Packet {
	PlayerIndex playerIndex;
	PlayerID playerId;
	unsigned int length;
	unsigned int bitSize;
	unsigned char* data;
	bool deleteData;
};

struct RPCParameters {
	unsigned char *input;
	unsigned int numberOfBitsOfData;
	PlayerID sender;
	void *recipient;
	BitStream *replyToSender;
};

struct NetworkID {
	static bool peerToPeerMode;
	PlayerID playerId;
	unsigned short localSystemId;
};

struct RPCNode {
	uint8_t uniqueIdentifier;
	void(*staticFunctionPointer) (RPCParameters *rpcParms);
};

struct RakNetStatisticsStruct {
	unsigned messageSendBuffer[NUMBER_OF_PRIORITIES];
	unsigned messagesSent[NUMBER_OF_PRIORITIES];
	unsigned messageDataBitsSent[NUMBER_OF_PRIORITIES];
	unsigned messageTotalBitsSent[NUMBER_OF_PRIORITIES];
	unsigned packetsContainingOnlyAcknowlegements;
	unsigned acknowlegementsSent;
	unsigned acknowlegementsPending;
	unsigned acknowlegementBitsSent;
	unsigned packetsContainingOnlyAcknowlegementsAndResends;
	unsigned messageResends;
	unsigned messageDataBitsResent;
	unsigned messagesTotalBitsResent;
	unsigned messagesOnResendQueue;
	unsigned numberOfUnsplitMessages;
	unsigned numberOfSplitMessages;
	unsigned totalSplits;
	unsigned packetsSent;
	unsigned encryptionBitsSent;
	unsigned totalBitsSent;
	unsigned sequencedMessagesOutOfOrder;
	unsigned sequencedMessagesInOrder;
	unsigned orderedMessagesOutOfOrder;
	unsigned orderedMessagesInOrder;
	unsigned packetsReceived;
	unsigned packetsWithBadCRCReceived;
	unsigned bitsReceived;
	unsigned bitsWithBadCRCReceived;
	unsigned acknowlegementsReceived;
	unsigned duplicateAcknowlegementsReceived;
	unsigned messagesReceived;
	unsigned invalidMessagesReceived;
	unsigned duplicateMessagesReceived;
	unsigned messagesWaitingForReassembly;
	unsigned internalOutputQueueSize;
	double bitsPerSecond;
	RakNetTime connectionStartTime;
};

class RakClientInterface {
public:
	virtual ~RakClientInterface() {}
	virtual bool Connect(const char* host, unsigned short serverPort, unsigned short clientPort, unsigned int depreciated, int threadSleepTimer) = 0;
	virtual void Disconnect(unsigned int blockDuration, unsigned char orderingChannel = 0) = 0;
	virtual void InitializeSecurity(const char *privKeyP, const char *privKeyQ) = 0;
	virtual void SetPassword(const char *_password) = 0;
	virtual bool HasPassword(void) = 0;
	virtual bool Send(const char *data, const int length, PacketPriority priority, PacketReliability reliability, char orderingChannel) = 0;
	virtual bool Send(BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel) = 0;
	virtual Packet* Receive(void) = 0;
	virtual void DeallocatePacket(Packet *packet) = 0;
	virtual void PingServer(void) = 0;
	virtual void PingServer(const char* host, unsigned short serverPort, unsigned short clientPort, bool onlyReplyOnAcceptingConnections) = 0;
	virtual int GetAveragePing(void) = 0;
	virtual int GetLastPing(void) = 0;
	virtual int GetLowestPing(void) = 0;
	virtual int GetPlayerPing(const PlayerID playerId) = 0;
	virtual void StartOccasionalPing(void) = 0;
	virtual void StopOccasionalPing(void) = 0;
	virtual bool IsConnected(void) = 0;
	virtual unsigned int GetSynchronizedRandomInteger(void) = 0;
	virtual bool GenerateCompressionLayer(unsigned int inputFrequencyTable[256], bool inputLayer) = 0;
	virtual bool DeleteCompressionLayer(bool inputLayer) = 0;
	virtual void RegisterAsRemoteProcedureCall(int* uniqueID, void(*functionPointer) (RPCParameters *rpcParms)) = 0;
	virtual void RegisterClassMemberRPC(int* uniqueID, void *functionPointer) = 0;
	virtual void UnregisterAsRemoteProcedureCall(int* uniqueID) = 0;
	virtual bool RPC(int* uniqueID, const char *data, unsigned int bitLength, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp) = 0;
	virtual bool RPC(int* uniqueID, BitStream *bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp) = 0;
	virtual bool RPC_(int* uniqueID, BitStream *bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp, NetworkID networkID) = 0;
	virtual void SetTrackFrequencyTable(bool b) = 0;
	virtual bool GetSendFrequencyTable(unsigned int outputFrequencyTable[256]) = 0;
	virtual float GetCompressionRatio(void) = 0;
	virtual float GetDecompressionRatio(void) = 0;
	virtual void AttachPlugin(void *messageHandler) = 0;
	virtual void DetachPlugin(void *messageHandler) = 0;
	virtual BitStream * GetStaticServerData(void) = 0;
	virtual void SetStaticServerData(const char *data, const int length) = 0;
	virtual BitStream * GetStaticClientData(const PlayerID playerId) = 0;
	virtual void SetStaticClientData(const PlayerID playerId, const char *data, const int length) = 0;
	virtual void SendStaticClientDataToServer(void) = 0;
	virtual PlayerID GetServerID(void) = 0;
	virtual PlayerID GetPlayerID(void) = 0;
	virtual PlayerID GetInternalID(void) = 0;
	virtual const char* PlayerIDToDottedIP(const PlayerID playerId) = 0;
	virtual void PushBackPacket(Packet *packet, bool pushAtHead) = 0;
	virtual void SetRouterInterface(void *routerInterface) = 0;
	virtual void RemoveRouterInterface(void *routerInterface) = 0;
	virtual void SetTimeoutTime(RakNetTime timeMS) = 0;
	virtual bool SetMTUSize(int size) = 0;
	virtual int GetMTUSize(void) = 0;
	virtual void AllowConnectionResponseIPMigration(bool allow) = 0;
	virtual void AdvertiseSystem(const char *host, unsigned short remotePort, const char *data, int dataLength) = 0;
	virtual RakNetStatisticsStruct * GetStatistics(void) = 0;
	virtual void ApplyNetworkSimulator(double maxSendBPS, unsigned short minExtraPing, unsigned short extraPingVariance) = 0;
	virtual bool IsNetworkSimulatorActive(void) = 0;
	virtual PlayerIndex GetPlayerIndex(void) = 0;
};

#pragma pack(pop)
