/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "raknet.h"

#include <cassert>

#include "logger.h"

namespace
{
    constexpr uint8_t ConnectRaknetRcpId = 25;

    constexpr const char* GetRakServerInterfaceFuncPattern =
#ifdef _WIN32
        "\x64\xA1\x00\x00\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x51"
        "\x68\x18\x0E\x00\x00\xE8\xFF\xFF\xFF\xFF\x83\xC4\x04\x89\x04"
        "\x24\x85\xC0\xC7\x44\x24\xFF\x00\x00\x00\x00\x74\x16"
#else
        "\x04\x24\xFF\xFF\xFF\xFF\x89\x75\xFF\x89\x5D\xFF\xE8\xFF\xFF"
        "\xFF\xFF\x89\x04\x24\x89\xC6\xE8\xFF\xFF\xFF\xFF\x89\xF0\x8B"
        "\x5D\xFF\x8B\x75\xFF\x89\xEC\x5D\xC3"
#endif
        ;

    constexpr const char* GetRakServerInterfaceFuncMask =
#ifdef _WIN32
        "xxxxxxxxxxxxxxxx????x????xxxxxxxxxxx?xxxxxx"
#else
        "xx????xx?xx?x????xxxxxx????xxxx?xx?xxxx"
#endif
        ;

    struct FunctionOffset
    {
        enum
        {
#ifdef _WIN32
            Start = 1,
            Send = 7,
            Receive = 10,
            Kick = 11,
            DeallocatePacket = 12,
            SetAllowedPlayers = 13,
            GetLastPing = 19,
            RegisterRpc = 29,
            UnregisterRpc = 31,
            Rpc = 32,
            GetLocalIp = 52,
            GetInternalId = 53,
            GetIndexFromPlayerId = 57,
            GetPlayerIdFromIndex = 58,
            AddBan = 60,
            RemoveBan = 61,
            ClearBan = 62,
            SetTimeout = 65
#else
            Start = 2,
            Send = 9,
            Receive = 11,
            Kick = 12,
            DeallocatePacket = 13,
            SetAllowedPlayers = 14,
            GetLastPing = 20,
            RegisterRpc = 30,
            UnregisterRpc = 32,
            Rpc = 35,
            GetLocalIp = 53,
            GetInternalId = 54,
            GetIndexFromPlayerId = 58,
            GetPlayerIdFromIndex = 59,
            AddBan = 61,
            RemoveBan = 62,
            ClearBan = 63,
            SetTimeout = 65
#endif
        };
    };
}

bool RakNet::Init(const void* const serverBaseAddr,
                  ConnectHandlerType&& connectHandler,
                  DisconnectHandlerType&& disconnectHandler,
                  PacketHandlerType&& packetHandler)
{
    assert(serverBaseAddr);

    if (RakNet::initStatus) return false;

    Logger::Log("[dbg:raknet:init] : module initializing...");

    void* moduleAddr { nullptr };
    uint32_t moduleSize { 0 };

    if (!Memory::GetModuleInfo(serverBaseAddr, moduleAddr, moduleSize))
    {
        Logger::Log("[err:raknet:init] : failed to get module info");
        return false;
    }

    auto functionPointer = Memory::Scanner(moduleAddr, moduleSize).Find(GetRakServerInterfaceFuncPattern,
                                                                        GetRakServerInterfaceFuncMask);

    if (!functionPointer)
    {
        Logger::Log("[err:raknet:init] : failed to find 'GetRakServerInterface' function address");
        return false;
    }

    RakNet::getRakServerInterfaceHook = MakeJumpHook(static_cast<uint8_t*>(functionPointer) - 7, RakNet::GetRakServerInterfaceHook);

    Logger::Log("[dbg:raknet:init] : installed hook to 'GetRakServerInterface' function (ptr:%p)", ((uint8_t*)(functionPointer)) - 7);

    if (disconnectHandler)
    {
#ifdef _WIN32
        functionPointer = (void*)(0x046D970);
#else
        functionPointer = (void*)(0x80A51D0);
#endif

        RakNet::disconnectHook = MakeJumpHook(functionPointer, RakNet::DisconnectHook);
    }

    Logger::Log("[dbg:raknet:init] : installed hook to 'OnPlayerDisconnect' function (ptr:%p)", functionPointer);

    RakNet::connectHandler = std::move(connectHandler);
    RakNet::disconnectHandler = std::move(disconnectHandler);
    RakNet::packetHandler = std::move(packetHandler);

    RakNet::loadStatus = false;

    Logger::Log("[dbg:raknet:init] : module initialized");

    RakNet::initStatus = true;

    return true;
}

void RakNet::Free() noexcept
{
    if (!RakNet::initStatus) return;

    Logger::Log("[dbg:raknet:free] : module releasing...");

    if (RakNet::loadStatus)
    {
        const auto vTable = *static_cast<void***>(RakNet::pRakServerInterface);

        if (RakNet::connectHandler)
        {
            if (RakNet::origConnectHandler)
            {
                const uint8_t rpcConnectId { ConnectRaknetRcpId };
                RakNet::UnregisterRpc(&rpcConnectId);
                RakNet::RegisterRpc(&rpcConnectId, RakNet::origConnectHandler);
            }

            RakNet::origConnectHandler = nullptr;

            {
                const Memory::UnprotectScope scope { &vTable[FunctionOffset::RegisterRpc], sizeof(void*) };
                vTable[FunctionOffset::RegisterRpc] = static_cast<void*>(RakNet::rpcFunc);
            }
        }

        if (RakNet::packetHandler)
        {
            const Memory::UnprotectScope scope { &vTable[FunctionOffset::Receive], sizeof(void*) };
            vTable[FunctionOffset::Receive] = static_cast<void*>(RakNet::receiveFunc);
        }

        RakNet::startFunc = nullptr;
        RakNet::sendFunc = nullptr;
        RakNet::receiveFunc = nullptr;
        RakNet::kickFunc = nullptr;
        RakNet::deallocatePacketFunc = nullptr;
        RakNet::setAllowedPlayersFunc = nullptr;
        RakNet::getLastPingFunc = nullptr;
        RakNet::registerAsRpcFunc = nullptr;
        RakNet::unregisterAsRpcFunc = nullptr;
        RakNet::rpcFunc = nullptr;
        RakNet::getLocalIpFunc = nullptr;
        RakNet::getIndexFromPlayerIdFunc = nullptr;
        RakNet::getPlayerIdFromIndexFunc = nullptr;
        RakNet::addToBanListFunc = nullptr;
        RakNet::removeFromBanListFunc = nullptr;
        RakNet::clearBanListFunc = nullptr;
        RakNet::setTimeoutTimeFunc = nullptr;

        RakNet::pRakServerInterface = nullptr;
    }

    RakNet::loadStatus = false;

    for (uint16_t playerId = 0; playerId < MAX_PLAYERS; ++playerId)
    {
        if (RakNet::playerStatus[playerId] && RakNet::disconnectHandler)
            RakNet::disconnectHandler(playerId);

        RakNet::playerStatus[playerId] = false;
    }

    RakNet::connectHandler = nullptr;
    RakNet::disconnectHandler = nullptr;
    RakNet::packetHandler = nullptr;

    RakNet::disconnectHook.reset();
    RakNet::getRakServerInterfaceHook.reset();

    Logger::Log("[dbg:raknet:free] : module released");

    RakNet::initStatus = false;
}

bool RakNet::IsLoaded() noexcept
{
    return RakNet::loadStatus;
}

void RakNet::Process() noexcept
{
    // Rpc's sending...
    {
        const std::unique_lock<std::shared_mutex> lock { RakNet::rpcQueueMutex };

        SendRpcInfo sendRpcInfo;
        
        while (RakNet::rpcQueue.try_pop(sendRpcInfo)) RakNet::Rpc(
            &sendRpcInfo.rpcId, sendRpcInfo.bitStream.get(), PacketPriority::MEDIUM_PRIORITY,
            PacketReliability::RELIABLE_ORDERED, '\0', RakNet::GetPlayerIdFromIndex(sendRpcInfo.playerId),
            sendRpcInfo.playerId == 0xffff, false);
    }

    // Packets sending...
    {
        const std::unique_lock<std::shared_mutex> lock { RakNet::packetQueueMutex };

        SendPacketInfo sendPacketInfo;
        
        while (RakNet::packetQueue.try_pop(sendPacketInfo)) RakNet::Send(
            sendPacketInfo.bitStream.get(), PacketPriority::MEDIUM_PRIORITY, PacketReliability::RELIABLE_ORDERED,
            '\0', RakNet::GetPlayerIdFromIndex(sendPacketInfo.playerId), sendPacketInfo.playerId == 0xffff);
    }

    // Kicking players...
    {
        const std::unique_lock<std::shared_mutex> lock { RakNet::kickQueueMutex };

        uint16_t kickPlayerId;
        
        while (RakNet::kickQueue.try_pop(kickPlayerId))
            RakNet::Kick(RakNet::GetPlayerIdFromIndex(kickPlayerId));
    }
}

bool RakNet::SendRPC(const uint8_t rpcId, const uint16_t playerId, const void* const dataPtr, const int dataSize)
{
    auto bitStream = std::make_shared<BitStream>((uint8_t*)(dataPtr), dataSize, true);

    const std::shared_lock<std::shared_mutex> lock { RakNet::rpcQueueMutex };

    return RakNet::rpcQueue.try_emplace(std::move(bitStream), playerId, rpcId);
}

bool RakNet::SendPacket(const uint8_t packetId, const uint16_t playerId, const void* const dataPtr, const int dataSize)
{
    auto bitStream = std::make_shared<BitStream>(sizeof(packetId) + dataSize);

    bitStream->Write(packetId);
    bitStream->Write(static_cast<const char*>(dataPtr), dataSize);

    const std::shared_lock<std::shared_mutex> lock { RakNet::packetQueueMutex };

    return RakNet::packetQueue.try_emplace(std::move(bitStream), playerId);
}

bool RakNet::KickPlayer(const uint16_t playerId) noexcept
{
    const std::shared_lock<std::shared_mutex> lock { RakNet::kickQueueMutex };

    return RakNet::kickQueue.try_emplace(playerId);
}

// ----------------------------------------------------------------------------

void* RakNet::GetRakServerInterfaceHook() noexcept
{
    RakNet::getRakServerInterfaceHook->Disable();
    const auto pOrigInterface = static_cast<void*(*)()>(RakNet::getRakServerInterfaceHook->GetPatch().memAddr)();
    RakNet::getRakServerInterfaceHook->Enable();

    if (RakNet::pRakServerInterface = pOrigInterface)
    {
        const auto vTable = *static_cast<void***>(pOrigInterface);

        RakNet::startFunc                = static_cast<StartFuncType>                (vTable[FunctionOffset::Start]);
        RakNet::sendFunc                 = static_cast<SendFuncType>                 (vTable[FunctionOffset::Send]);
        RakNet::receiveFunc              = static_cast<ReceiveFuncType>              (vTable[FunctionOffset::Receive]);
        RakNet::kickFunc                 = static_cast<KickFuncType>                 (vTable[FunctionOffset::Kick]);
        RakNet::deallocatePacketFunc     = static_cast<DeallocatePacketFuncType>     (vTable[FunctionOffset::DeallocatePacket]);
        RakNet::setAllowedPlayersFunc    = static_cast<SetAllowedPlayersFuncType>    (vTable[FunctionOffset::SetAllowedPlayers]);
        RakNet::getLastPingFunc          = static_cast<GetLastPingFuncType>          (vTable[FunctionOffset::GetLastPing]);
        RakNet::registerAsRpcFunc        = static_cast<RegisterAsRpcFuncType>        (vTable[FunctionOffset::RegisterRpc]);
        RakNet::unregisterAsRpcFunc      = static_cast<UnregisterAsRpcFuncType>      (vTable[FunctionOffset::UnregisterRpc]);
        RakNet::rpcFunc                  = static_cast<RpcFuncType>                  (vTable[FunctionOffset::Rpc]);
        RakNet::getLocalIpFunc           = static_cast<GetLocalIpFuncType>           (vTable[FunctionOffset::GetLocalIp]);
        RakNet::getInternalIdFunc        = static_cast<GetInternalIdFuncType>        (vTable[FunctionOffset::GetInternalId]);
        RakNet::getIndexFromPlayerIdFunc = static_cast<GetIndexFromPlayerIdFuncType> (vTable[FunctionOffset::GetIndexFromPlayerId]);
        RakNet::getPlayerIdFromIndexFunc = static_cast<GetPlayerIdFromIndexFuncType> (vTable[FunctionOffset::GetPlayerIdFromIndex]);
        RakNet::addToBanListFunc         = static_cast<AddToBanListFuncType>         (vTable[FunctionOffset::AddBan]);
        RakNet::removeFromBanListFunc    = static_cast<RemoveFromBanListFuncType>    (vTable[FunctionOffset::RemoveBan]);
        RakNet::clearBanListFunc         = static_cast<ClearBanListFuncType>         (vTable[FunctionOffset::ClearBan]);
        RakNet::setTimeoutTimeFunc       = static_cast<SetTimeoutTimeFuncType>       (vTable[FunctionOffset::SetTimeout]);

        if (RakNet::connectHandler)
        {
            const Memory::UnprotectScope scope { &vTable[FunctionOffset::RegisterRpc], sizeof(void*) };
            vTable[FunctionOffset::RegisterRpc] = static_cast<void*>(RakNet::RegisterRpcHook);
        }

        if (RakNet::packetHandler)
        {
            const Memory::UnprotectScope scope { &vTable[FunctionOffset::Receive], sizeof(void*) };
            vTable[FunctionOffset::Receive] = static_cast<void*>(RakNet::ReceiveHook);
        }

        RakNet::loadStatus = true;
    }

    return pOrigInterface;
}

void RakNet::ConnectHook(RPCParameters* const rpc) noexcept
{
    const auto sender = RakNet::GetIndexFromPlayerId(rpc->sender);

    if (sender >= 0 && sender < MAX_PLAYERS)
    {
        if (RakNet::playerStatus[sender] && RakNet::disconnectHandler)
            RakNet::disconnectHandler(sender);

        RakNet::playerStatus[sender] = true;
        RakNet::connectHandler(sender, rpc);
    }

    RakNet::origConnectHandler(rpc);
}

void THISCALL RakNet::RegisterRpcHook(void* const _this, uint8_t* const rpcIdPointer, RPCFunction rpcHandler) noexcept
{
    if (*rpcIdPointer == ConnectRaknetRcpId)
    {
        RakNet::origConnectHandler = rpcHandler;
        rpcHandler = RakNet::ConnectHook;
    }

    RakNet::RegisterRpc(rpcIdPointer, rpcHandler);
}

int THISCALL RakNet::DisconnectHook(void* const _this, int playerId, int reason) noexcept
{
    if (playerId >= 0 && playerId < MAX_PLAYERS)
    {
        if (RakNet::playerStatus[playerId])
            RakNet::disconnectHandler(playerId);
        RakNet::playerStatus[playerId] = false;
    }

    RakNet::disconnectHook->Disable();
    const auto returnStatus = static_cast<int(THISCALL*)(void*, int, int)>
        (RakNet::disconnectHook->GetPatch().memAddr)(_this, playerId, reason);
    RakNet::disconnectHook->Enable();

    return returnStatus;
}

Packet* THISCALL RakNet::ReceiveHook(void* const _this) noexcept
{
    Packet* packet;

    while ((packet = RakNet::Receive()) && !RakNet::packetHandler(packet->playerIndex, packet))
        RakNet::DeallocatePacket(packet);

    return packet;
}

// ----------------------------------------------------------------------------

bool RakNet::initStatus { false };
bool RakNet::loadStatus { false };

void* RakNet::pRakServerInterface { nullptr };

RakNet::ConnectHandlerType RakNet::connectHandler { nullptr };
RakNet::DisconnectHandlerType RakNet::disconnectHandler { nullptr };
RakNet::PacketHandlerType RakNet::packetHandler { nullptr };

std::array<bool, MAX_PLAYERS> RakNet::playerStatus {};

RPCFunction RakNet::origConnectHandler { nullptr };

Memory::JumpHookPtr RakNet::disconnectHook { nullptr };
Memory::JumpHookPtr RakNet::getRakServerInterfaceHook { nullptr };

// ----------------------------------------------------------------------------

std::shared_mutex RakNet::rpcQueueMutex;
MPMCQueue<RakNet::SendRpcInfo> RakNet::rpcQueue { 16 * MAX_PLAYERS };

std::shared_mutex RakNet::packetQueueMutex;
MPMCQueue<RakNet::SendPacketInfo> RakNet::packetQueue { 16 * MAX_PLAYERS };

std::shared_mutex RakNet::kickQueueMutex;
MPMCQueue<uint16_t> RakNet::kickQueue { MAX_PLAYERS };

// ----------------------------------------------------------------------------

bool RakNet::Start(const uint16_t allowedPlayers, const uint32_t depreciated,
                   const int32_t threadSleepTimer, const uint16_t port,
                   const char* const host) noexcept
{
    if (!RakNet::loadStatus) return false;
    if (!RakNet::startFunc) return false;

    return RakNet::startFunc(RakNet::pRakServerInterface, allowedPlayers,
                             depreciated, threadSleepTimer, port, host);
}

bool RakNet::Send(BitStream* const parameters, const PacketPriority priority,
                  const PacketReliability reliability, const uint32_t orderingChannel,
                  const PlayerID playerId, const bool broadcast) noexcept
{
    if (!RakNet::loadStatus) return false;
    if (!RakNet::sendFunc) return false;

    return RakNet::sendFunc(RakNet::pRakServerInterface, parameters, priority,
                            reliability, orderingChannel, playerId, broadcast);
}

Packet* RakNet::Receive() noexcept
{
    if (!RakNet::loadStatus) return nullptr;
    if (!RakNet::receiveFunc) return nullptr;

    return RakNet::receiveFunc(RakNet::pRakServerInterface);
}

void RakNet::Kick(const PlayerID playerId) noexcept
{
    if (!RakNet::loadStatus) return;
    if (!RakNet::kickFunc) return;

    RakNet::kickFunc(RakNet::pRakServerInterface, playerId);
}

void RakNet::DeallocatePacket(Packet* const packet) noexcept
{
    if (!RakNet::loadStatus) return;
    if (!RakNet::deallocatePacketFunc) return;

    RakNet::deallocatePacketFunc(RakNet::pRakServerInterface, packet);
}

void RakNet::SetAllowedPlayers(const uint16_t numberAllowed) noexcept
{
    if (!RakNet::loadStatus) return;
    if (!RakNet::setAllowedPlayersFunc) return;

    return RakNet::setAllowedPlayersFunc(RakNet::pRakServerInterface, numberAllowed);
}

int RakNet::GetLastPing(const PlayerID playerId) noexcept
{
    if (!RakNet::loadStatus) return -1;
    if (!RakNet::getLastPingFunc) return -1;

    return RakNet::getLastPingFunc(RakNet::pRakServerInterface, playerId);
}

void RakNet::RegisterRpc(const uint8_t* const rpcIdPointer, const RPCFunction rpcHandler) noexcept
{
    if (!RakNet::loadStatus) return;
    if (!RakNet::registerAsRpcFunc) return;

    RakNet::registerAsRpcFunc(RakNet::pRakServerInterface, rpcIdPointer, rpcHandler);
}

void RakNet::UnregisterRpc(const uint8_t* const rpcIdPointer) noexcept
{
    if (!RakNet::loadStatus) return;
    if (!RakNet::unregisterAsRpcFunc) return;

    RakNet::unregisterAsRpcFunc(RakNet::pRakServerInterface, rpcIdPointer);
}

bool RakNet::Rpc(const uint8_t* const rpcIdPointer, BitStream* const parameters,
                 const PacketPriority priority, const PacketReliability reliability,
                 const uint32_t orderingChannel, const PlayerID playerId,
                 const bool broadcast, const bool shiftTimestamp) noexcept
{
    if (!RakNet::loadStatus) return false;
    if (!RakNet::rpcFunc) return false;

    return RakNet::rpcFunc(RakNet::pRakServerInterface, rpcIdPointer, parameters,
                           priority, reliability, orderingChannel, playerId,
                           broadcast, shiftTimestamp);
}

const char* RakNet::GetLocalIp(const uint32_t index) noexcept
{
    if (!RakNet::loadStatus) return nullptr;
    if (!RakNet::getLocalIpFunc) return nullptr;

    return RakNet::getLocalIpFunc(RakNet::pRakServerInterface, index);
}

PlayerID RakNet::GetInternalId() noexcept
{
    if (!RakNet::loadStatus) return UNASSIGNED_PLAYER_ID;
    if (!RakNet::getInternalIdFunc) return UNASSIGNED_PLAYER_ID;

    return RakNet::getInternalIdFunc(RakNet::pRakServerInterface);
}

int RakNet::GetIndexFromPlayerId(const PlayerID playerId) noexcept
{
    if (!RakNet::loadStatus) return -1;
    if (!RakNet::getIndexFromPlayerIdFunc) return -1;

    return RakNet::getIndexFromPlayerIdFunc(RakNet::pRakServerInterface, playerId);
}

PlayerID RakNet::GetPlayerIdFromIndex(const int32_t index) noexcept
{
    if (!RakNet::loadStatus) return UNASSIGNED_PLAYER_ID;
    if (!RakNet::getPlayerIdFromIndexFunc) return UNASSIGNED_PLAYER_ID;

    return RakNet::getPlayerIdFromIndexFunc(RakNet::pRakServerInterface, index);
}

void RakNet::AddToBanList(const char* const ip, const uint32_t milliseconds) noexcept
{
    if (!RakNet::loadStatus) return;
    if (!RakNet::addToBanListFunc) return;

    RakNet::addToBanListFunc(RakNet::pRakServerInterface, ip, milliseconds);
}

void RakNet::RemoveFromBanList(const char* const ip) noexcept
{
    if (!RakNet::loadStatus) return;
    if (!RakNet::removeFromBanListFunc) return;

    RakNet::removeFromBanListFunc(RakNet::pRakServerInterface, ip);
}

void RakNet::ClearBanList() noexcept
{
    if (!RakNet::loadStatus) return;
    if (!RakNet::clearBanListFunc) return;

    RakNet::clearBanListFunc(RakNet::pRakServerInterface);
}

void RakNet::SetTimeoutTime(const RakNetTime timeMs, const PlayerID target) noexcept
{
    if (!RakNet::loadStatus) return;
    if (!RakNet::setTimeoutTimeFunc) return;

    RakNet::setTimeoutTimeFunc(RakNet::pRakServerInterface, timeMs, target);
}

// ----------------------------------------------------------------------------

RakNet::StartFuncType                RakNet::startFunc                { nullptr };
RakNet::SendFuncType                 RakNet::sendFunc                 { nullptr };
RakNet::ReceiveFuncType              RakNet::receiveFunc              { nullptr };
RakNet::KickFuncType                 RakNet::kickFunc                 { nullptr };
RakNet::DeallocatePacketFuncType     RakNet::deallocatePacketFunc     { nullptr };
RakNet::SetAllowedPlayersFuncType    RakNet::setAllowedPlayersFunc    { nullptr };
RakNet::GetLastPingFuncType          RakNet::getLastPingFunc          { nullptr };
RakNet::RegisterAsRpcFuncType        RakNet::registerAsRpcFunc        { nullptr };
RakNet::UnregisterAsRpcFuncType      RakNet::unregisterAsRpcFunc      { nullptr };
RakNet::RpcFuncType                  RakNet::rpcFunc                  { nullptr };
RakNet::GetLocalIpFuncType           RakNet::getLocalIpFunc           { nullptr };
RakNet::GetInternalIdFuncType        RakNet::getInternalIdFunc        { nullptr };
RakNet::GetIndexFromPlayerIdFuncType RakNet::getIndexFromPlayerIdFunc { nullptr };
RakNet::GetPlayerIdFromIndexFuncType RakNet::getPlayerIdFromIndexFunc { nullptr };
RakNet::AddToBanListFuncType         RakNet::addToBanListFunc         { nullptr };
RakNet::RemoveFromBanListFuncType    RakNet::removeFromBanListFunc    { nullptr };
RakNet::ClearBanListFuncType         RakNet::clearBanListFunc         { nullptr };
RakNet::SetTimeoutTimeFuncType       RakNet::setTimeoutTimeFunc       { nullptr };
