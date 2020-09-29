/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "RakNet.h"

static uint32_t rakClientOffset { 0 };

bool RakNet::Init(const AddressesBase& addrBase,
                  ConnectHandlerType&& connectHandler,
                  PacketIncomingHandlerType&& packetIncomingHandler,
                  PacketOutcomingHandlerType&& packetOutcomingHandler,
                  RpcOutcomingHandlerType&& rpcOutcomingHandler,
                  DisconnectHandlerType&& disconnectHandler) noexcept
{
    if (RakNet::initStatus) return false;

    Logger::LogToFile("[dbg:raknet:init] : module initializing...");

    if (*(WORD*)(addrBase.GetRcInitAddr()) == 0x4689) rakClientOffset = *(BYTE*)(addrBase.GetRcInitAddr() + 2);
    else if (*(WORD*)(addrBase.GetRcInitAddr()) == 0x8689) rakClientOffset = *(DWORD*)(addrBase.GetRcInitAddr() + 2);

    Logger::LogToFile("[dbg:raknet:init] : finded rakclient interface offset (value:0x%x)", rakClientOffset);

    if (!(RakNet::sampDestructHook = MakeJumpHook(addrBase.GetSampDestructAddr(), RakNet::SampDestructHookFunc)))
    {
        Logger::LogToFile("[err:raknet:init] : failed to create 'SampDestruct' function hook");
        return false;
    }

    if (!(RakNet::rakClientInitHook = MakeJumpHook(addrBase.GetRcInitAddr(), RakNet::RakClientInitHookFunc)))
    {
        Logger::LogToFile("[err:raknet:init] : failed to create 'RakClientInit' function hook");
        RakNet::sampDestructHook.reset();
        return false;
    }

    RakNet::connectHandler = std::move(connectHandler);
    RakNet::packetIncomingHandler = std::move(packetIncomingHandler);
    RakNet::packetOutcomingHandler = std::move(packetOutcomingHandler);
    RakNet::rpcOutcomingHandler = std::move(rpcOutcomingHandler);
    RakNet::disconnectHandler = std::move(disconnectHandler);

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

bool RakNet::IsConnected() noexcept
{
    return RakNet::connectStatus;
}

bool RakNet::Send(BitStream* bitStream) noexcept
{
    if (!bitStream || !RakNet::connectStatus)
        return false;

    return RakNet::pRakClientInterface->Send(
        bitStream, PacketPriority::MEDIUM_PRIORITY,
        PacketReliability::RELIABLE_ORDERED, '\0'
    );
}

void RakNet::Free() noexcept
{
    if (!RakNet::initStatus) return;

    Logger::LogToFile("[dbg:raknet:free] : module releasing...");

    RakNet::sampDestructHook.reset();
    RakNet::rakClientInitHook.reset();

    if (RakNet::connectStatus && RakNet::disconnectHandler)
        RakNet::disconnectHandler();

    RakNet::connectStatus = false;

    RakNet::connectHandler = nullptr;
    RakNet::packetIncomingHandler = nullptr;
    RakNet::packetOutcomingHandler = nullptr;
    RakNet::rpcOutcomingHandler = nullptr;
    RakNet::disconnectHandler = nullptr;

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

RakNet::RakClientHookInterface::RakClientHookInterface(RakClientInterface* pOrigInterface) noexcept
    : pOrigInterface(pOrigInterface) {}

bool RakNet::RakClientHookInterface::RPC(
    int* const rpcIdPointer,
    BitStream* const parameters,
    const PacketPriority priority,
    const PacketReliability reliability,
    const char orderingChannel,
    const bool shiftTimestamp
) noexcept
{
    if (RakNet::rpcOutcomingHandler && !RakNet::rpcOutcomingHandler(*rpcIdPointer, parameters))
        return true;

    return this->pOrigInterface->RPC(
        rpcIdPointer, parameters, priority,
        reliability, orderingChannel, shiftTimestamp
    );
}

bool RakNet::RakClientHookInterface::Send(
    BitStream* const bitStream,
    const PacketPriority priority,
    const PacketReliability reliability,
    const char orderingChannel
) noexcept
{
    if (RakNet::packetOutcomingHandler && !RakNet::packetOutcomingHandler(bitStream))
        return true;

    return this->pOrigInterface->Send(bitStream, priority, reliability, orderingChannel);
}

Packet* RakNet::RakClientHookInterface::Receive() noexcept
{
    Packet* packetPointer;

    while ((packetPointer = this->pOrigInterface->Receive()) && RakNet::packetIncomingHandler &&
        !RakNet::packetIncomingHandler(packetPointer)) this->pOrigInterface->DeallocatePacket(packetPointer);

    return packetPointer;
}

bool RakNet::RakClientHookInterface::Connect(
    const char* const hostIp,
    const uint16_t serverPort,
    const uint16_t clientPort,
    const uint32_t depreciated,
    const int threadSleepTimer
) noexcept
{
    Logger::LogToFile(
        "[dbg:raknet:client_%p:connect] : connecting to game server '%s:%hu'...",
        this, hostIp, serverPort
    );

    RakNet::connectStatus = this->pOrigInterface->Connect(
        hostIp, serverPort, clientPort,
        depreciated, threadSleepTimer
    );

    if (RakNet::connectStatus)
    {
        Logger::LogToFile("[dbg:raknet:client_%p:connect] : connected", this);

        if (RakNet::connectHandler) RakNet::connectHandler(hostIp, serverPort);
    }

    return RakNet::connectStatus;
}

void RakNet::RakClientHookInterface::Disconnect(
    const uint32_t blockDuration,
    const uint8_t orderingChannel
) noexcept
{
    Logger::LogToFile("[dbg:raknet:client_%p:disconnect] : disconnecting from server...", this);

    if (RakNet::connectStatus)
    {
        if (RakNet::disconnectHandler) RakNet::disconnectHandler();

        RakNet::connectStatus = false;
    }

    this->pOrigInterface->Disconnect(blockDuration, orderingChannel);
}

void RakNet::RakClientHookInterface::InitializeSecurity(
    const char* const privateKeyP,
    const char* const privateKeyQ
) noexcept
{
    this->pOrigInterface->InitializeSecurity(privateKeyP, privateKeyQ);
}

void RakNet::RakClientHookInterface::SetPassword(
    const char* const password
) noexcept
{
    this->pOrigInterface->SetPassword(password);
}

bool RakNet::RakClientHookInterface::HasPassword() noexcept
{
    return this->pOrigInterface->HasPassword();
}

bool RakNet::RakClientHookInterface::Send(
    const char* const dataPointer,
    const int dataLength,
    const PacketPriority priority,
    const PacketReliability reliability,
    const char orderingChannel
) noexcept
{
    BitStream bitStream((uint8_t*)(dataPointer), dataLength, true);

    if (RakNet::packetOutcomingHandler && !RakNet::packetOutcomingHandler(&bitStream))
        return true;

    return this->pOrigInterface->Send(&bitStream, priority, reliability, orderingChannel);
}

void RakNet::RakClientHookInterface::DeallocatePacket(
    Packet* const packetPointer
) noexcept
{
    this->pOrigInterface->DeallocatePacket(packetPointer);
}

void RakNet::RakClientHookInterface::PingServer() noexcept
{
    this->pOrigInterface->PingServer();
}

void RakNet::RakClientHookInterface::PingServer(
    const char* const hostIp,
    const uint16_t serverPort,
    const uint16_t clientPort,
    const bool onlyReplyOnAcceptingConnections
) noexcept
{
    this->pOrigInterface->PingServer(
        hostIp, serverPort, clientPort,
        onlyReplyOnAcceptingConnections
    );
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

int RakNet::RakClientHookInterface::GetPlayerPing(
    const PlayerID playerId
) noexcept
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

bool RakNet::RakClientHookInterface::GenerateCompressionLayer(
    uint32_t inputFrequencyTable[256],
    const bool inputLayer
) noexcept
{
    return this->pOrigInterface->GenerateCompressionLayer(inputFrequencyTable, inputLayer);
}

bool RakNet::RakClientHookInterface::DeleteCompressionLayer(
    const bool inputLayer
) noexcept
{
    return this->pOrigInterface->DeleteCompressionLayer(inputLayer);
}

void RakNet::RakClientHookInterface::RegisterAsRemoteProcedureCall(
    int* const rpcIdPointer,
    const RPCFunction rpcHandler
) noexcept
{
    this->pOrigInterface->RegisterAsRemoteProcedureCall(rpcIdPointer, rpcHandler);
}

void RakNet::RakClientHookInterface::RegisterClassMemberRPC(
    int* const rpcIdPointer,
    void* const rpcHandler
) noexcept
{
    this->pOrigInterface->RegisterClassMemberRPC(rpcIdPointer, rpcHandler);
}

void RakNet::RakClientHookInterface::UnregisterAsRemoteProcedureCall(
    int* const rpcIdPointer
) noexcept
{
    this->pOrigInterface->UnregisterAsRemoteProcedureCall(rpcIdPointer);
}

bool RakNet::RakClientHookInterface::RPC(
    int* const rpcIdPointer,
    const char* const dataPointer,
    const uint32_t bitLength,
    const PacketPriority priority,
    const PacketReliability reliability,
    const char orderingChannel,
    const bool shiftTimestamp
) noexcept
{
    const int byteLength = (bitLength >> 3) + (bitLength & 7 ? 1 : 0);

    BitStream bitStream((uint8_t*)(dataPointer), byteLength, true);

    if (RakNet::rpcOutcomingHandler && !RakNet::rpcOutcomingHandler(*rpcIdPointer, &bitStream))
        return true;

    return this->pOrigInterface->RPC(
        rpcIdPointer, &bitStream, priority,
        reliability, orderingChannel, shiftTimestamp
    );
}

void RakNet::RakClientHookInterface::SetTrackFrequencyTable(
    const bool trackFrequencyTable
) noexcept
{
    this->pOrigInterface->SetTrackFrequencyTable(trackFrequencyTable);
}

bool RakNet::RakClientHookInterface::GetSendFrequencyTable(
    uint32_t outputFrequencyTable[256]
) noexcept
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

void RakNet::RakClientHookInterface::AttachPlugin(
    void* const messageHandler
) noexcept
{
    this->pOrigInterface->AttachPlugin(messageHandler);
}

void RakNet::RakClientHookInterface::DetachPlugin(
    void* const messageHandler
) noexcept
{
    this->pOrigInterface->DetachPlugin(messageHandler);
}

BitStream* RakNet::RakClientHookInterface::GetStaticServerData() noexcept
{
    return this->pOrigInterface->GetStaticServerData();
}

void RakNet::RakClientHookInterface::SetStaticServerData(
    const char* const dataPointer,
    const int dataLength
) noexcept
{
    this->pOrigInterface->SetStaticServerData(dataPointer, dataLength);
}

BitStream* RakNet::RakClientHookInterface::GetStaticClientData(
    const PlayerID playerId
) noexcept
{
    return this->pOrigInterface->GetStaticClientData(playerId);
}

void RakNet::RakClientHookInterface::SetStaticClientData(
    const PlayerID playerId,
    const char* const dataPointer,
    const int dataLength
) noexcept
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

const char* RakNet::RakClientHookInterface::PlayerIDToDottedIP(
    const PlayerID playerId
) noexcept
{
    return this->pOrigInterface->PlayerIDToDottedIP(playerId);
}

void RakNet::RakClientHookInterface::PushBackPacket(
    Packet* const packetPointer,
    const bool pushAtHead
) noexcept
{
    this->pOrigInterface->PushBackPacket(packetPointer, pushAtHead);
}

void RakNet::RakClientHookInterface::SetRouterInterface(
    void* const routerInterface
) noexcept
{
    this->pOrigInterface->SetRouterInterface(routerInterface);
}

void RakNet::RakClientHookInterface::RemoveRouterInterface(
    void* const routerInterface
) noexcept
{
    this->pOrigInterface->RemoveRouterInterface(routerInterface);
}

void RakNet::RakClientHookInterface::SetTimeoutTime(
    const RakNetTime timeMs
) noexcept
{
    this->pOrigInterface->SetTimeoutTime(timeMs);
}

bool RakNet::RakClientHookInterface::SetMTUSize(
    const int mtuSize
) noexcept
{
    return this->pOrigInterface->SetMTUSize(mtuSize);
}

int RakNet::RakClientHookInterface::GetMTUSize() noexcept
{
    return this->pOrigInterface->GetMTUSize();
}

void RakNet::RakClientHookInterface::AllowConnectionResponseIPMigration(
    const bool allowConnectionResponseIpMigration
) noexcept
{
    this->pOrigInterface->AllowConnectionResponseIPMigration(allowConnectionResponseIpMigration);
}

void RakNet::RakClientHookInterface::AdvertiseSystem(
    const char* const hostIp,
    const uint16_t hostPort,
    const char* const dataPointer,
    const int dataLength
) noexcept
{
    this->pOrigInterface->AdvertiseSystem(hostIp, hostPort, dataPointer, dataLength);
}

RakNetStatisticsStruct* RakNet::RakClientHookInterface::GetStatistics() noexcept
{
    return this->pOrigInterface->GetStatistics();
}

void RakNet::RakClientHookInterface::ApplyNetworkSimulator(
    const double maxSendBps,
    const uint16_t minExtraPing,
    const uint16_t extraPingVariance
) noexcept
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

bool RakNet::RakClientHookInterface::RPC_(
    int* const rpcIdPointer,
    BitStream* const bitStream,
    const PacketPriority priority,
    const PacketReliability reliability,
    const char orderingChannel,
    const bool shiftTimestamp,
    const NetworkID networkId
) noexcept
{
    if (RakNet::rpcOutcomingHandler && !RakNet::rpcOutcomingHandler(*rpcIdPointer, bitStream))
        return true;

    return this->pOrigInterface->RPC_(
        rpcIdPointer, bitStream, priority, reliability,
        orderingChannel, shiftTimestamp, networkId
    );
}

void __declspec(naked) RakNet::SampDestructHookFunc() noexcept
{
    static void* retAddr { nullptr };

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

void __declspec(naked) RakNet::RakClientInitHookFunc() noexcept
{
    static void* retAddr { nullptr };
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

    if (retIface = new (std::nothrow) RakClientHookInterface(RakNet::pRakClientInterface))
    {
        Logger::LogToFile("[dbg:raknet:hookinit] : module loaded");
        RakNet::loadStatus = true;
    }
    else
    {
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

bool RakNet::initStatus { false };
bool RakNet::loadStatus { false };
bool RakNet::connectStatus { false };

RakNet::ConnectHandlerType RakNet::connectHandler { nullptr };
RakNet::PacketIncomingHandlerType RakNet::packetIncomingHandler { nullptr };
RakNet::PacketOutcomingHandlerType RakNet::packetOutcomingHandler { nullptr };
RakNet::RpcOutcomingHandlerType RakNet::rpcOutcomingHandler { nullptr };
RakNet::DisconnectHandlerType RakNet::disconnectHandler { nullptr };

RakClientInterface* RakNet::pRakClientInterface { nullptr };
RakClientInterface** RakNet::ppRakClientInterface { nullptr };

Memory::JumpHookPtr RakNet::rakClientInitHook { nullptr };
Memory::JumpHookPtr RakNet::sampDestructHook { nullptr };
