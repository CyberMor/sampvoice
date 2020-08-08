/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "RakNet.h"

bool RakNet::initStatus(false);
bool RakNet::loadStatus(false);
bool RakNet::connectStatus(false);

RakNet::ConnectHandlerType RakNet::connectHandler(nullptr);
RakNet::PacketIncomingHandlerType RakNet::packetIncomingHandler(nullptr);
RakNet::PacketOutcomingHandlerType RakNet::packetOutcomingHandler(nullptr);
RakNet::RpcOutcomingHandlerType RakNet::rpcOutcomingHandler(nullptr);
RakNet::DisconnectHandlerType RakNet::disconnectHandler(nullptr);

RakClientInterface* RakNet::pRakClientInterface(nullptr);
RakClientInterface** RakNet::ppRakClientInterface(nullptr);

Memory::JumpHookPtr RakNet::rakClientInitHook(nullptr);
Memory::JumpHookPtr RakNet::sampDestructHook(nullptr);

RakNet::RakClientHookInterface::RakClientHookInterface(
	RakClientInterface* pOrigInterface
) :
	pOrigInterface(pOrigInterface)
{}

bool RakNet::RakClientHookInterface::RPC(
	int* rpcIdPointer,
	BitStream* parameters,
	PacketPriority priority,
	PacketReliability reliability,
	char orderingChannel,
	bool shiftTimestamp
) {

	if (RakNet::rpcOutcomingHandler && !RakNet::rpcOutcomingHandler(*rpcIdPointer, parameters))
		return true;

	return this->pOrigInterface->RPC(
		rpcIdPointer, parameters, priority,
		reliability, orderingChannel,
		shiftTimestamp
	);

}

bool RakNet::RakClientHookInterface::Send(
	BitStream* bitStream,
	PacketPriority priority,
	PacketReliability reliability,
	char orderingChannel
) {

	if (RakNet::packetOutcomingHandler && !RakNet::packetOutcomingHandler(bitStream))
		return true;

	return this->pOrigInterface->Send(bitStream, priority, reliability, orderingChannel);

}

Packet* RakNet::RakClientHookInterface::Receive() {

	Packet* packetPointer = nullptr;

	while ((packetPointer = this->pOrigInterface->Receive()) && RakNet::packetIncomingHandler &&
		!RakNet::packetIncomingHandler(packetPointer)) this->pOrigInterface->DeallocatePacket(packetPointer);

	return packetPointer;

}

bool RakNet::RakClientHookInterface::Connect(
	const char* hostIp,
	unsigned short serverPort,
	unsigned short clientPort,
	unsigned int depreciated,
	int threadSleepTimer
) {

	Logger::LogToFile(
		"[dbg:raknet:client_%p:connect] : connecting to game server '%s:%hu'...",
		this, hostIp, serverPort
	);

	RakNet::connectStatus = this->pOrigInterface->Connect(
		hostIp, serverPort, clientPort,
		depreciated, threadSleepTimer
	);

	if (RakNet::connectStatus) {

		Logger::LogToFile("[dbg:raknet:client_%p:connect] : connected", this);

		if (RakNet::connectHandler) RakNet::connectHandler(hostIp, serverPort);

	}

	return RakNet::connectStatus;

}

void RakNet::RakClientHookInterface::Disconnect(
	unsigned int blockDuration,
	unsigned char orderingChannel
) {

	Logger::LogToFile("[dbg:raknet:client_%p:disconnect] : disconnecting from server...", this);

	if (RakNet::connectStatus) {

		if (RakNet::disconnectHandler)
			RakNet::disconnectHandler();

		RakNet::connectStatus = false;

	}

	this->pOrigInterface->Disconnect(
		blockDuration, orderingChannel
	);

}

void RakNet::RakClientHookInterface::InitializeSecurity(
	const char* privateKeyP,
	const char* privateKeyQ
) {

	this->pOrigInterface->InitializeSecurity(
		privateKeyP, privateKeyQ
	);

}

void RakNet::RakClientHookInterface::SetPassword(const char* password) {

	this->pOrigInterface->SetPassword(password);

}

bool RakNet::RakClientHookInterface::HasPassword() {

	return this->pOrigInterface->HasPassword();

}

bool RakNet::RakClientHookInterface::Send(
	const char* dataPointer,
	const int dataLength,
	PacketPriority priority,
	PacketReliability reliability,
	char orderingChannel
) {

	BitStream bitStream((uint8_t*)(dataPointer), dataLength, true);

	if (RakNet::packetOutcomingHandler && !RakNet::packetOutcomingHandler(&bitStream))
		return true;

	return this->pOrigInterface->Send(
		&bitStream, priority,
		reliability, orderingChannel
	);

}

void RakNet::RakClientHookInterface::DeallocatePacket(Packet* packetPointer) {

	this->pOrigInterface->DeallocatePacket(packetPointer);

}

void RakNet::RakClientHookInterface::PingServer() {

	this->pOrigInterface->PingServer();

}

void RakNet::RakClientHookInterface::PingServer(
	const char* hostIp,
	unsigned short serverPort,
	unsigned short clientPort,
	bool onlyReplyOnAcceptingConnections
) {

	this->pOrigInterface->PingServer(
		hostIp, serverPort, clientPort,
		onlyReplyOnAcceptingConnections
	);

}

int RakNet::RakClientHookInterface::GetAveragePing() {

	return this->pOrigInterface->GetAveragePing();

}

int RakNet::RakClientHookInterface::GetLastPing() {

	return this->pOrigInterface->GetLastPing();

}

int RakNet::RakClientHookInterface::GetLowestPing() {

	return this->pOrigInterface->GetLowestPing();

}

int RakNet::RakClientHookInterface::GetPlayerPing(PlayerID playerId) {

	return this->pOrigInterface->GetPlayerPing(playerId);

}

void RakNet::RakClientHookInterface::StartOccasionalPing() {

	this->pOrigInterface->StartOccasionalPing();

}

void RakNet::RakClientHookInterface::StopOccasionalPing() {

	this->pOrigInterface->StopOccasionalPing();

}

bool RakNet::RakClientHookInterface::IsConnected() {

	return this->pOrigInterface->IsConnected();

}

unsigned int RakNet::RakClientHookInterface::GetSynchronizedRandomInteger() {

	return this->pOrigInterface->GetSynchronizedRandomInteger();

}

bool RakNet::RakClientHookInterface::GenerateCompressionLayer(
	unsigned int inputFrequencyTable[256], bool inputLayer
) {

	return this->pOrigInterface->GenerateCompressionLayer(
		inputFrequencyTable, inputLayer
	);

}

bool RakNet::RakClientHookInterface::DeleteCompressionLayer(bool inputLayer) {

	return this->pOrigInterface->DeleteCompressionLayer(inputLayer);

}

void RakNet::RakClientHookInterface::RegisterAsRemoteProcedureCall(
	int* rpcIdPointer, RPCFunction rpcHandler
) {

	this->pOrigInterface->RegisterAsRemoteProcedureCall(
		rpcIdPointer, rpcHandler
	);

}

void RakNet::RakClientHookInterface::RegisterClassMemberRPC(
	int* rpcIdPointer, void* rpcHandler
) {

	this->pOrigInterface->RegisterClassMemberRPC(rpcIdPointer, rpcHandler);

}

void RakNet::RakClientHookInterface::UnregisterAsRemoteProcedureCall(int* rpcIdPointer) {

	this->pOrigInterface->UnregisterAsRemoteProcedureCall(rpcIdPointer);

}

bool RakNet::RakClientHookInterface::RPC(
	int* rpcIdPointer,
	const char* dataPointer,
	unsigned int bitLength,
	PacketPriority priority,
	PacketReliability reliability,
	char orderingChannel,
	bool shiftTimestamp
) {

	const int byteLength = (bitLength >> 3) + ((bitLength & 7) ? 1 : 0);

	BitStream bitStream((uint8_t*)(dataPointer), byteLength, true);

	if (RakNet::rpcOutcomingHandler && !RakNet::rpcOutcomingHandler(*rpcIdPointer, &bitStream))
		return true;

	return this->pOrigInterface->RPC(
		rpcIdPointer, &bitStream,
		priority, reliability,
		orderingChannel,
		shiftTimestamp
	);

}

void RakNet::RakClientHookInterface::SetTrackFrequencyTable(bool trackFrequencyTable) {

	this->pOrigInterface->SetTrackFrequencyTable(trackFrequencyTable);

}

bool RakNet::RakClientHookInterface::GetSendFrequencyTable(
	unsigned int outputFrequencyTable[256]
) {

	return this->pOrigInterface->GetSendFrequencyTable(outputFrequencyTable);

}

float RakNet::RakClientHookInterface::GetCompressionRatio() {

	return this->pOrigInterface->GetCompressionRatio();

}

float RakNet::RakClientHookInterface::GetDecompressionRatio() {

	return this->pOrigInterface->GetDecompressionRatio();

}

void RakNet::RakClientHookInterface::AttachPlugin(void* messageHandler) {

	this->pOrigInterface->AttachPlugin(messageHandler);

}

void RakNet::RakClientHookInterface::DetachPlugin(void* messageHandler) {

	this->pOrigInterface->DetachPlugin(messageHandler);

}

BitStream* RakNet::RakClientHookInterface::GetStaticServerData() {

	return this->pOrigInterface->GetStaticServerData();

}

void RakNet::RakClientHookInterface::SetStaticServerData(
	const char* dataPointer, const int dataLength
) {

	this->pOrigInterface->SetStaticServerData(dataPointer, dataLength);

}

BitStream* RakNet::RakClientHookInterface::GetStaticClientData(PlayerID playerId) {

	return this->pOrigInterface->GetStaticClientData(playerId);

}

void RakNet::RakClientHookInterface::SetStaticClientData(
	const PlayerID playerId,
	const char* dataPointer,
	const int dataLength
) {

	this->pOrigInterface->SetStaticClientData(
		playerId, dataPointer, dataLength
	);

}

void RakNet::RakClientHookInterface::SendStaticClientDataToServer() {

	this->pOrigInterface->SendStaticClientDataToServer();

}

PlayerID RakNet::RakClientHookInterface::GetServerID() {

	return this->pOrigInterface->GetServerID();

}

PlayerID RakNet::RakClientHookInterface::GetPlayerID() {

	return this->pOrigInterface->GetPlayerID();

}

PlayerID RakNet::RakClientHookInterface::GetInternalID() {

	return this->pOrigInterface->GetInternalID();

}

const char* RakNet::RakClientHookInterface::PlayerIDToDottedIP(PlayerID playerId) {

	return this->pOrigInterface->PlayerIDToDottedIP(playerId);

}

void RakNet::RakClientHookInterface::PushBackPacket(
	Packet* packetPointer, bool pushAtHead
) {

	this->pOrigInterface->PushBackPacket(packetPointer, pushAtHead);

}

void RakNet::RakClientHookInterface::SetRouterInterface(void* routerInterface) {

	this->pOrigInterface->SetRouterInterface(routerInterface);

}

void RakNet::RakClientHookInterface::RemoveRouterInterface(void* routerInterface) {

	this->pOrigInterface->RemoveRouterInterface(routerInterface);

}

void RakNet::RakClientHookInterface::SetTimeoutTime(RakNetTime timeMs) {

	this->pOrigInterface->SetTimeoutTime(timeMs);

}

bool RakNet::RakClientHookInterface::SetMTUSize(int mtuSize) {

	return this->pOrigInterface->SetMTUSize(mtuSize);

}

int RakNet::RakClientHookInterface::GetMTUSize() {

	return this->pOrigInterface->GetMTUSize();

}

void RakNet::RakClientHookInterface::AllowConnectionResponseIPMigration(
	bool allowConnectionResponseIpMigration
) {

	this->pOrigInterface->AllowConnectionResponseIPMigration(
		allowConnectionResponseIpMigration
	);

}

void RakNet::RakClientHookInterface::AdvertiseSystem(
	const char* hostIp,
	unsigned short hostPort,
	const char* dataPointer,
	int dataLength
) {

	this->pOrigInterface->AdvertiseSystem(
		hostIp, hostPort, dataPointer,
		dataLength
	);

}

RakNetStatisticsStruct* RakNet::RakClientHookInterface::GetStatistics() {

	return this->pOrigInterface->GetStatistics();

}

void RakNet::RakClientHookInterface::ApplyNetworkSimulator(
	double maxSendBps,
	unsigned short minExtraPing,
	unsigned short extraPingVariance
) {

	this->pOrigInterface->ApplyNetworkSimulator(
		maxSendBps, minExtraPing,
		extraPingVariance
	);

}

bool RakNet::RakClientHookInterface::IsNetworkSimulatorActive() {

	return this->pOrigInterface->IsNetworkSimulatorActive();

}

PlayerIndex RakNet::RakClientHookInterface::GetPlayerIndex() {

	return this->pOrigInterface->GetPlayerIndex();

}

bool RakNet::RakClientHookInterface::RPC_(
	int* rpcIdPointer,
	BitStream* bitStream,
	PacketPriority priority,
	PacketReliability reliability,
	char orderingChannel,
	bool shiftTimestamp,
	NetworkID networkId
) {

	if (RakNet::rpcOutcomingHandler && !RakNet::rpcOutcomingHandler(*rpcIdPointer, bitStream))
		return true;

	return this->pOrigInterface->RPC_(
		rpcIdPointer, bitStream, priority,
		reliability, orderingChannel,
		shiftTimestamp, networkId
	);

}

void __declspec(naked) RakNet::SampDestructHookFunc() {

	static void* retAddr = nullptr;

	__asm {
		pushad
		mov ebp, esp
		sub esp, __LOCAL_SIZE
	}

	retAddr = RakNet::sampDestructHook->GetPatch().memAddr;
	RakNet::sampDestructHook.reset();

	RakNet::Free();

	__asm {
		mov esp, ebp
		popad
		jmp retAddr
	}

}

RakNet::RakClientHookInterface* RakNet::MakeHookInterface() {

	return new RakClientHookInterface(RakNet::pRakClientInterface);

}

static uint32_t rakClientOffset(0);

void __declspec(naked) RakNet::RakClientInitHookFunc() {

	static void* retAddr = nullptr;
	static RakClientInterface* retIface = nullptr;

	__asm {
		pushad
		mov ebp, esp
		sub esp, __LOCAL_SIZE
		mov pRakClientInterface, eax
	}

	Logger::LogToFile("[dbg:raknet:hookinit] : module loading...");

	retAddr = RakNet::rakClientInitHook->GetPatch().memAddr;
	RakNet::rakClientInitHook.reset();

	__asm {
		mov eax, rakClientOffset
		add eax, esi
		mov ppRakClientInterface, eax
	}

	if (retIface = RakNet::MakeHookInterface()) {

		Logger::LogToFile("[dbg:raknet:hookinit] : module loaded");

		RakNet::loadStatus = true;

	} else {

		Logger::LogToFile("[err:raknet:hookinit] : failed to load module");

		retIface = RakNet::pRakClientInterface;

		RakNet::Free();

	}

	__asm {
		mov esp, ebp
		popad
		mov eax, retIface
		jmp retAddr
	}

}

bool RakNet::Init(
	const AddressesBase& addrBase,
	const ConnectHandlerType& connectHandler,
	const PacketIncomingHandlerType& packetIncomingHandler,
	const PacketOutcomingHandlerType& packetOutcomingHandler,
	const RpcOutcomingHandlerType& rpcOutcomingHandler,
	const DisconnectHandlerType& disconnectHandler
) {

	if (RakNet::initStatus) return false;

	Logger::LogToFile("[dbg:raknet:init] : module initializing...");

	if (*(WORD*)(addrBase.rcInitAddr) == 0x4689) rakClientOffset = *(BYTE*)(addrBase.rcInitAddr + 2);
	else if (*(WORD*)(addrBase.rcInitAddr) == 0x8689) rakClientOffset = *(DWORD*)(addrBase.rcInitAddr + 2);

	Logger::LogToFile("[dbg:raknet:init] : finded rakclient interface offset (value:0x%x)", rakClientOffset);

	if (!(RakNet::sampDestructHook = MakeJumpHook(addrBase.sampDestructAddr, &RakNet::SampDestructHookFunc))) {

		Logger::LogToFile("[err:raknet:init] : failed to create 'SampDestruct' function hook");
		return false;

	}

	if (!(RakNet::rakClientInitHook = MakeJumpHook(addrBase.rcInitAddr, &RakNet::RakClientInitHookFunc))) {

		Logger::LogToFile("[err:raknet:init] : failed to create 'RakClientInit' function hook");
		RakNet::sampDestructHook.reset();
		return false;

	}

	RakNet::connectHandler = connectHandler;
	RakNet::packetIncomingHandler = packetIncomingHandler;
	RakNet::packetOutcomingHandler = packetOutcomingHandler;
	RakNet::rpcOutcomingHandler = rpcOutcomingHandler;
	RakNet::disconnectHandler = disconnectHandler;

	RakNet::connectStatus = false;
	RakNet::loadStatus = false;

	Logger::LogToFile("[dbg:raknet:init] : module initialized");

	return RakNet::initStatus = true;

}

bool RakNet::IsInited() {

	return RakNet::initStatus;

}

bool RakNet::IsLoaded() {

	return RakNet::loadStatus;

}

bool RakNet::IsConnected() {

	return RakNet::connectStatus;

}

bool RakNet::Send(BitStream* bitStream) {

	if (!bitStream || !RakNet::connectStatus)
		return false;

	return RakNet::pRakClientInterface->Send(
		bitStream, PacketPriority::MEDIUM_PRIORITY,
		PacketReliability::RELIABLE_ORDERED, '\0'
	);

}

void RakNet::Free() {

	if (!RakNet::initStatus) return;

	Logger::LogToFile("[dbg:raknet:free] : module releasing...");

	RakNet::sampDestructHook.reset();
	RakNet::rakClientInitHook.reset();

	if (RakNet::connectStatus) {

		if (RakNet::disconnectHandler)
			RakNet::disconnectHandler();

		RakNet::connectStatus = false;

	}

	RakNet::connectHandler = nullptr;
	RakNet::packetIncomingHandler = nullptr;
	RakNet::packetOutcomingHandler = nullptr;
	RakNet::rpcOutcomingHandler = nullptr;
	RakNet::disconnectHandler = nullptr;

	if (RakNet::loadStatus) {

		const auto pRakClientInterfaceHook = reinterpret_cast<RakClientHookInterface*>(*RakNet::ppRakClientInterface);
		*RakNet::ppRakClientInterface = RakNet::pRakClientInterface;
		delete pRakClientInterfaceHook;

		RakNet::loadStatus = false;

	}

	RakNet::ppRakClientInterface = nullptr;
	RakNet::pRakClientInterface = nullptr;

	Logger::LogToFile("[dbg:raknet:free] : module released");

	RakNet::initStatus = false;

}
