/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "RakNet.h"

#include "Logger.h"

static DWORD rakClientOffset { 0 };

bool RakNet::Init(const AddressesBase& addrBase) noexcept
{
    if (RakNet::initStatus)
        return false;

    Logger::LogToFile("[dbg:raknet:init] : module initializing...");

    if (*reinterpret_cast<WORD*>(addrBase.GetRcInitAddr()) == 0x4689)
        rakClientOffset = *reinterpret_cast<BYTE*>(addrBase.GetRcInitAddr() + 2);
    else if (*reinterpret_cast<WORD*>(addrBase.GetRcInitAddr()) == 0x8689)
        rakClientOffset = *reinterpret_cast<DWORD*>(addrBase.GetRcInitAddr() + 2);

    Logger::LogToFile("[dbg:raknet:init] : finded rakclient interface offset (value:0x%x)", rakClientOffset);

    try
    {
        RakNet::sampDestructHook = MakeJumpHook(addrBase.GetSampDestructAddr(), RakNet::HookSampDestruct);
        RakNet::rakClientInitHook = MakeJumpHook(addrBase.GetRcInitAddr(), RakNet::HookRaknetInit);
    }
    catch (const std::exception& exception)
    {
        Logger::LogToFile("[err:raknet:init] : failed to create function hooks");
        RakNet::sampDestructHook.reset();
        RakNet::rakClientInitHook.reset();
        return false;
    }

    RakNet::connectCallbacks.clear();
    RakNet::receiveCallbacks.clear();
    RakNet::sendCallbacks.clear();
    RakNet::rpcCallbacks.clear();
    RakNet::disconnectCallbacks.clear();

    RakNet::connectStatus = false;
    RakNet::loadStatus = false;

    Logger::LogToFile("[dbg:raknet:init] : module initialized");

    RakNet::initStatus = true;

    return true;
}

bool RakNet::IsInited() noexcept
{
    return RakNet::initStatus;
}

bool RakNet::IsLoaded() noexcept
{
    return RakNet::loadStatus;
}

void RakNet::Free() noexcept
{
    if (!RakNet::initStatus)
        return;

    Logger::LogToFile("[dbg:raknet:free] : module releasing...");

    RakNet::sampDestructHook.reset();
    RakNet::rakClientInitHook.reset();

    if (RakNet::connectStatus)
    {
        for (const auto& disconnectCallback : RakNet::disconnectCallbacks)
        {
            if (disconnectCallback != nullptr) disconnectCallback();
        }
    }

    RakNet::connectStatus = false;

    RakNet::connectCallbacks.clear();
    RakNet::receiveCallbacks.clear();
    RakNet::sendCallbacks.clear();
    RakNet::rpcCallbacks.clear();
    RakNet::disconnectCallbacks.clear();

    if (RakNet::loadStatus)
    {
        const auto pRakClientInterfaceHook = reinterpret_cast<RakClientHookInterface*>(*RakNet::ppRakClientInterface);
        *RakNet::ppRakClientInterface = RakNet::pRakClientInterface;
        delete pRakClientInterfaceHook;
    }

    RakNet::loadStatus = false;

    RakNet::ppRakClientInterface = nullptr;
    RakNet::pRakClientInterface = nullptr;

    Logger::LogToFile("[dbg:raknet:free] : module released");

    RakNet::initStatus = false;
}

bool RakNet::IsConnected() noexcept
{
    return RakNet::connectStatus;
}

bool RakNet::Send(BitStream* const bitStream) noexcept
{
    if (bitStream == nullptr)
        return false;

    if (!RakNet::connectStatus)
        return false;

    return RakNet::pRakClientInterface->Send(bitStream, PacketPriority::MEDIUM_PRIORITY,
        PacketReliability::RELIABLE_ORDERED, '\0');
}

std::size_t RakNet::AddConnectCallback(ConnectCallback callback) noexcept
{
    if (!RakNet::initStatus)
        return -1;

    for (std::size_t i { 0 }; i < RakNet::connectCallbacks.size(); ++i)
    {
        if (RakNet::connectCallbacks[i] == nullptr)
        {
            RakNet::connectCallbacks[i] = std::move(callback);
            return i;
        }
    }

    RakNet::connectCallbacks.emplace_back(std::move(callback));
    return RakNet::connectCallbacks.size() - 1;
}

std::size_t RakNet::AddReceiveCallback(ReceiveCallback callback) noexcept
{
    if (!RakNet::initStatus)
        return -1;

    for (std::size_t i { 0 }; i < RakNet::receiveCallbacks.size(); ++i)
    {
        if (RakNet::receiveCallbacks[i] == nullptr)
        {
            RakNet::receiveCallbacks[i] = std::move(callback);
            return i;
        }
    }

    RakNet::receiveCallbacks.emplace_back(std::move(callback));
    return RakNet::receiveCallbacks.size() - 1;
}

std::size_t RakNet::AddSendCallback(SendCallback callback) noexcept
{
    if (!RakNet::initStatus)
        return -1;

    for (std::size_t i { 0 }; i < RakNet::sendCallbacks.size(); ++i)
    {
        if (RakNet::sendCallbacks[i] == nullptr)
        {
            RakNet::sendCallbacks[i] = std::move(callback);
            return i;
        }
    }

    RakNet::sendCallbacks.emplace_back(std::move(callback));
    return RakNet::sendCallbacks.size() - 1;
}

std::size_t RakNet::AddRpcCallback(RpcCallback callback) noexcept
{
    if (!RakNet::initStatus)
        return -1;

    for (std::size_t i { 0 }; i < RakNet::rpcCallbacks.size(); ++i)
    {
        if (RakNet::rpcCallbacks[i] == nullptr)
        {
            RakNet::rpcCallbacks[i] = std::move(callback);
            return i;
        }
    }

    RakNet::rpcCallbacks.emplace_back(std::move(callback));
    return RakNet::rpcCallbacks.size() - 1;
}

std::size_t RakNet::AddDisconnectCallback(DisconnectCallback callback) noexcept
{
    if (!RakNet::initStatus)
        return -1;

    for (std::size_t i { 0 }; i < RakNet::disconnectCallbacks.size(); ++i)
    {
        if (RakNet::disconnectCallbacks[i] == nullptr)
        {
            RakNet::disconnectCallbacks[i] = std::move(callback);
            return i;
        }
    }

    RakNet::disconnectCallbacks.emplace_back(std::move(callback));
    return RakNet::disconnectCallbacks.size() - 1;
}

void RakNet::RemoveConnectCallback(const std::size_t callback) noexcept
{
    if (!RakNet::initStatus)
        return;

    if (callback >= RakNet::connectCallbacks.size())
        return;

    RakNet::connectCallbacks[callback] = nullptr;
}

void RakNet::RemoveReceiveCallback(const std::size_t callback) noexcept
{
    if (!RakNet::initStatus)
        return;

    if (callback >= RakNet::receiveCallbacks.size())
        return;

    RakNet::receiveCallbacks[callback] = nullptr;
}

void RakNet::RemoveSendCallback(const std::size_t callback) noexcept
{
    if (!RakNet::initStatus)
        return;

    if (callback >= RakNet::sendCallbacks.size())
        return;

    RakNet::sendCallbacks[callback] = nullptr;
}

void RakNet::RemoveRpcCallback(const std::size_t callback) noexcept
{
    if (!RakNet::initStatus)
        return;

    if (callback >= RakNet::rpcCallbacks.size())
        return;

    RakNet::rpcCallbacks[callback] = nullptr;
}

void RakNet::RemoveDisconnectCallback(const std::size_t callback) noexcept
{
    if (!RakNet::initStatus)
        return;

    if (callback >= RakNet::disconnectCallbacks.size())
        return;

    RakNet::disconnectCallbacks[callback] = nullptr;
}

RakNet::RakClientHookInterface::RakClientHookInterface(RakClientInterface* const pOrigInterface) noexcept
    : pOrigInterface(pOrigInterface) {}

bool RakNet::RakClientHookInterface::RPC(int* const rpcIdPointer, BitStream* const parameters,
    const PacketPriority priority, const PacketReliability reliability, const char orderingChannel,
    const bool shiftTimestamp) noexcept
{
    bool breakStatus { false };

    for (const auto& rpcCallback : RakNet::rpcCallbacks)
    {
        if (rpcCallback != nullptr && !rpcCallback(*rpcIdPointer, *parameters))
            breakStatus = true;
    }

    if (breakStatus) return true;

    return this->pOrigInterface->RPC(rpcIdPointer, parameters, priority, reliability, orderingChannel, shiftTimestamp);
}

bool RakNet::RakClientHookInterface::Send(BitStream* const bitStream, const PacketPriority priority,
    const PacketReliability reliability, const char orderingChannel) noexcept
{
    bool breakStatus { false };

    for (const auto& sendCallback : RakNet::sendCallbacks)
    {
        if (sendCallback != nullptr && !sendCallback(*bitStream))
            breakStatus = true;
    }

    if (breakStatus) return true;

    return this->pOrigInterface->Send(bitStream, priority, reliability, orderingChannel);
}

Packet* RakNet::RakClientHookInterface::Receive() noexcept
{
    Packet* packetPointer;

    while ((packetPointer = this->pOrigInterface->Receive()) != nullptr)
    {
        bool breakStatus { true };

        for (const auto& receiveCallback : RakNet::receiveCallbacks)
        {
            if (receiveCallback != nullptr && !receiveCallback(*packetPointer))
                breakStatus = false;
        }

        if (breakStatus) break;

        this->pOrigInterface->DeallocatePacket(packetPointer);
    }

    return packetPointer;
}

bool RakNet::RakClientHookInterface::Connect(const char* const hostIp, const uint16_t serverPort,
    const uint16_t clientPort, const uint32_t depreciated, const int threadSleepTimer) noexcept
{
    Logger::LogToFile("[dbg:raknet:client:connect] : connecting to game server '%s:%hu'...", hostIp, serverPort);

    RakNet::connectStatus = this->pOrigInterface->Connect(hostIp, serverPort, clientPort, depreciated, threadSleepTimer);

    if (RakNet::connectStatus)
    {
        Logger::LogToFile("[dbg:raknet:client:connect] : connected");

        for (const auto& connectCallback : RakNet::connectCallbacks)
        {
            if (connectCallback != nullptr) connectCallback(hostIp, serverPort);
        }
    }

    return RakNet::connectStatus;
}

void RakNet::RakClientHookInterface::Disconnect(const uint32_t blockDuration, const uint8_t orderingChannel) noexcept
{
    Logger::LogToFile("[dbg:raknet:client:disconnect] : disconnecting from server...");

    if (RakNet::connectStatus)
    {
        for (const auto& disconnectCallback : RakNet::disconnectCallbacks)
        {
            if (disconnectCallback != nullptr) disconnectCallback();
        }
    }

    RakNet::connectStatus = false;

    this->pOrigInterface->Disconnect(blockDuration, orderingChannel);
}

void RakNet::RakClientHookInterface::InitializeSecurity(const char* const privateKeyP, const char* const privateKeyQ) noexcept
{
    this->pOrigInterface->InitializeSecurity(privateKeyP, privateKeyQ);
}

void RakNet::RakClientHookInterface::SetPassword(const char* const password) noexcept
{
    this->pOrigInterface->SetPassword(password);
}

bool RakNet::RakClientHookInterface::HasPassword() noexcept
{
    return this->pOrigInterface->HasPassword();
}

bool RakNet::RakClientHookInterface::Send(const char* const dataPointer, const int dataLength,
    const PacketPriority priority, const PacketReliability reliability, const char orderingChannel) noexcept
{
    BitStream bitStream { (uint8_t*)(dataPointer), static_cast<uint32_t>(dataLength), false };

    bool breakStatus { false };

    for (const auto& sendCallback : RakNet::sendCallbacks)
    {
        if (sendCallback != nullptr && !sendCallback(bitStream))
            breakStatus = true;
    }

    if (breakStatus) return true;

    return this->pOrigInterface->Send(&bitStream, priority, reliability, orderingChannel);
}

void RakNet::RakClientHookInterface::DeallocatePacket(Packet* const packetPointer) noexcept
{
    this->pOrigInterface->DeallocatePacket(packetPointer);
}

void RakNet::RakClientHookInterface::PingServer() noexcept
{
    this->pOrigInterface->PingServer();
}

void RakNet::RakClientHookInterface::PingServer(const char* const hostIp,
    const uint16_t serverPort, const uint16_t clientPort,
    const bool onlyReplyOnAcceptingConnections) noexcept
{
    this->pOrigInterface->PingServer(hostIp, serverPort, clientPort, onlyReplyOnAcceptingConnections);
}

int RakNet::RakClientHookInterface::GetAveragePing() noexcept
{
    return this->pOrigInterface->GetAveragePing();
}

int RakNet::RakClientHookInterface::GetLastPing() noexcept
{
    return this->pOrigInterface->GetLastPing();
}

int RakNet::RakClientHookInterface::GetLowestPing() noexcept
{
    return this->pOrigInterface->GetLowestPing();
}

int RakNet::RakClientHookInterface::GetPlayerPing(const PlayerID playerId) noexcept
{
    return this->pOrigInterface->GetPlayerPing(playerId);
}

void RakNet::RakClientHookInterface::StartOccasionalPing() noexcept
{
    this->pOrigInterface->StartOccasionalPing();
}

void RakNet::RakClientHookInterface::StopOccasionalPing() noexcept
{
    this->pOrigInterface->StopOccasionalPing();
}

bool RakNet::RakClientHookInterface::IsConnected() noexcept
{
    return this->pOrigInterface->IsConnected();
}

uint32_t RakNet::RakClientHookInterface::GetSynchronizedRandomInteger() noexcept
{
    return this->pOrigInterface->GetSynchronizedRandomInteger();
}

bool RakNet::RakClientHookInterface::GenerateCompressionLayer(uint32_t inputFrequencyTable[256], const bool inputLayer) noexcept
{
    return this->pOrigInterface->GenerateCompressionLayer(inputFrequencyTable, inputLayer);
}

bool RakNet::RakClientHookInterface::DeleteCompressionLayer(const bool inputLayer) noexcept
{
    return this->pOrigInterface->DeleteCompressionLayer(inputLayer);
}

void RakNet::RakClientHookInterface::RegisterAsRemoteProcedureCall(int* const rpcIdPointer, const RPCFunction rpcHandler) noexcept
{
    this->pOrigInterface->RegisterAsRemoteProcedureCall(rpcIdPointer, rpcHandler);
}

void RakNet::RakClientHookInterface::RegisterClassMemberRPC(int* const rpcIdPointer, void* const rpcHandler) noexcept
{
    this->pOrigInterface->RegisterClassMemberRPC(rpcIdPointer, rpcHandler);
}

void RakNet::RakClientHookInterface::UnregisterAsRemoteProcedureCall(int* const rpcIdPointer) noexcept
{
    this->pOrigInterface->UnregisterAsRemoteProcedureCall(rpcIdPointer);
}

bool RakNet::RakClientHookInterface::RPC(int* const rpcIdPointer, const char* const dataPointer,
    const uint32_t bitLength, const PacketPriority priority, const PacketReliability reliability,
    const char orderingChannel, const bool shiftTimestamp) noexcept
{
    const uint32_t byteLength = (bitLength >> 3) + ((bitLength & 7) == 0 ? 0 : 1);
    BitStream bitStream { (uint8_t*)(dataPointer), byteLength, false };

    bool breakStatus { false };

    for (const auto& rpcCallback : RakNet::rpcCallbacks)
    {
        if (rpcCallback != nullptr && !rpcCallback(*rpcIdPointer, bitStream))
            breakStatus = true;
    }

    if (breakStatus) return true;

    return this->pOrigInterface->RPC(rpcIdPointer, &bitStream, priority,
        reliability, orderingChannel, shiftTimestamp);
}

void RakNet::RakClientHookInterface::SetTrackFrequencyTable(const bool trackFrequencyTable) noexcept
{
    this->pOrigInterface->SetTrackFrequencyTable(trackFrequencyTable);
}

bool RakNet::RakClientHookInterface::GetSendFrequencyTable(uint32_t outputFrequencyTable[256]) noexcept
{
    return this->pOrigInterface->GetSendFrequencyTable(outputFrequencyTable);
}

float RakNet::RakClientHookInterface::GetCompressionRatio() noexcept
{
    return this->pOrigInterface->GetCompressionRatio();
}

float RakNet::RakClientHookInterface::GetDecompressionRatio() noexcept
{
    return this->pOrigInterface->GetDecompressionRatio();
}

void RakNet::RakClientHookInterface::AttachPlugin(void* const messageHandler) noexcept
{
    this->pOrigInterface->AttachPlugin(messageHandler);
}

void RakNet::RakClientHookInterface::DetachPlugin(void* const messageHandler) noexcept
{
    this->pOrigInterface->DetachPlugin(messageHandler);
}

BitStream* RakNet::RakClientHookInterface::GetStaticServerData() noexcept
{
    return this->pOrigInterface->GetStaticServerData();
}

void RakNet::RakClientHookInterface::SetStaticServerData(const char* const dataPointer, const int dataLength) noexcept
{
    this->pOrigInterface->SetStaticServerData(dataPointer, dataLength);
}

BitStream* RakNet::RakClientHookInterface::GetStaticClientData(const PlayerID playerId) noexcept
{
    return this->pOrigInterface->GetStaticClientData(playerId);
}

void RakNet::RakClientHookInterface::SetStaticClientData(const PlayerID playerId,
    const char* const dataPointer, const int dataLength) noexcept
{
    this->pOrigInterface->SetStaticClientData(playerId, dataPointer, dataLength);
}

void RakNet::RakClientHookInterface::SendStaticClientDataToServer() noexcept
{
    this->pOrigInterface->SendStaticClientDataToServer();
}

PlayerID RakNet::RakClientHookInterface::GetServerID() noexcept
{
    return this->pOrigInterface->GetServerID();
}

PlayerID RakNet::RakClientHookInterface::GetPlayerID() noexcept
{
    return this->pOrigInterface->GetPlayerID();
}

PlayerID RakNet::RakClientHookInterface::GetInternalID() noexcept
{
    return this->pOrigInterface->GetInternalID();
}

const char* RakNet::RakClientHookInterface::PlayerIDToDottedIP(const PlayerID playerId) noexcept
{
    return this->pOrigInterface->PlayerIDToDottedIP(playerId);
}

void RakNet::RakClientHookInterface::PushBackPacket(Packet* const packetPointer, const bool pushAtHead) noexcept
{
    this->pOrigInterface->PushBackPacket(packetPointer, pushAtHead);
}

void RakNet::RakClientHookInterface::SetRouterInterface(void* const routerInterface) noexcept
{
    this->pOrigInterface->SetRouterInterface(routerInterface);
}

void RakNet::RakClientHookInterface::RemoveRouterInterface(void* const routerInterface) noexcept
{
    this->pOrigInterface->RemoveRouterInterface(routerInterface);
}

void RakNet::RakClientHookInterface::SetTimeoutTime(const RakNetTime timeMs) noexcept
{
    this->pOrigInterface->SetTimeoutTime(timeMs);
}

bool RakNet::RakClientHookInterface::SetMTUSize(const int mtuSize) noexcept
{
    return this->pOrigInterface->SetMTUSize(mtuSize);
}

int RakNet::RakClientHookInterface::GetMTUSize() noexcept
{
    return this->pOrigInterface->GetMTUSize();
}

void RakNet::RakClientHookInterface::AllowConnectionResponseIPMigration(const bool allowConnectionResponseIpMigration) noexcept
{
    this->pOrigInterface->AllowConnectionResponseIPMigration(allowConnectionResponseIpMigration);
}

void RakNet::RakClientHookInterface::AdvertiseSystem(const char* const hostIp,
    const uint16_t hostPort, const char* const dataPointer, const int dataLength) noexcept
{
    this->pOrigInterface->AdvertiseSystem(hostIp, hostPort, dataPointer, dataLength);
}

RakNetStatisticsStruct* RakNet::RakClientHookInterface::GetStatistics() noexcept
{
    return this->pOrigInterface->GetStatistics();
}

void RakNet::RakClientHookInterface::ApplyNetworkSimulator(const double maxSendBps,
    const uint16_t minExtraPing, const uint16_t extraPingVariance) noexcept
{
    this->pOrigInterface->ApplyNetworkSimulator(maxSendBps, minExtraPing, extraPingVariance);
}

bool RakNet::RakClientHookInterface::IsNetworkSimulatorActive() noexcept
{
    return this->pOrigInterface->IsNetworkSimulatorActive();
}

PlayerIndex RakNet::RakClientHookInterface::GetPlayerIndex() noexcept
{
    return this->pOrigInterface->GetPlayerIndex();
}

bool RakNet::RakClientHookInterface::RPC_(int* const rpcIdPointer, BitStream* const bitStream,
    const PacketPriority priority, const PacketReliability reliability, const char orderingChannel,
    const bool shiftTimestamp, const NetworkID networkId) noexcept
{
    bool breakStatus { false };

    for (const auto& rpcCallback : RakNet::rpcCallbacks)
    {
        if (rpcCallback != nullptr && !rpcCallback(*rpcIdPointer, *bitStream))
            breakStatus = true;
    }

    if (breakStatus) return true;

    return this->pOrigInterface->RPC_(rpcIdPointer, bitStream, priority,
        reliability, orderingChannel, shiftTimestamp, networkId);
}

void __declspec(naked) RakNet::HookSampDestruct() noexcept
{
    static LPVOID retAddr { nullptr };

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

void __declspec(naked) RakNet::HookRaknetInit() noexcept
{
    static LPVOID retAddr { nullptr };
    static RakClientInterface* retIface { nullptr };

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

    retIface = new (std::nothrow) RakClientHookInterface(RakNet::pRakClientInterface);

    if (retIface == nullptr)
    {
        Logger::LogToFile("[err:raknet:hookinit] : failed to load module");
        retIface = RakNet::pRakClientInterface;
        RakNet::Free();
    }
    else
    {
        Logger::LogToFile("[dbg:raknet:hookinit] : module loaded");
        RakNet::loadStatus = true;
    }

    __asm {
        mov esp, ebp
        popad
        mov eax, retIface
        jmp retAddr
    }
}

bool RakNet::initStatus { false };
bool RakNet::loadStatus { false };

bool RakNet::connectStatus { false };

std::vector<RakNet::ConnectCallback> RakNet::connectCallbacks;
std::vector<RakNet::ReceiveCallback> RakNet::receiveCallbacks;
std::vector<RakNet::SendCallback> RakNet::sendCallbacks;
std::vector<RakNet::RpcCallback> RakNet::rpcCallbacks;
std::vector<RakNet::DisconnectCallback> RakNet::disconnectCallbacks;

RakClientInterface* RakNet::pRakClientInterface { nullptr };
RakClientInterface** RakNet::ppRakClientInterface { nullptr };

Memory::JumpHookPtr RakNet::rakClientInitHook { nullptr };
Memory::JumpHookPtr RakNet::sampDestructHook { nullptr };
