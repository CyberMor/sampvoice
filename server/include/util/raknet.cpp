/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "raknet.h"

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

    constexpr auto OnPlayerDisconnectAddr =
#ifdef _WIN32
        0x046D970
#else
        0x80A51D0
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

bool RakNet::Init(const void* const serverBaseAddr) noexcept
{
    if (RakNet::initStatus) return false;

    Logger::Log("[dbg:raknet:init] : module initializing...");

    Memory::addr_t moduleAddr { nullptr };
    Memory::size_t moduleSize { 0 };

    if (!Memory::GetModuleInfo(serverBaseAddr, moduleAddr, moduleSize))
    {
        Logger::Log("[err:raknet:init] : failed to get module info");
        return false;
    }

    const auto functionPointer = Memory::Scanner(moduleAddr, moduleSize).Find(GetRakServerInterfaceFuncPattern,
                                                                              GetRakServerInterfaceFuncMask);

    if (functionPointer == nullptr)
    {
        Logger::Log("[err:raknet:init] : failed to find 'GetRakServerInterface' function address");
        return false;
    }

    try
    {
        RakNet::hookGetRakServerInterface = MakeJumpHook(static_cast<uint8_t*>(functionPointer) - 7,
                                                         RakNet::GetRakServerInterfaceHook);
        RakNet::hookDisconnect = MakeJumpHook(OnPlayerDisconnectAddr, RakNet::DisconnectHook);
    }
    catch (const std::exception& exception)
    {
        Logger::Log("[err:raknet:init] : failed to create function hooks");
        RakNet::hookGetRakServerInterface.reset();
        RakNet::hookDisconnect.reset();
        return false;
    }

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

        if (RakNet::origConnectHandler != nullptr)
        {
            const uint8_t rpcConnectId { ConnectRaknetRcpId };
            RakNet::UnregisterRpc(&rpcConnectId);
            RakNet::RegisterRpc(&rpcConnectId, RakNet::origConnectHandler);
        }

        RakNet::origConnectHandler = nullptr;

        {
            const Memory::UnprotectScope scope { &vTable[FunctionOffset::RegisterRpc], sizeof(void*) };
            vTable[FunctionOffset::RegisterRpc] = reinterpret_cast<void*>(RakNet::rpcFunc);
        }
        
        {
            const Memory::UnprotectScope scope { &vTable[FunctionOffset::Receive], sizeof(void*) };
            vTable[FunctionOffset::Receive] = reinterpret_cast<void*>(RakNet::receiveFunc);
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

    for (uint16_t playerId { 0 }; playerId < MAX_PLAYERS; ++playerId)
    {
        if (RakNet::playerStatus[playerId])
        {
            for (const auto& disconnectCallback : RakNet::disconnectCallbacks)
            {
                if (disconnectCallback != nullptr) disconnectCallback(playerId);
            }
        }

        RakNet::playerStatus[playerId] = false;
    }

    RakNet::connectCallbacks.clear();
    RakNet::packetCallbacks.clear();
    RakNet::disconnectCallbacks.clear();

    RakNet::hookDisconnect.reset();
    RakNet::hookGetRakServerInterface.reset();

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
    auto bitStream = std::make_unique<BitStream>((uint8_t*)(dataPtr), dataSize, true);

    const std::shared_lock<std::shared_mutex> lock { RakNet::rpcQueueMutex };

    return RakNet::rpcQueue.try_emplace(std::move(bitStream), playerId, rpcId);
}

bool RakNet::SendPacket(const uint8_t packetId, const uint16_t playerId, const void* const dataPtr, const int dataSize)
{
    auto bitStream = std::make_unique<BitStream>(sizeof(packetId) + dataSize);

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

std::size_t RakNet::AddConnectCallback(ConnectCallback callback) noexcept
{
    if (!RakNet::initStatus) return -1;

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

std::size_t RakNet::AddPacketCallback(PacketCallback callback) noexcept
{
    if (!RakNet::initStatus) return -1;

    for (std::size_t i { 0 }; i < RakNet::packetCallbacks.size(); ++i)
    {
        if (RakNet::packetCallbacks[i] == nullptr)
        {
            RakNet::packetCallbacks[i] = std::move(callback);
            return i;
        }
    }

    RakNet::packetCallbacks.emplace_back(std::move(callback));
    return RakNet::packetCallbacks.size() - 1;
}

std::size_t RakNet::AddDisconnectCallback(DisconnectCallback callback) noexcept
{
    if (!RakNet::initStatus) return -1;

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
    if (!RakNet::initStatus) return;

    if (callback >= RakNet::connectCallbacks.size())
        return;

    RakNet::connectCallbacks[callback] = nullptr;
}

void RakNet::RemovePacketCallback(const std::size_t callback) noexcept
{
    if (!RakNet::initStatus) return;

    if (callback >= RakNet::packetCallbacks.size())
        return;

    RakNet::packetCallbacks[callback] = nullptr;
}

void RakNet::RemoveDisconnectCallback(const std::size_t callback) noexcept
{
    if (!RakNet::initStatus) return;

    if (callback >= RakNet::disconnectCallbacks.size())
        return;

    RakNet::disconnectCallbacks[callback] = nullptr;
}

// ----------------------------------------------------------------------------

void* RakNet::GetRakServerInterfaceHook() noexcept
{
    RakNet::hookGetRakServerInterface->Disable();
    const auto pOrigInterface = reinterpret_cast<void*(*)()>(RakNet::hookGetRakServerInterface->GetPatch().memAddr)();
    RakNet::hookGetRakServerInterface->Enable();

    if (pOrigInterface != nullptr)
    {
        const auto vTable = *static_cast<void***>(pOrigInterface);

        RakNet::startFunc                = reinterpret_cast<StartFuncType>                (vTable[FunctionOffset::Start]);
        RakNet::sendFunc                 = reinterpret_cast<SendFuncType>                 (vTable[FunctionOffset::Send]);
        RakNet::receiveFunc              = reinterpret_cast<ReceiveFuncType>              (vTable[FunctionOffset::Receive]);
        RakNet::kickFunc                 = reinterpret_cast<KickFuncType>                 (vTable[FunctionOffset::Kick]);
        RakNet::deallocatePacketFunc     = reinterpret_cast<DeallocatePacketFuncType>     (vTable[FunctionOffset::DeallocatePacket]);
        RakNet::setAllowedPlayersFunc    = reinterpret_cast<SetAllowedPlayersFuncType>    (vTable[FunctionOffset::SetAllowedPlayers]);
        RakNet::getLastPingFunc          = reinterpret_cast<GetLastPingFuncType>          (vTable[FunctionOffset::GetLastPing]);
        RakNet::registerAsRpcFunc        = reinterpret_cast<RegisterAsRpcFuncType>        (vTable[FunctionOffset::RegisterRpc]);
        RakNet::unregisterAsRpcFunc      = reinterpret_cast<UnregisterAsRpcFuncType>      (vTable[FunctionOffset::UnregisterRpc]);
        RakNet::rpcFunc                  = reinterpret_cast<RpcFuncType>                  (vTable[FunctionOffset::Rpc]);
        RakNet::getLocalIpFunc           = reinterpret_cast<GetLocalIpFuncType>           (vTable[FunctionOffset::GetLocalIp]);
        RakNet::getInternalIdFunc        = reinterpret_cast<GetInternalIdFuncType>        (vTable[FunctionOffset::GetInternalId]);
        RakNet::getIndexFromPlayerIdFunc = reinterpret_cast<GetIndexFromPlayerIdFuncType> (vTable[FunctionOffset::GetIndexFromPlayerId]);
        RakNet::getPlayerIdFromIndexFunc = reinterpret_cast<GetPlayerIdFromIndexFuncType> (vTable[FunctionOffset::GetPlayerIdFromIndex]);
        RakNet::addToBanListFunc         = reinterpret_cast<AddToBanListFuncType>         (vTable[FunctionOffset::AddBan]);
        RakNet::removeFromBanListFunc    = reinterpret_cast<RemoveFromBanListFuncType>    (vTable[FunctionOffset::RemoveBan]);
        RakNet::clearBanListFunc         = reinterpret_cast<ClearBanListFuncType>         (vTable[FunctionOffset::ClearBan]);
        RakNet::setTimeoutTimeFunc       = reinterpret_cast<SetTimeoutTimeFuncType>       (vTable[FunctionOffset::SetTimeout]);

        {
            const Memory::UnprotectScope scope { &vTable[FunctionOffset::RegisterRpc], sizeof(void*) };
            vTable[FunctionOffset::RegisterRpc] = reinterpret_cast<void*>(RakNet::RegisterRpcHook);
        }

        {
            const Memory::UnprotectScope scope { &vTable[FunctionOffset::Receive], sizeof(void*) };
            vTable[FunctionOffset::Receive] = reinterpret_cast<void*>(RakNet::ReceiveHook);
        }

        RakNet::pRakServerInterface = pOrigInterface;
        RakNet::loadStatus = true;
    }

    return pOrigInterface;
}

void RakNet::ConnectHook(RPCParameters* const rpc) noexcept
{
    const auto sender = RakNet::GetIndexFromPlayerId(rpc->sender);

    if (sender >= 0 && sender < MAX_PLAYERS)
    {
        if (RakNet::playerStatus[sender])
        {
            for (const auto& disconnectCallback : RakNet::disconnectCallbacks)
            {
                if (disconnectCallback != nullptr) disconnectCallback(sender);
            }
        }

        RakNet::playerStatus[sender] = true;

        for (const auto& connectCallback : RakNet::connectCallbacks)
        {
            if (connectCallback != nullptr) connectCallback(sender, *rpc);
        }
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

int THISCALL RakNet::DisconnectHook(void* const _this, const int playerId, const int reason) noexcept
{
    if (playerId >= 0 && playerId < MAX_PLAYERS)
    {
        if (RakNet::playerStatus[playerId])
        {
            for (const auto& disconnectCallback : RakNet::disconnectCallbacks)
            {
                if (disconnectCallback != nullptr) disconnectCallback(playerId);
            }
        }

        RakNet::playerStatus[playerId] = false;
    }

    RakNet::hookDisconnect->Disable();
    const auto returnStatus = reinterpret_cast<int(THISCALL*)(void*, int, int)>
        (RakNet::hookDisconnect->GetPatch().memAddr)(_this, playerId, reason);
    RakNet::hookDisconnect->Enable();

    return returnStatus;
}

Packet* THISCALL RakNet::ReceiveHook(void* const _this) noexcept
{
    Packet* packet;

    while ((packet = RakNet::Receive()) != nullptr)
    {
        bool breakStatus { true };

        for (const auto& packetCallback : RakNet::packetCallbacks)
        {
            if (packetCallback != nullptr && !packetCallback(packet->playerIndex, *packet))
                breakStatus = false;
        }

        if (breakStatus) break;

        RakNet::DeallocatePacket(packet);
    }

    return packet;
}

// ----------------------------------------------------------------------------

bool RakNet::initStatus { false };
bool RakNet::loadStatus { false };

void* RakNet::pRakServerInterface { nullptr };

std::vector<RakNet::ConnectCallback> RakNet::connectCallbacks;
std::vector<RakNet::PacketCallback> RakNet::packetCallbacks;
std::vector<RakNet::DisconnectCallback> RakNet::disconnectCallbacks;

std::array<bool, MAX_PLAYERS> RakNet::playerStatus {};

RPCFunction RakNet::origConnectHandler { nullptr };

Memory::JumpHookPtr RakNet::hookDisconnect { nullptr };
Memory::JumpHookPtr RakNet::hookGetRakServerInterface { nullptr };

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
    if (RakNet::startFunc == nullptr) return false;

    return RakNet::startFunc(RakNet::pRakServerInterface, allowedPlayers,
                             depreciated, threadSleepTimer, port, host);
}

bool RakNet::Send(BitStream* const parameters, const PacketPriority priority,
                  const PacketReliability reliability, const uint32_t orderingChannel,
                  const PlayerID playerId, const bool broadcast) noexcept
{
    if (!RakNet::loadStatus) return false;
    if (RakNet::sendFunc == nullptr) return false;

    return RakNet::sendFunc(RakNet::pRakServerInterface, parameters, priority,
                            reliability, orderingChannel, playerId, broadcast);
}

Packet* RakNet::Receive() noexcept
{
    if (!RakNet::loadStatus) return nullptr;
    if (RakNet::receiveFunc == nullptr) return nullptr;

    return RakNet::receiveFunc(RakNet::pRakServerInterface);
}

void RakNet::Kick(const PlayerID playerId) noexcept
{
    if (!RakNet::loadStatus) return;
    if (RakNet::kickFunc == nullptr) return;

    RakNet::kickFunc(RakNet::pRakServerInterface, playerId);
}

void RakNet::DeallocatePacket(Packet* const packet) noexcept
{
    if (!RakNet::loadStatus) return;
    if (RakNet::deallocatePacketFunc == nullptr) return;

    RakNet::deallocatePacketFunc(RakNet::pRakServerInterface, packet);
}

void RakNet::SetAllowedPlayers(const uint16_t numberAllowed) noexcept
{
    if (!RakNet::loadStatus) return;
    if (RakNet::setAllowedPlayersFunc == nullptr) return;

    return RakNet::setAllowedPlayersFunc(RakNet::pRakServerInterface, numberAllowed);
}

int RakNet::GetLastPing(const PlayerID playerId) noexcept
{
    if (!RakNet::loadStatus) return -1;
    if (RakNet::getLastPingFunc == nullptr) return -1;

    return RakNet::getLastPingFunc(RakNet::pRakServerInterface, playerId);
}

void RakNet::RegisterRpc(const uint8_t* const rpcIdPointer, const RPCFunction rpcHandler) noexcept
{
    if (!RakNet::loadStatus) return;
    if (RakNet::registerAsRpcFunc == nullptr) return;

    RakNet::registerAsRpcFunc(RakNet::pRakServerInterface, rpcIdPointer, rpcHandler);
}

void RakNet::UnregisterRpc(const uint8_t* const rpcIdPointer) noexcept
{
    if (!RakNet::loadStatus) return;
    if (RakNet::unregisterAsRpcFunc == nullptr) return;

    RakNet::unregisterAsRpcFunc(RakNet::pRakServerInterface, rpcIdPointer);
}

bool RakNet::Rpc(const uint8_t* const rpcIdPointer, BitStream* const parameters,
                 const PacketPriority priority, const PacketReliability reliability,
                 const uint32_t orderingChannel, const PlayerID playerId,
                 const bool broadcast, const bool shiftTimestamp) noexcept
{
    if (!RakNet::loadStatus) return false;
    if (RakNet::rpcFunc == nullptr) return false;

    return RakNet::rpcFunc(RakNet::pRakServerInterface, rpcIdPointer, parameters,
                           priority, reliability, orderingChannel, playerId,
                           broadcast, shiftTimestamp);
}

const char* RakNet::GetLocalIp(const uint32_t index) noexcept
{
    if (!RakNet::loadStatus) return nullptr;
    if (RakNet::getLocalIpFunc == nullptr) return nullptr;

    return RakNet::getLocalIpFunc(RakNet::pRakServerInterface, index);
}

PlayerID RakNet::GetInternalId() noexcept
{
    if (!RakNet::loadStatus) return UNASSIGNED_PLAYER_ID;
    if (RakNet::getInternalIdFunc == nullptr) return UNASSIGNED_PLAYER_ID;

    return RakNet::getInternalIdFunc(RakNet::pRakServerInterface);
}

int RakNet::GetIndexFromPlayerId(const PlayerID playerId) noexcept
{
    if (!RakNet::loadStatus) return -1;
    if (RakNet::getIndexFromPlayerIdFunc == nullptr) return -1;

    return RakNet::getIndexFromPlayerIdFunc(RakNet::pRakServerInterface, playerId);
}

PlayerID RakNet::GetPlayerIdFromIndex(const int32_t index) noexcept
{
    if (!RakNet::loadStatus) return UNASSIGNED_PLAYER_ID;
    if (RakNet::getPlayerIdFromIndexFunc == nullptr) return UNASSIGNED_PLAYER_ID;

    return RakNet::getPlayerIdFromIndexFunc(RakNet::pRakServerInterface, index);
}

void RakNet::AddToBanList(const char* const ip, const uint32_t milliseconds) noexcept
{
    if (!RakNet::loadStatus) return;
    if (RakNet::addToBanListFunc == nullptr) return;

    RakNet::addToBanListFunc(RakNet::pRakServerInterface, ip, milliseconds);
}

void RakNet::RemoveFromBanList(const char* const ip) noexcept
{
    if (!RakNet::loadStatus) return;
    if (RakNet::removeFromBanListFunc == nullptr) return;

    RakNet::removeFromBanListFunc(RakNet::pRakServerInterface, ip);
}

void RakNet::ClearBanList() noexcept
{
    if (!RakNet::loadStatus) return;
    if (RakNet::clearBanListFunc == nullptr) return;

    RakNet::clearBanListFunc(RakNet::pRakServerInterface);
}

void RakNet::SetTimeoutTime(const RakNetTime timeMs, const PlayerID target) noexcept
{
    if (!RakNet::loadStatus) return;
    if (RakNet::setTimeoutTimeFunc == nullptr) return;

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
