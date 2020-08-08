/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Network.h"

#pragma comment(lib, "Ws2_32.lib")

bool Network::initStatus(false);
std::atomic_int Network::connectionStatus(ConnectionStatusType::Disconnected);

Network::ConnectHandlerType Network::connectHandler(nullptr);
Network::PluginConnectHandlerType Network::pluginConnectHandler(nullptr);
Network::PluginInitHandlerType Network::pluginInitHandler(nullptr);
Network::DisconnectHandlerType Network::disconnectHandler(nullptr);

SOCKET Network::socketHandle(NULL);

SPSCQueue<ControlPacketContainerPtr> Network::controlQueue(128);
SPSCQueue<VoicePacketContainerPtr> Network::voiceQueue(512);

std::thread Network::voiceThread;
std::string Network::serverIp;

VoicePacketContainer Network::inputVoicePacket(MaxVoicePacketSize);
VoicePacketContainer Network::outputVoicePacket(MaxVoicePacketSize);

DWORD Network::serverKey(NULL);
DWORD Network::packetId(NULL);

void Network::VoiceThread() {

	VoicePacket keepAlivePacket;
	int64_t keepAliveLastTime = NULL;

	ZeroMemory(&keepAlivePacket, sizeof(keepAlivePacket));

	keepAlivePacket.svrkey = Network::serverKey;
	keepAlivePacket.packet = SV::VoicePacketType::keepAlive;
	keepAlivePacket.packid = NULL;
	keepAlivePacket.length = NULL;
	keepAlivePacket.sender = NULL;
	keepAlivePacket.stream = NULL;
	keepAlivePacket.CalcHash();

	while (true) {

		const auto curStatus = Network::connectionStatus.load();

		if (curStatus == ConnectionStatusType::Disconnected) break;

		// Sending keep-alive packets...
		// -----------------------------------------------------------------

		const int64_t curTime = GetTimestamp();

		if (curTime - keepAliveLastTime >= KeepAliveInterval) {

			send(Network::socketHandle, (char*)(&keepAlivePacket), sizeof(keepAlivePacket), NULL);

			keepAliveLastTime = curTime;

		}

		// Receiving voice packets...
		// -----------------------------------------------------------------

		if (curStatus != ConnectionStatusType::Connected) {

			SleepForMilliseconds(100);
			continue;

		}

		const auto receivedDataSize = recv(Network::socketHandle, (char*)(Network::inputVoicePacket.GetData()), Network::inputVoicePacket.GetSize(), NULL);

		if (receivedDataSize == SOCKET_ERROR) {

			if (WSAGetLastError() == WSAEWOULDBLOCK)
				SleepForMilliseconds(100);

			continue;

		}

		if (receivedDataSize < (int)(sizeof(VoicePacket))) continue;

		if (!Network::inputVoicePacket->CheckHeader()) continue;
		if (receivedDataSize != Network::inputVoicePacket->GetFullSize()) continue;
		if (Network::inputVoicePacket->packet == SV::VoicePacketType::keepAlive) continue;
		if (!GameUtil::IsGameActive()) continue;

		if (const auto voicePacket = MakeVoicePacketContainer(Network::inputVoicePacket))
			Network::voiceQueue.push(voicePacket);

	}

}

void Network::OnRaknetConnect(const char* serverIp, WORD serverPort) {

	if (!Network::initStatus) return;

	Network::serverIp = serverIp;

	if (!Network::connectHandler) return;

	Network::connectHandler(serverIp, serverPort);

}

bool Network::OnRaknetSendRpc(const int rpcId, BitStream* rpcParameters) {

	if (!Network::initStatus) return true;

	if (rpcId != RaknetConnectRcpId) return true;
	if (!Network::pluginConnectHandler) return true;

	SV::ConnectPacket stData = {};

	Network::pluginConnectHandler(stData);

	rpcParameters->Write(stData);

	Logger::LogToFile("[sv:dbg:network:connect] : raknet connecting... (version:%hhu;micro:%hhu)", stData.version, stData.micro);

	Network::connectionStatus.store(ConnectionStatusType::RNConnecting);

	return true;

}

bool Network::OnRaknetReceivePacket(Packet* packet) {

	if (!Network::initStatus) return true;

	if (packet->length < sizeof(BYTE) + sizeof(ControlPacket)) return true;
	if (*packet->data != RaknetPacketId) return true;

	const auto controlPacketPtr = (ControlPacket*)(packet->data + sizeof(BYTE));
	const DWORD controlPacketSize = packet->length - sizeof(BYTE);

	if (controlPacketSize != controlPacketPtr->GetFullSize()) return false;

	switch (controlPacketPtr->packet) {
		case SV::ControlPacketType::serverInfo:
		{

			const auto stData = (SV::ServerInfoPacket*)(controlPacketPtr->data);
			if (controlPacketPtr->length != sizeof(*stData)) return false;

			Logger::LogToFile(
				"[sv:dbg:network:serverInfo] : connecting to voiceserver '%s:%hu'...",
				Network::serverIp.c_str(), stData->serverPort
			);

			sockaddr_in serverAddress = {};

			serverAddress.sin_family = AF_INET;
			serverAddress.sin_addr.s_addr = inet_addr(Network::serverIp.c_str());
			serverAddress.sin_port = htons(stData->serverPort);

			if (connect(Network::socketHandle, (sockaddr*)(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR) {

				Logger::LogToFile("[sv:err:network:serverInfo] : connect error (code:%d)", WSAGetLastError());
				return false;

			}

			Network::serverKey = stData->serverKey;

			Network::outputVoicePacket->svrkey = Network::serverKey;
			Network::outputVoicePacket->packet = SV::VoicePacketType::voicePacket;
			Network::outputVoicePacket->packid = NULL;
			Network::outputVoicePacket->length = NULL;
			Network::outputVoicePacket->sender = NULL;
			Network::outputVoicePacket->stream = NULL;
			Network::outputVoicePacket->CalcHash();

			Network::connectionStatus.store(ConnectionStatusType::SVConnecting);

			Network::voiceThread = std::thread(&Network::VoiceThread);

		} break;
		case SV::ControlPacketType::pluginInit:
		{

			const auto stData = (SV::PluginInitPacket*)(controlPacketPtr->data);
			if (controlPacketPtr->length != sizeof(*stData)) return false;

			Logger::LogToFile(
				"[sv:dbg:network:pluginInit] : plugin init packet (bitrate:%u;mute:%hhu)",
				stData->bitrate, stData->mute
			);

			Network::packetId = NULL;

			if (Network::pluginInitHandler) Network::pluginInitHandler(*stData);

			Network::connectionStatus.store(ConnectionStatusType::Connected);

		} break;
		default:
		{

			if (const auto controlPacket = MakeControlPacketContainer(controlPacketPtr, controlPacketSize))
				Network::controlQueue.push(controlPacket);

		}
	}

	return false;

}

void Network::OnRaknetDisconnect() {

	if (!Network::initStatus) return;

	Logger::LogToFile("[sv:dbg:network:disconnect]");

	if (Network::disconnectHandler) Network::disconnectHandler();

	Network::connectionStatus.store(ConnectionStatusType::Disconnected);

	while (!Network::controlQueue.empty()) Network::controlQueue.pop();
	if (Network::voiceThread.joinable()) Network::voiceThread.join();
	while (!Network::voiceQueue.empty()) Network::voiceQueue.pop();

	Network::serverIp.clear();
	Network::serverKey = NULL;
	Network::packetId = NULL;

}

bool Network::Init(
	const AddressesBase& addrBase,
	const ConnectHandlerType& connectHandler,
	const PluginConnectHandlerType& pluginConnectHandler,
	const PluginInitHandlerType& pluginInitHandler,
	const DisconnectHandlerType& disconnectHandler
) {

	if (Network::initStatus) return false;

	Logger::LogToFile("[sv:dbg:network:init] : module initializing...");

	if (const auto error = WSAStartup(MAKEWORD(2, 2), &WSADATA())) {

		Logger::LogToFile("[sv:err:network:init] : wsastartup error (code:%d)", error);

		return false;

	}

	if ((Network::socketHandle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET) {

		Logger::LogToFile("[sv:err:network:init] : socket error (code:%d)", WSAGetLastError());

		WSACleanup();

		return false;

	}

	const int sendBufferSize = SendBufferSize, recvBufferSize = RecvBufferSize;

	if (setsockopt(Network::socketHandle, SOL_SOCKET, SO_SNDBUF, (PCCH)(&sendBufferSize), sizeof(sendBufferSize)) == SOCKET_ERROR ||
		setsockopt(Network::socketHandle, SOL_SOCKET, SO_RCVBUF, (PCCH)(&recvBufferSize), sizeof(recvBufferSize)) == SOCKET_ERROR
	) {

		Logger::LogToFile("[sv:err:network:init] : setsockopt error (code:%d)", WSAGetLastError());

		closesocket(Network::socketHandle);
		WSACleanup();
		
		return false;

	}

	if (u_long mode = TRUE, error; (error = ioctlsocket(Network::socketHandle, FIONBIO, &mode)) != NO_ERROR) {

		Logger::LogToFile("[sv:err:network:init] : failed to put socket into non-blocking mode (code:%d)", error);

		closesocket(Network::socketHandle);
		WSACleanup();
		
		return false;

	}

	if (!RakNet::Init(addrBase, &Network::OnRaknetConnect, &Network::OnRaknetReceivePacket, nullptr, &Network::OnRaknetSendRpc, &Network::OnRaknetDisconnect)) {

		Logger::LogToFile("[sv:err:network:init] : failed to init raknet");

		closesocket(Network::socketHandle);
		WSACleanup();

		return false;

	}

	Network::connectHandler = connectHandler;
	Network::pluginConnectHandler = pluginConnectHandler;
	Network::pluginInitHandler = pluginInitHandler;
	Network::disconnectHandler = disconnectHandler;

	Network::connectionStatus.store(ConnectionStatusType::Disconnected);

	Logger::LogToFile("[sv:dbg:network:init] : module initialized");

	return Network::initStatus = true;

}

bool Network::SendControlPacket(WORD packet, LPCVOID dataAddr, WORD dataSize) {

	if (!Network::initStatus) return false;

	if (Network::connectionStatus.load() != ConnectionStatusType::Connected) return false;

	BitStream bitStream;

	bitStream.Write<BYTE>(RaknetPacketId);

	bitStream.Write<WORD>(packet);
	bitStream.Write<WORD>(dataSize);

	if (dataAddr && dataSize) bitStream.Write((PCCH)(dataAddr), dataSize);

	return RakNet::Send(&bitStream);

}

bool Network::SendVoicePacket(LPCVOID dataAddr, WORD dataSize) {

	if (!Network::initStatus) return false;

	if (!dataAddr || !dataSize || dataSize > MaxVoiceDataSize) return false;
	if (Network::connectionStatus.load() != ConnectionStatusType::Connected) return false;

	Network::outputVoicePacket->packid = Network::packetId++;
	Network::outputVoicePacket->length = dataSize;
	Network::outputVoicePacket->CalcHash();

	memcpy(Network::outputVoicePacket->data, dataAddr, dataSize);

	const auto voicePacketAddr = (PCHAR)(&Network::outputVoicePacket);
	const auto voicePacketSize = Network::outputVoicePacket->GetFullSize();

	return send(Network::socketHandle, voicePacketAddr, voicePacketSize, NULL) == voicePacketSize;

}

void Network::EndSequence() {

	if (!Network::initStatus) return;

	Network::packetId = NULL;

}

ControlPacketContainerPtr Network::ReceiveControlPacket() {

	if (!Network::initStatus) return nullptr;

	if (Network::controlQueue.empty()) return nullptr;

	const auto controlPacket = *Network::controlQueue.front();

	Network::controlQueue.pop();
	return controlPacket;

}

VoicePacketContainerPtr Network::ReceiveVoicePacket() {

	if (!Network::initStatus) return nullptr;

	if (Network::voiceQueue.empty()) return nullptr;

	const auto voicePacket = *Network::voiceQueue.front();

	Network::voiceQueue.pop();
	return voicePacket;

}

void Network::Free() {

	if (!Network::initStatus) return;

	Logger::LogToFile("[sv:dbg:network:free] : module releasing...");

	Network::connectionStatus.store(ConnectionStatusType::Disconnected);

	while (!Network::controlQueue.empty()) Network::controlQueue.pop();
	if (Network::voiceThread.joinable()) Network::voiceThread.join();
	while (!Network::voiceQueue.empty()) Network::voiceQueue.pop();

	Network::serverIp.clear();
	Network::serverKey = NULL;
	Network::packetId = NULL;

	closesocket(Network::socketHandle);
	Network::socketHandle = NULL;

	Network::connectHandler = nullptr;
	Network::pluginConnectHandler = nullptr;
	Network::pluginInitHandler = nullptr;
	Network::disconnectHandler = nullptr;

	WSACleanup();

	RakNet::Free();

	Logger::LogToFile("[sv:dbg:network:free] : module released");

	Network::initStatus = false;

}
