/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Network.h"

#include <cassert>

#include <util/Logger.h>
#include <util/RakNet.h>
#include <util/GameUtil.h>

#pragma comment(lib, "Ws2_32.lib")

bool Network::Init(const AddressesBase& const addrBase,
                   ConnectHandlerType connectHandler,
                   PluginConnectHandlerType pluginConnectHandler,
                   PluginInitHandlerType pluginInitHandler,
                   DisconnectHandlerType disconnectHandler)
{
    if (Network::initStatus) return false;

    Logger::LogToFile("[sv:dbg:network:init] : module initializing...");

    if (const int error = WSAStartup(MAKEWORD(2, 2), &WSADATA()))
    {
        Logger::LogToFile("[sv:err:network:init] : wsastartup error (code:%d)", error);
        return false;
    }

    if ((Network::socketHandle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
    {
        Logger::LogToFile("[sv:err:network:init] : socket error (code:%d)", WSAGetLastError());
        WSACleanup();
        return false;
    }

    if (const int sendBufferSize { SendBufferSize }, recvBufferSize { RecvBufferSize };
        setsockopt(Network::socketHandle, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<PCCH>(&sendBufferSize), sizeof(sendBufferSize)) == SOCKET_ERROR ||
        setsockopt(Network::socketHandle, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<PCCH>(&recvBufferSize), sizeof(recvBufferSize)) == SOCKET_ERROR)
    {
        Logger::LogToFile("[sv:err:network:init] : setsockopt error (code:%d)", WSAGetLastError());
        closesocket(Network::socketHandle);
        WSACleanup();
        return false;
    }

    if (DWORD mode { TRUE }, error; (error = ioctlsocket(Network::socketHandle, FIONBIO, &mode)) != NO_ERROR)
    {
        Logger::LogToFile("[sv:err:network:init] : failed to put socket into non-blocking mode (code:%d)", error);
        closesocket(Network::socketHandle);
        WSACleanup();
        return false;
    }

    if (!RakNet::Init(addrBase, Network::OnRaknetConnect, Network::OnRaknetReceivePacket,
                      nullptr, Network::OnRaknetSendRpc, Network::OnRaknetDisconnect))
    {
        Logger::LogToFile("[sv:err:network:init] : failed to init raknet");
        closesocket(Network::socketHandle);
        WSACleanup();
        return false;
    }

    Network::connectHandler = std::move(connectHandler);
    Network::pluginConnectHandler = std::move(pluginConnectHandler);
    Network::pluginInitHandler = std::move(pluginInitHandler);
    Network::disconnectHandler = std::move(disconnectHandler);

    Network::connectionStatus = ConnectionStatusType::Disconnected;

    Logger::LogToFile("[sv:dbg:network:init] : module initialized");

    Network::initStatus = true;

    return true;
}

bool Network::SendControlPacket(WORD packet, LPCVOID dataAddr, WORD dataSize)
{
    if (!Network::initStatus) return false;

    if (Network::connectionStatus != ConnectionStatusType::Connected)
        return false;

    BitStream bitStream { sizeof(BYTE) + sizeof(ControlPacket) + dataSize };

    bitStream.Write<BYTE>(RaknetPacketId);

    bitStream.Write<WORD>(packet);
    bitStream.Write<WORD>(dataSize);

    if (dataAddr != nullptr && dataSize != NULL)
        bitStream.Write(static_cast<PCCH>(dataAddr), dataSize);

    return RakNet::Send(&bitStream);
}

bool Network::SendVoicePacket(LPCVOID dataAddr, WORD dataSize)
{
    if (dataAddr == nullptr || dataSize == NULL || dataSize > MaxVoiceDataSize)
        return false;

    if (!Network::initStatus) return false;

    if (Network::connectionStatus != ConnectionStatusType::Connected)
        return false;

    Network::outputVoicePacket->packid = Network::packetId++;
    Network::outputVoicePacket->length = dataSize;
    Network::outputVoicePacket->CalcHash();

    std::memcpy(Network::outputVoicePacket->data, dataAddr, dataSize);

    const auto voicePacketAddr = reinterpret_cast<PCCH>(&Network::outputVoicePacket);
    const auto voicePacketSize = static_cast<int>(Network::outputVoicePacket->GetFullSize());

    return send(Network::socketHandle, voicePacketAddr, voicePacketSize, NULL) == voicePacketSize;
}

void Network::EndSequence()
{
    if (!Network::initStatus) return;

    Network::packetId = NULL;
}

ControlPacketContainerPtr Network::ReceiveControlPacket()
{
    if (!Network::initStatus) return nullptr;

    if (Network::controlQueue.empty()) return nullptr;

    auto controlPacket = std::move(*Network::controlQueue.front());
    Network::controlQueue.pop();

    return controlPacket;
}

VoicePacketContainerPtr Network::ReceiveVoicePacket()
{
    if (!Network::initStatus) return nullptr;

    if (Network::voiceQueue.empty()) return nullptr;

    auto voicePacket = std::move(*Network::voiceQueue.front());
    Network::voiceQueue.pop();

    return voicePacket;
}

void Network::Free()
{
    if (!Network::initStatus) return;

    Logger::LogToFile("[sv:dbg:network:free] : module releasing...");

    Network::connectionStatus = ConnectionStatusType::Disconnected;

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

void Network::VoiceThread()
{
    VoicePacket keepAlivePacket;
    int64_t keepAliveLastTime { NULL };

    ZeroMemory(&keepAlivePacket, sizeof(keepAlivePacket));

    keepAlivePacket.svrkey = Network::serverKey;
    keepAlivePacket.packet = SV::VoicePacketType::keepAlive;
    keepAlivePacket.packid = NULL;
    keepAlivePacket.length = NULL;
    keepAlivePacket.sender = NULL;
    keepAlivePacket.stream = NULL;
    keepAlivePacket.CalcHash();

    while (true)
    {
        const auto curStatus = Network::connectionStatus;

        if (curStatus == ConnectionStatusType::Disconnected) break;

        // Sending keep-alive packets...
        // -----------------------------------------------------------------

        const int64_t curTime = GetTimestamp();

        if (curTime - keepAliveLastTime >= KeepAliveInterval)
        {
            send(Network::socketHandle, reinterpret_cast<PCCH>(&keepAlivePacket),
                 sizeof(keepAlivePacket), NULL);

            keepAliveLastTime = curTime;
        }

        // Receiving voice packets...
        // -----------------------------------------------------------------

        if (curStatus != ConnectionStatusType::Connected)
        {
            SleepForMilliseconds(100);
            continue;
        }

        const auto receivedDataSize = recv(Network::socketHandle,
            static_cast<PCH>(Network::inputVoicePacket.GetData()),
            Network::inputVoicePacket.GetSize(), NULL);

        if (receivedDataSize == SOCKET_ERROR)
        {
            if (WSAGetLastError() == WSAEWOULDBLOCK)
                SleepForMilliseconds(100);

            continue;
        }

        if (receivedDataSize < static_cast<decltype(receivedDataSize)>(sizeof(VoicePacket)))
            continue;

        if (!Network::inputVoicePacket->CheckHeader()) continue;
        if (receivedDataSize != Network::inputVoicePacket->GetFullSize()) continue;
        if (Network::inputVoicePacket->packet == SV::VoicePacketType::keepAlive) continue;
        if (!GameUtil::IsGameActive()) continue;

        Network::voiceQueue.emplace(MakeVoicePacketContainer(Network::inputVoicePacket));
    }
}

void Network::OnRaknetConnect(const PCCH serverIp, const WORD serverPort)
{
    if (!Network::initStatus) return;

    Network::serverIp = serverIp;
    Network::connectHandler(Network::serverIp, serverPort);
}

bool Network::OnRaknetSendRpc(const int rpcId, BitStream* const rpcParameters)
{
    if (!Network::initStatus) return true;

    if (rpcId != RaknetConnectRcpId)
        return true;

    assert(Network::pluginConnectHandler);

    SV::ConnectPacket stData {};
    Network::pluginConnectHandler(stData);
    rpcParameters->Write(stData);

    Logger::LogToFile("[sv:dbg:network:connect] : raknet connecting... (version:%hhu;micro:%hhu)",
        stData.version, stData.micro);

    Network::connectionStatus = ConnectionStatusType::RNConnecting;

    return true;
}

bool Network::OnRaknetReceivePacket(Packet* const packet)
{
    if (!Network::initStatus) return true;

    if (packet->length < sizeof(BYTE) + sizeof(ControlPacket))
        return true;

    if (*packet->data != RaknetPacketId)
        return true;

    const auto controlPacketPtr = reinterpret_cast<ControlPacket*>(packet->data + sizeof(BYTE));
    const DWORD controlPacketSize = packet->length - sizeof(BYTE);

    if (controlPacketSize != controlPacketPtr->GetFullSize())
        return false;

    switch (controlPacketPtr->packet)
    {
        case SV::ControlPacketType::serverInfo:
        {
            const auto stData = reinterpret_cast<SV::ServerInfoPacket*>(controlPacketPtr->data);
            if (controlPacketPtr->length != sizeof(*stData)) return false;

            Logger::LogToFile("[sv:dbg:network:serverInfo] : connecting to voiceserver '%s:%hu'...",
                Network::serverIp.c_str(), stData->serverPort);

            sockaddr_in serverAddress {};

            serverAddress.sin_family = AF_INET;
            serverAddress.sin_addr.s_addr = inet_addr(Network::serverIp.c_str());
            serverAddress.sin_port = htons(stData->serverPort);

            if (connect(Network::socketHandle, reinterpret_cast<const sockaddr*>(&serverAddress),
                sizeof(serverAddress)) == SOCKET_ERROR)
            {
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

            Network::connectionStatus = ConnectionStatusType::SVConnecting;

            Network::voiceThread = std::thread(Network::VoiceThread);
        } break;
        case SV::ControlPacketType::pluginInit:
        {
            const auto stData = reinterpret_cast<SV::PluginInitPacket*>(controlPacketPtr->data);
            if (controlPacketPtr->length != sizeof(*stData)) return false;

            Logger::LogToFile("[sv:dbg:network:pluginInit] : plugin init packet (bitrate:%u;mute:%hhu)",
                stData->bitrate, stData->mute);

            Network::packetId = NULL;

            if (Network::pluginInitHandler) Network::pluginInitHandler(*stData);

            Network::connectionStatus = ConnectionStatusType::Connected;
        } break;
        default:
        {
            Network::controlQueue.emplace(MakeControlPacketContainer(controlPacketPtr, controlPacketSize));
        }
    }

    return false;
}

void Network::OnRaknetDisconnect() noexcept
{
    if (!Network::initStatus) return;

    Logger::LogToFile("[sv:dbg:network:disconnect] : raknet disconnected");

    if (Network::disconnectHandler) Network::disconnectHandler();

    Network::connectionStatus = ConnectionStatusType::Disconnected;

    while (!Network::controlQueue.empty()) Network::controlQueue.pop();
    if (Network::voiceThread.joinable()) Network::voiceThread.join();
    while (!Network::voiceQueue.empty()) Network::voiceQueue.pop();

    Network::serverIp.clear();
    Network::serverKey = NULL;
    Network::packetId = NULL;
}

bool Network::initStatus { false };

SOCKET Network::socketHandle { NULL };
int Network::connectionStatus { ConnectionStatusType::Disconnected };

Network::ConnectHandlerType Network::connectHandler { nullptr };
Network::PluginConnectHandlerType Network::pluginConnectHandler { nullptr };
Network::PluginInitHandlerType Network::pluginInitHandler { nullptr };
Network::DisconnectHandlerType Network::disconnectHandler { nullptr };

SPSCQueue<ControlPacketContainerPtr> Network::controlQueue { 128 };
SPSCQueue<VoicePacketContainerPtr> Network::voiceQueue { 512 };

std::thread Network::voiceThread;
std::string Network::serverIp;

VoicePacketContainer Network::inputVoicePacket { MaxVoicePacketSize };
VoicePacketContainer Network::outputVoicePacket { MaxVoicePacketSize };

DWORD Network::serverKey { NULL };
DWORD Network::packetId { NULL };
