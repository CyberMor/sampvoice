/// \file
/// \brief Types used by RakNet, most of which involve user code.
///
/// This file is part of RakNet Copyright 2003 Kevin Jenkins.
///
/// Usage of RakNet is subject to the appropriate license agreement.
/// Creative Commons Licensees are subject to the
/// license found at
/// http://creativecommons.org/licenses/by-nc/2.5/
/// Single application licensees are subject to the license found at
/// http://www.rakkarsoft.com/SingleApplicationLicense.html
/// Custom license users are subject to the terms therein.
/// GPL license users are subject to the GNU General Public
/// License as published by the Free
/// Software Foundation; either version 2 of the License, or (at your
/// option) any later version.

#ifndef __NETWORK_TYPES_H
#define __NETWORK_TYPES_H

/// Forward declaration
class BitStream;

/// These enumerations are used to describe when packets are delivered.
enum PacketPriority
{
	SYSTEM_PRIORITY,   /// \internal Used by RakNet to send above-high priority messages.
	HIGH_PRIORITY,   /// High priority messages are send before medium priority messages.
	MEDIUM_PRIORITY,   /// Medium priority messages are send before low priority messages.
	LOW_PRIORITY,   /// Low priority messages are only sent when no other messages are waiting.
	NUMBER_OF_PRIORITIES
};

/// These enumerations are used to describe how packets are delivered.
/// \note  Note to self: I write this with 3 bits in the stream.  If I add more remember to change that
enum PacketReliability
{
	UNRELIABLE = 6,   /// Same as regular UDP, except that it will also discard duplicate datagrams.  RakNet adds (6 to 17) + 21 bits of overhead, 16 of which is used to detect duplicate packets and 6 to 17 of which is used for message length.
	UNRELIABLE_SEQUENCED,  /// Regular UDP with a sequence counter.  Out of order messages will be discarded.  This adds an additional 13 bits on top what is used for UNRELIABLE.
	RELIABLE,   /// The message is sent reliably, but not necessarily in any order.  Same overhead as UNRELIABLE.
	RELIABLE_ORDERED,   /// This message is reliable and will arrive in the order you sent it.  Messages will be delayed while waiting for out of order messages.  Same overhead as UNRELIABLE_SEQUENCED.
	RELIABLE_SEQUENCED /// This message is reliable and will arrive in the sequence you sent it.  Out or order messages will be dropped.  Same overhead as UNRELIABLE_SEQUENCED.
};

struct RPCParameters;

#define BITS_TO_BYTES(x) (((x)+7)>>3)
#define BYTES_TO_BITS(x) ((x)<<3)

/// First byte of a network message
typedef unsigned char MessageID;

typedef unsigned int RakNetTime;
typedef long long RakNetTimeNS;
typedef unsigned short PlayerIndex;
typedef unsigned char RPCIndex;
const int MAX_RPC_MAP_SIZE = ((RPCIndex)-1) - 1;

using RPCFunction = void(*)(RPCParameters *p);

struct PlayerID
{
	unsigned int binaryAddress;
	unsigned short port;
};

const PlayerID UNASSIGNED_PLAYER_ID =
{
	0xFFFFFFFF, 0xFFFF
};

struct Packet
{
	PlayerIndex playerIndex;
	PlayerID playerId;
	unsigned int length;
	unsigned int bitSize;
	unsigned char* data;
	bool deleteData;
};

struct RPCParameters
{
	unsigned char *input;
	unsigned int numberOfBitsOfData;
	PlayerID sender;
};

class PluginInterface;
class RouterInterface;

struct NetworkID
{
	static bool peerToPeerMode;
	PlayerID playerId;
	unsigned short localSystemId;
};

struct RakNetStatisticsStruct
{
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

	RakNetStatisticsStruct operator +=(const RakNetStatisticsStruct& other)
	{
		unsigned i;
		for (i = 0; i < NUMBER_OF_PRIORITIES; i++)
		{
			messageSendBuffer[i] += other.messageSendBuffer[i];
			messagesSent[i] += other.messagesSent[i];
			messageDataBitsSent[i] += other.messageDataBitsSent[i];
			messageTotalBitsSent[i] += other.messageTotalBitsSent[i];
		}

		packetsContainingOnlyAcknowlegements += other.packetsContainingOnlyAcknowlegements;
		acknowlegementsSent += other.packetsContainingOnlyAcknowlegements;
		acknowlegementsPending += other.acknowlegementsPending;
		acknowlegementBitsSent += other.acknowlegementBitsSent;
		packetsContainingOnlyAcknowlegementsAndResends += other.packetsContainingOnlyAcknowlegementsAndResends;
		messageResends += other.messageResends;
		messageDataBitsResent += other.messageDataBitsResent;
		messagesTotalBitsResent += other.messagesTotalBitsResent;
		messagesOnResendQueue += other.messagesOnResendQueue;
		numberOfUnsplitMessages += other.numberOfUnsplitMessages;
		numberOfSplitMessages += other.numberOfSplitMessages;
		totalSplits += other.totalSplits;
		packetsSent += other.packetsSent;
		encryptionBitsSent += other.encryptionBitsSent;
		totalBitsSent += other.totalBitsSent;
		sequencedMessagesOutOfOrder += other.sequencedMessagesOutOfOrder;
		sequencedMessagesInOrder += other.sequencedMessagesInOrder;
		orderedMessagesOutOfOrder += other.orderedMessagesOutOfOrder;
		orderedMessagesInOrder += other.orderedMessagesInOrder;
		packetsReceived += other.packetsReceived;
		packetsWithBadCRCReceived += other.packetsWithBadCRCReceived;
		bitsReceived += other.bitsReceived;
		bitsWithBadCRCReceived += other.bitsWithBadCRCReceived;
		acknowlegementsReceived += other.acknowlegementsReceived;
		duplicateAcknowlegementsReceived += other.duplicateAcknowlegementsReceived;
		messagesReceived += other.messagesReceived;
		invalidMessagesReceived += other.invalidMessagesReceived;
		duplicateMessagesReceived += other.duplicateMessagesReceived;
		messagesWaitingForReassembly += other.messagesWaitingForReassembly;
		internalOutputQueueSize += other.internalOutputQueueSize;

		return *this;
	}
};

class RakServerInterface {
public:
	virtual ~RakServerInterface() {}
	virtual bool Start(unsigned short AllowedPlayers, unsigned int depreciated, int threadSleepTimer, unsigned short port, const char *forceHostAddress = 0) = 0;
	virtual void InitializeSecurity(const char *privateKeyE, const char *privateKeyN) = 0;
	virtual void DisableSecurity(void) = 0;
	virtual void SetPassword(const char *_password) = 0;
	virtual bool HasPassword(void) = 0;
	virtual void Disconnect(unsigned int blockDuration, unsigned char orderingChannel = 0) = 0;
	virtual bool Send(const char *data, const int length, PacketPriority priority, PacketReliability reliability, char orderingChannel, PlayerID playerId, bool broadcast) = 0;
	virtual bool Send(BitStream *bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, PlayerID playerId, bool broadcast) = 0;
	virtual Packet* Receive(void) = 0;
	virtual void Kick(const PlayerID playerId) = 0;
	virtual void DeallocatePacket(Packet *packet) = 0;
	virtual void SetAllowedPlayers(unsigned short AllowedPlayers) = 0;
	virtual unsigned short GetAllowedPlayers(void) const = 0;
	virtual unsigned short GetConnectedPlayers(void) = 0;
	virtual void GetPlayerIPFromID(const PlayerID playerId, char returnValue[22], unsigned short *port) = 0;
	virtual void PingPlayer(const PlayerID playerId) = 0;
	virtual int GetAveragePing(const PlayerID playerId) = 0;
	virtual int GetLastPing(const PlayerID playerId) = 0;
	virtual int GetLowestPing(const PlayerID playerId) = 0;
	virtual void StartOccasionalPing(void) = 0;
	virtual void StopOccasionalPing(void) = 0;
	virtual bool IsActive(void) const = 0;
	virtual unsigned int GetSynchronizedRandomInteger(void) const = 0;
	virtual void StartSynchronizedRandomInteger(void) = 0;
	virtual void StopSynchronizedRandomInteger(void) = 0;
	virtual bool GenerateCompressionLayer(unsigned int inputFrequencyTable[256], bool inputLayer) = 0;
	virtual bool DeleteCompressionLayer(bool inputLayer) = 0;
	virtual void RegisterAsRemoteProcedureCall(char* uniqueID, void(*functionPointer) (RPCParameters *rpcParms)) = 0;
	virtual void RegisterClassMemberRPC(char* uniqueID, void *functionPointer) = 0;
	virtual void UnregisterAsRemoteProcedureCall(char* uniqueID) = 0;
	virtual bool RPC(char* uniqueID, const char *data, unsigned int bitLength, PacketPriority priority, PacketReliability reliability, char orderingChannel, PlayerID playerId, bool broadcast, bool shiftTimestamp, NetworkID networkID, BitStream *replyFromTarget) = 0;
	virtual bool RPC(char* uniqueID, BitStream *bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, PlayerID playerId, bool broadcast, bool shiftTimestamp, NetworkID networkID, BitStream *replyFromTarget) = 0;
	virtual bool RPC(char* uniqueID, BitStream *bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, PlayerID playerId, bool broadcast, bool shiftTimestamp) = 0;
	virtual void SetTrackFrequencyTable(bool b) = 0;
	virtual bool GetSendFrequencyTable(unsigned int outputFrequencyTable[256]) = 0;
	virtual float GetCompressionRatio(void) const = 0;
	virtual float GetDecompressionRatio(void) const = 0;
	virtual void AttachPlugin(PluginInterface *messageHandler) = 0;
	virtual void DetachPlugin(PluginInterface *messageHandler) = 0;
	virtual BitStream * GetStaticServerData(void) = 0;
	virtual void SetStaticServerData(const char *data, const int length) = 0;
	virtual void SetRelayStaticClientData(bool b) = 0;
	virtual void SendStaticServerDataToClient(const PlayerID playerId) = 0;
	virtual void SetOfflinePingResponse(const char *data, const unsigned int length) = 0;
	virtual BitStream * GetStaticClientData(const PlayerID playerId) = 0;
	virtual void SetStaticClientData(const PlayerID playerId, const char *data, const int length) = 0;
	virtual void ChangeStaticClientData(const PlayerID playerChangedId, PlayerID playerToSendToId) = 0;
	virtual unsigned int GetNumberOfAddresses(void) = 0;
	virtual const char* GetLocalIP(unsigned int index) = 0;
	virtual PlayerID GetInternalID(void) const = 0;
	virtual void PushBackPacket(Packet *packet, bool pushAtHead) = 0;
	virtual void SetRouterInterface(RouterInterface *routerInterface) = 0;
	virtual void RemoveRouterInterface(RouterInterface *routerInterface) = 0;
	virtual int GetIndexFromPlayerID(const PlayerID playerId) = 0;
	virtual PlayerID GetPlayerIDFromIndex(int index) = 0;
	virtual void AddToBanList(const char *IP) = 0;
	virtual void RemoveFromBanList(const char *IP) = 0;
	virtual void ClearBanList(void) = 0;
	virtual bool IsBanned(const char *IP) = 0;
	virtual bool IsActivePlayerID(const PlayerID playerId) = 0;
	virtual void SetTimeoutTime(RakNetTime timeMS, const PlayerID target) = 0;
	virtual bool SetMTUSize(int size) = 0;
	virtual int GetMTUSize(void) const = 0;
	virtual void AdvertiseSystem(const char *host, unsigned short remotePort, const char *data, int dataLength) = 0;
	virtual RakNetStatisticsStruct * const GetStatistics(const PlayerID playerId) = 0;
	virtual void ApplyNetworkSimulator(double maxSendBPS, unsigned short minExtraPing, unsigned short extraPingVariance) = 0;
	virtual bool IsNetworkSimulatorActive(void) = 0;
};

#endif