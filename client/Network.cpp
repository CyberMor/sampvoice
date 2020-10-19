/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Network.h"

#include <util/Logger.h>
#include <util/RakNet.h>
#include <util/GameUtil.h>

#pragma comment(lib, "Ws2_32.lib")

bool Network::Init(const AddressesBase& addrBase) noexcept
{
    if (Network::initStatus)
        return false;

    Logger::LogToFile("[sv:dbg:network:init] : module initializing...");

    if (const auto error = WSAStartup(MAKEWORD(2, 2), &WSADATA()))
    {
        Logger::LogToFile("[sv:err:network:init] : wsastartup error (code:%d)", error);
        return false;
    }

    Memory::ScopeExit wsaResetScope { [] { WSACleanup(); } };

    Network::socketHandle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (Network::socketHandle == INVALID_SOCKET)
    {
        Logger::LogToFile("[sv:err:network:init] : socket error (code:%d)", WSAGetLastError());
        return false;
    }

    Memory::ScopeExit socketResetScope { [] { closesocket(Network::socketHandle);
                                              Network::socketHandle = INVALID_SOCKET; } };

    if (const int sendBufferSize { kSendBufferSize }, recvBufferSize { kRecvBufferSize };
        setsockopt(Network::socketHandle, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<PCCH>
            (&sendBufferSize), sizeof(sendBufferSize)) == SOCKET_ERROR ||
        setsockopt(Network::socketHandle, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<PCCH>
            (&recvBufferSize), sizeof(recvBufferSize)) == SOCKET_ERROR)
    {
        Logger::LogToFile("[sv:err:network:init] : setsockopt error (code:%d)", WSAGetLastError());
        return false;
    }

    if (!RakNet::Init(addrBase))
    {
        Logger::LogToFile("[sv:err:network:init] : failed to init raknet");
        return false;
    }

    RakNet::AddConnectCallback(Network::OnRaknetConnect);
    RakNet::AddReceiveCallback(Network::OnRaknetReceive);
    RakNet::AddRpcCallback(Network::OnRaknetRpc);
    RakNet::AddDisconnectCallback(Network::OnRaknetDisconnect);

    Network::connectionStatus = ConnectionStatus::Disconnected;

    wsaResetScope.Release();
    socketResetScope.Release();

    Logger::LogToFile("[sv:dbg:network:init] : module initialized");

    Network::initStatus = true;

    return true;
}

void Network::Free() noexcept
{
    if (!Network::initStatus)
        return;

    Logger::LogToFile("[sv:dbg:network:free] : module releasing...");

    if (Network::connectionStatus != ConnectionStatus::Disconnected)
    {
        for (const auto& disconnectCallback : Network::disconnectCallbacks)
        {
            if (disconnectCallback != nullptr) disconnectCallback();
        }
    }

    Network::connectionStatus = ConnectionStatus::Disconnected;

    if (Network::voiceThread.joinable())
        Network::voiceThread.detach();

    closesocket(Network::socketHandle);
    Network::socketHandle = INVALID_SOCKET;

    Network::serverIp.clear();
    Network::serverKey = NULL;

    Network::connectCallbacks.clear();
    Network::svConnectCallbacks.clear();
    Network::svInitCallbacks.clear();
    Network::disconnectCallbacks.clear();

    ZeroMemory(Network::inputVoicePacket.GetData(),
        Network::inputVoicePacket.GetSize());
    ZeroMemory(Network::outputVoicePacket.GetData(),
        Network::outputVoicePacket.GetSize());

    while (!Network::controlQueue.empty())
        Network::controlQueue.pop();
    while (!Network::voiceQueue.empty())
        Network::voiceQueue.pop();

    RakNet::Free();
    WSACleanup();

    Logger::LogToFile("[sv:dbg:network:free] : module released");

    Network::initStatus = false;
}

bool Network::SendControlPacket(const WORD packet, const LPCVOID dataAddr, const WORD dataSize) noexcept
{
    if (Network::connectionStatus != ConnectionStatus::Connected)
        return false;

    BitStream bitStream { sizeof(BYTE) + sizeof(ControlPacket) + dataSize };

    bitStream.Write<BYTE>(kRaknetPacketId);

    bitStream.Write<WORD>(packet);
    bitStream.Write<WORD>(dataSize);

    if (dataAddr != nullptr && dataSize != 0)
    {
        bitStream.Write(static_cast<PCCH>(dataAddr), dataSize);
    }

    return RakNet::Send(&bitStream);
}

bool Network::SendVoicePacket(const LPCVOID dataAddr, const WORD dataSize) noexcept
{
    if (dataAddr == nullptr || dataSize == 0 || dataSize > kMaxVoiceDataSize)
        return false;

    if (Network::connectionStatus != ConnectionStatus::Connected)
        return false;

    Network::outputVoicePacket->length = dataSize;
    Network::outputVoicePacket->CalcHash();

    std::memcpy(Network::outputVoicePacket->data, dataAddr, dataSize);

    const auto voicePacketAddr = reinterpret_cast<PCCH>(&Network::outputVoicePacket);
    const auto voicePacketSize = static_cast<int>(Network::outputVoicePacket->GetFullSize());

    const auto sended = send(Network::socketHandle, voicePacketAddr, voicePacketSize, NULL);

    ++Network::outputVoicePacket->packid;

    return sended == voicePacketSize;
}

void Network::EndSequence() noexcept
{
    if (!Network::initStatus)
        return;

    Network::outputVoicePacket->packid = NULL;
}

ControlPacketContainerPtr Network::ReceiveControlPacket() noexcept
{
    if (!Network::initStatus || Network::controlQueue.empty())
        return nullptr;

    auto controlPacket = std::move(*Network::controlQueue.front());
    Network::controlQueue.pop();

    return controlPacket;
}

VoicePacketContainerPtr Network::ReceiveVoicePacket() noexcept
{
    if (!Network::initStatus || Network::voiceQueue.empty())
        return nullptr;

    auto voicePacket = std::move(*Network::voiceQueue.front());
    Network::voiceQueue.pop();

    return voicePacket;
}

std::size_t Network::AddConnectCallback(ConnectCallback callback) noexcept
{
    if (!Network::initStatus)
        return -1;

    for (std::size_t i { 0 }; i < Network::connectCallbacks.size(); ++i)
    {
        if (Network::connectCallbacks[i] == nullptr)
        {
            Network::connectCallbacks[i] = std::move(callback);
            return i;
        }
    }

    Network::connectCallbacks.emplace_back(std::move(callback));
    return Network::connectCallbacks.size() - 1;
}

std::size_t Network::AddSvConnectCallback(SvConnectCallback callback) noexcept
{
    if (!Network::initStatus)
        return -1;

    for (std::size_t i { 0 }; i < Network::svConnectCallbacks.size(); ++i)
    {
        if (Network::svConnectCallbacks[i] == nullptr)
        {
            Network::svConnectCallbacks[i] = std::move(callback);
            return i;
        }
    }

    Network::svConnectCallbacks.emplace_back(std::move(callback));
    return Network::svConnectCallbacks.size() - 1;
}

std::size_t Network::AddSvInitCallback(SvInitCallback callback) noexcept
{
    if (!Network::initStatus)
        return -1;

    for (std::size_t i { 0 }; i < Network::svInitCallbacks.size(); ++i)
    {
        if (Network::svInitCallbacks[i] == nullptr)
        {
            Network::svInitCallbacks[i] = std::move(callback);
            return i;
        }
    }

    Network::svInitCallbacks.emplace_back(std::move(callback));
    return Network::svInitCallbacks.size() - 1;
}

std::size_t Network::AddDisconnectCallback(DisconnectCallback callback) noexcept
{
    if (!Network::initStatus)
        return -1;

    for (std::size_t i { 0 }; i < Network::disconnectCallbacks.size(); ++i)
    {
        if (Network::disconnectCallbacks[i] == nullptr)
        {
            Network::disconnectCallbacks[i] = std::move(callback);
            return i;
        }
    }

    Network::disconnectCallbacks.emplace_back(std::move(callback));
    return Network::disconnectCallbacks.size() - 1;
}

void Network::RemoveConnectCallback(const std::size_t callback) noexcept
{
    if (!Network::initStatus)
        return;

    if (callback >= Network::connectCallbacks.size())
        return;

    Network::connectCallbacks[callback] = nullptr;
}

void Network::RemoveSvConnectCallback(const std::size_t callback) noexcept
{
    if (!Network::initStatus)
        return;

    if (callback >= Network::svConnectCallbacks.size())
        return;

    Network::svConnectCallbacks[callback] = nullptr;
}

void Network::RemoveSvInitCallback(const std::size_t callback) noexcept
{
    if (!Network::initStatus)
        return;

    if (callback >= Network::svInitCallbacks.size())
        return;

    Network::svInitCallbacks[callback] = nullptr;
}

void Network::RemoveDisconnectCallback(const std::size_t callback) noexcept
{
    if (!Network::initStatus)
        return;

    if (callback >= Network::disconnectCallbacks.size())
        return;

    Network::disconnectCallbacks[callback] = nullptr;
}

void Network::VoiceThread() noexcept
{
    VoicePacket keepAlivePacket;
    Timer::time_t keepAliveLastTime { NULL };

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

        if (curStatus == ConnectionStatus::Disconnected) break;

        // Sending keep-alive packets...
        // -----------------------------------------------------------------

        const auto curTime = Timer::Get();

        if (curTime - keepAliveLastTime >= kKeepAliveInterval)
        {
            send(Network::socketHandle, reinterpret_cast<PCCH>(&keepAlivePacket),
                 sizeof(keepAlivePacket), NULL);

            keepAliveLastTime = curTime;
        }

        // Receiving voice packets...
        // -----------------------------------------------------------------

        if (curStatus != ConnectionStatus::Connected)
        {
            SleepForMilliseconds(100);
            continue;
        }

        const auto received = recv(Network::socketHandle,
            static_cast<PCH>(Network::inputVoicePacket.GetData()),
            Network::inputVoicePacket.GetSize(), NULL);

        if (received == SOCKET_ERROR)
            break;

        if (received < static_cast<decltype(received)>(sizeof(VoicePacket)))
            continue;

        if (!Network::inputVoicePacket->CheckHeader()) continue;
        if (received != Network::inputVoicePacket->GetFullSize()) continue;
        if (Network::inputVoicePacket->packet == SV::VoicePacketType::keepAlive) continue;
        if (!GameUtil::IsGameActive()) continue;

        Network::voiceQueue.try_emplace(MakeVoicePacketContainer(Network::inputVoicePacket));
    }
}

void Network::OnRaknetConnect(const PCCH ip, const WORD port) noexcept
{
    if (!Network::initStatus)
        return;

    Network::serverIp = ip;

    if (Network::connectionStatus == ConnectionStatus::Disconnected)
    {
        for (const auto& connectCallback : Network::connectCallbacks)
        {
            if (connectCallback != nullptr) connectCallback(Network::serverIp, port);
        }
    }

    Network::connectionStatus = ConnectionStatus::RNConnecting;
}

bool Network::OnRaknetRpc(const int id, BitStream& parameters) noexcept
{
    if (!Network::initStatus || id != kRaknetConnectRcpId)
        return true;

    if (Network::svConnectCallbacks.empty())
        return true;

    SV::ConnectPacket stData {};

    for (const auto& svConnectCallback : Network::svConnectCallbacks)
    {
        if (svConnectCallback != nullptr) svConnectCallback(stData);
    }

    parameters.Write(reinterpret_cast<const char*>(&stData), sizeof(stData));

    Logger::LogToFile("[sv:dbg:network:connect] : raknet connecting... "
        "(version:%hhu;micro:%hhu)", stData.version, stData.micro);

    return true;
}

bool Network::OnRaknetReceive(Packet& packet) noexcept
{
    if (!Network::initStatus)
        return true;

    if (packet.length < sizeof(BYTE) + sizeof(ControlPacket))
        return true;

    if (*packet.data != kRaknetPacketId)
        return true;

    const auto controlPacketPtr = reinterpret_cast<ControlPacket*>(packet.data + sizeof(BYTE));
    const DWORD controlPacketSize = packet.length - sizeof(BYTE);

    if (controlPacketSize != controlPacketPtr->GetFullSize())
        return false;

    switch (controlPacketPtr->packet)
    {
        case SV::ControlPacketType::serverInfo:
        {
            const auto& stData = *reinterpret_cast<SV::ServerInfoPacket*>(controlPacketPtr->data);
            if (controlPacketPtr->length != sizeof(stData)) return false;

            Logger::LogToFile("[sv:dbg:network:serverInfo] : connecting to voiceserver "
                "'%s:%hu'...", Network::serverIp.c_str(), stData.serverPort);

            sockaddr_in serverAddress {};

            serverAddress.sin_family = AF_INET;
            serverAddress.sin_addr.s_addr = inet_addr(Network::serverIp.c_str());
            serverAddress.sin_port = htons(stData.serverPort);

            if (connect(Network::socketHandle, reinterpret_cast<const sockaddr*>(&serverAddress),
                sizeof(serverAddress)) == SOCKET_ERROR)
            {
                Logger::LogToFile("[sv:err:network:serverInfo] : connect error (code:%d)", WSAGetLastError());
                return false;
            }

            Network::serverKey = stData.serverKey;

            Network::outputVoicePacket->svrkey = Network::serverKey;
            Network::outputVoicePacket->packet = SV::VoicePacketType::voicePacket;
            Network::outputVoicePacket->packid = NULL;
            Network::outputVoicePacket->length = NULL;
            Network::outputVoicePacket->sender = NULL;
            Network::outputVoicePacket->stream = NULL;
            Network::outputVoicePacket->CalcHash();

            Network::connectionStatus = ConnectionStatus::SVConnecting;

            Network::voiceThread = std::thread(Network::VoiceThread);
        } break;
        case SV::ControlPacketType::pluginInit:
        {
            const auto& stData = *reinterpret_cast<SV::PluginInitPacket*>(controlPacketPtr->data);
            if (controlPacketPtr->length != sizeof(stData)) return false;

            Logger::LogToFile("[sv:dbg:network:pluginInit] : plugin init packet "
                "(bitrate:%u;mute:%hhu)", stData.bitrate, stData.mute);

            for (const auto& svInitCallback : Network::svInitCallbacks)
            {
                if (svInitCallback != nullptr) svInitCallback(stData);
            }

            Network::connectionStatus = ConnectionStatus::Connected;
        } break;
        default:
        {
            Network::controlQueue.try_emplace(MakeControlPacketContainer(controlPacketPtr, controlPacketSize));
        }
    }

    return false;
}

void Network::OnRaknetDisconnect() noexcept
{
    if (!Network::initStatus)
        return;

    Logger::LogToFile("[sv:dbg:network:disconnect] : raknet disconnected");

    if (Network::connectionStatus != ConnectionStatus::Disconnected)
    {
        for (const auto& disconnectCallback : Network::disconnectCallbacks)
        {
            if (disconnectCallback != nullptr) disconnectCallback();
        }
    }

    Network::connectionStatus = ConnectionStatus::Disconnected;

    if (Network::voiceThread.joinable())
        Network::voiceThread.detach();

    Network::serverIp.clear();
    Network::serverKey = NULL;

    ZeroMemory(Network::inputVoicePacket.GetData(),
        Network::inputVoicePacket.GetSize());
    ZeroMemory(Network::outputVoicePacket.GetData(),
        Network::outputVoicePacket.GetSize());

    while (!Network::controlQueue.empty())
        Network::controlQueue.pop();
    while (!Network::voiceQueue.empty())
        Network::voiceQueue.pop();
}

bool Network::initStatus { false };

SOCKET Network::socketHandle { INVALID_SOCKET };
int Network::connectionStatus { ConnectionStatus::Disconnected };
std::thread Network::voiceThread;
std::string Network::serverIp;
DWORD Network::serverKey { NULL };

std::vector<Network::ConnectCallback> Network::connectCallbacks;
std::vector<Network::SvConnectCallback> Network::svConnectCallbacks;
std::vector<Network::SvInitCallback> Network::svInitCallbacks;
std::vector<Network::DisconnectCallback> Network::disconnectCallbacks;

SPSCQueue<ControlPacketContainerPtr> Network::controlQueue { 128 };
SPSCQueue<VoicePacketContainerPtr> Network::voiceQueue { 512 };

VoicePacketContainer Network::inputVoicePacket { kMaxVoiceDataSize };
VoicePacketContainer Network::outputVoicePacket { kMaxVoiceDataSize };
