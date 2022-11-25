/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "RakNet.h"

#include "Logger.h"

static DWORD rak_client_offset = 0;

bool RakNet::Init(const AddressesBase& addr_base) noexcept
{
    if (_init_status) return false;

    Logger::LogToFile("[dbg:raknet:init] : module initializing...");

    if (*reinterpret_cast<WORD*>(addr_base.GetRcInitAddr()) == 0x4689)
    {
        rak_client_offset = *reinterpret_cast<BYTE*>(addr_base.GetRcInitAddr() + 2);
    }
    if (*reinterpret_cast<WORD*>(addr_base.GetRcInitAddr()) == 0x8689)
    {
        rak_client_offset = *reinterpret_cast<DWORD*>(addr_base.GetRcInitAddr() + 2);
    }

    Logger::LogToFile("[dbg:raknet:init] : finded rakclient interface offset (value:0x%x)", rak_client_offset);

    _samp_destruct_hook   = Memory::JumpHook((LPVOID)(addr_base.GetSampDestructAddr()), &HookSampDestruct);
    _rak_client_init_hook = Memory::JumpHook((LPVOID)(addr_base.GetRcInitAddr()), &HookRaknetInit);

    _connect_callbacks.clear();
    _receive_callbacks.clear();
    _send_callbacks.clear();
    _rpc_callbacks.clear();
    _disconnect_callbacks.clear();

    _connect_status = false;
    _load_status = false;

    Logger::LogToFile("[dbg:raknet:init] : module initialized");

    _init_status = true;

    return true;
}

bool RakNet::IsInited() noexcept
{
    return _init_status;
}

bool RakNet::IsLoaded() noexcept
{
    return _load_status;
}

void RakNet::Free() noexcept
{
    if (_init_status)
    {
        Logger::LogToFile("[dbg:raknet:free] : module releasing...");

        _samp_destruct_hook   = {};
        _rak_client_init_hook = {};

        if (_connect_status)
        {
            for (const auto& disconnect_callback : _disconnect_callbacks)
            {
                if (disconnect_callback != nullptr) disconnect_callback();
            }
        }

        _connect_status = false;

        _connect_callbacks.clear();
        _receive_callbacks.clear();
        _send_callbacks.clear();
        _rpc_callbacks.clear();
        _disconnect_callbacks.clear();

        if (_load_status)
        {
            const auto rak_client_interface_hook = reinterpret_cast<RakClientHookInterface*>(*_rak_client_interface_ptr);
            *_rak_client_interface_ptr = _rak_client_interface;
            delete rak_client_interface_hook;
        }

        _load_status = false;

        _rak_client_interface_ptr = nullptr;
        _rak_client_interface     = nullptr;

        Logger::LogToFile("[dbg:raknet:free] : module released");

        _init_status = false;
    }
}

bool RakNet::IsConnected() noexcept
{
    return _connect_status;
}

bool RakNet::Send(BitStream* const bit_stream) noexcept
{
    return bit_stream != nullptr && _connect_status && _rak_client_interface->Send
        (bit_stream, PacketPriority::MEDIUM_PRIORITY, PacketReliability::RELIABLE_ORDERED, '\0');
}

std::size_t RakNet::AddConnectCallback(ConnectCallback callback) noexcept
{
    if (!_init_status) return -1;

    for (std::size_t i = 0; i < _connect_callbacks.size(); ++i)
    {
        if (_connect_callbacks[i] == nullptr)
        {
            _connect_callbacks[i] = std::move(callback);
            return i;
        }
    }

    _connect_callbacks.emplace_back(std::move(callback));
    return _connect_callbacks.size() - 1;
}

std::size_t RakNet::AddReceiveCallback(ReceiveCallback callback) noexcept
{
    if (!_init_status) return -1;

    for (std::size_t i = 0; i < _receive_callbacks.size(); ++i)
    {
        if (_receive_callbacks[i] == nullptr)
        {
            _receive_callbacks[i] = std::move(callback);
            return i;
        }
    }

    _receive_callbacks.emplace_back(std::move(callback));
    return _receive_callbacks.size() - 1;
}

std::size_t RakNet::AddSendCallback(SendCallback callback) noexcept
{
    if (!_init_status) return -1;

    for (std::size_t i = 0; i < _send_callbacks.size(); ++i)
    {
        if (_send_callbacks[i] == nullptr)
        {
            _send_callbacks[i] = std::move(callback);
            return i;
        }
    }

    _send_callbacks.emplace_back(std::move(callback));
    return _send_callbacks.size() - 1;
}

std::size_t RakNet::AddRpcCallback(RpcCallback callback) noexcept
{
    if (!_init_status) return -1;

    for (std::size_t i = 0; i < _rpc_callbacks.size(); ++i)
    {
        if (_rpc_callbacks[i] == nullptr)
        {
            _rpc_callbacks[i] = std::move(callback);
            return i;
        }
    }

    _rpc_callbacks.emplace_back(std::move(callback));
    return _rpc_callbacks.size() - 1;
}

std::size_t RakNet::AddDisconnectCallback(DisconnectCallback callback) noexcept
{
    if (!_init_status) return -1;

    for (std::size_t i = 0; i < _disconnect_callbacks.size(); ++i)
    {
        if (_disconnect_callbacks[i] == nullptr)
        {
            _disconnect_callbacks[i] = std::move(callback);
            return i;
        }
    }

    _disconnect_callbacks.emplace_back(std::move(callback));
    return _disconnect_callbacks.size() - 1;
}

void RakNet::RemoveConnectCallback(const std::size_t callback) noexcept
{
    if (_init_status && callback < _connect_callbacks.size())
    {
        _connect_callbacks[callback] = nullptr;
    }
}

void RakNet::RemoveReceiveCallback(const std::size_t callback) noexcept
{
    if (_init_status && callback < _receive_callbacks.size())
    {
        _receive_callbacks[callback] = nullptr;
    }
}

void RakNet::RemoveSendCallback(const std::size_t callback) noexcept
{
    if (_init_status && callback < _send_callbacks.size())
    {
        _send_callbacks[callback] = nullptr;
    }
}

void RakNet::RemoveRpcCallback(const std::size_t callback) noexcept
{
    if (_init_status && callback < _rpc_callbacks.size())
    {
        _rpc_callbacks[callback] = nullptr;
    }
}

void RakNet::RemoveDisconnectCallback(const std::size_t callback) noexcept
{
    if (_init_status && callback < _disconnect_callbacks.size())
    {
        _disconnect_callbacks[callback] = nullptr;
    }
}

RakNet::RakClientHookInterface::RakClientHookInterface(RakClientInterface* const orig_interface) noexcept
    : _orig_interface { orig_interface }
{}

bool RakNet::RakClientHookInterface::RPC(int* const rpcIdPointer, BitStream* const parameters,
    const PacketPriority priority, const PacketReliability reliability, const char orderingChannel,
    const bool shiftTimestamp) noexcept
{
    bool break_status = false;

    for (const auto& rpc_callback : _rpc_callbacks)
    {
        if (rpc_callback != nullptr && !rpc_callback(*rpcIdPointer, *parameters))
        {
            break_status = true;
        }
    }

    if (break_status) return true;

    return _orig_interface->RPC(rpcIdPointer, parameters, priority, reliability, orderingChannel, shiftTimestamp);
}

bool RakNet::RakClientHookInterface::Send(BitStream* const bit_stream, const PacketPriority priority,
    const PacketReliability reliability, const char orderingChannel) noexcept
{
    bool break_status = false;

    for (const auto& send_callback : _send_callbacks)
    {
        if (send_callback != nullptr && !send_callback(*bit_stream))
        {
            break_status = true;
        }
    }

    if (break_status) return true;

    return _orig_interface->Send(bit_stream, priority, reliability, orderingChannel);
}

Packet* RakNet::RakClientHookInterface::Receive() noexcept
{
    Packet* packet_pointer;

    while ((packet_pointer = _orig_interface->Receive()) != nullptr)
    {
        bool break_status = true;

        for (const auto& receive_callback : _receive_callbacks)
        {
            if (receive_callback != nullptr && !receive_callback(*packet_pointer))
            {
                break_status = false;
            }
        }

        if (break_status) break;

        _orig_interface->DeallocatePacket(packet_pointer);
    }

    return packet_pointer;
}

bool RakNet::RakClientHookInterface::Connect(const char* const hostIp, const uint16_t serverPort,
    const uint16_t clientPort, const uint32_t depreciated, const int threadSleepTimer) noexcept
{
    Logger::LogToFile("[dbg:raknet:client:connect] : connecting to game server '%s:%hu'...", hostIp, serverPort);

    _connect_status = _orig_interface->Connect(hostIp, serverPort, clientPort, depreciated, threadSleepTimer);
    if (_connect_status)
    {
        Logger::LogToFile("[dbg:raknet:client:connect] : connected");

        for (const auto& connect_callback : _connect_callbacks)
        {
            if (connect_callback != nullptr) connect_callback(hostIp, serverPort);
        }
    }

    return _connect_status;
}

void RakNet::RakClientHookInterface::Disconnect(const uint32_t blockDuration, const uint8_t orderingChannel) noexcept
{
    Logger::LogToFile("[dbg:raknet:client:disconnect] : disconnecting from server...");

    if (_connect_status)
    {
        for (const auto& disconnect_callback : _disconnect_callbacks)
        {
            if (disconnect_callback != nullptr) disconnect_callback();
        }
    }

    _connect_status = false;

    _orig_interface->Disconnect(blockDuration, orderingChannel);
}

void RakNet::RakClientHookInterface::InitializeSecurity(const char* const privateKeyP, const char* const privateKeyQ) noexcept
{
    _orig_interface->InitializeSecurity(privateKeyP, privateKeyQ);
}

void RakNet::RakClientHookInterface::SetPassword(const char* const password) noexcept
{
    _orig_interface->SetPassword(password);
}

bool RakNet::RakClientHookInterface::HasPassword() noexcept
{
    return _orig_interface->HasPassword();
}

bool RakNet::RakClientHookInterface::Send(const char* const dataPointer, const int dataLength,
    const PacketPriority priority, const PacketReliability reliability, const char orderingChannel) noexcept
{
    BitStream bit_stream { (uint8_t*)(dataPointer), static_cast<uint32_t>(dataLength), false };

    bool break_status = false;

    for (const auto& send_callback : _send_callbacks)
    {
        if (send_callback != nullptr && !send_callback(bit_stream))
        {
            break_status = true;
        }
    }

    if (break_status) return true;

    return _orig_interface->Send(&bit_stream, priority, reliability, orderingChannel);
}

void RakNet::RakClientHookInterface::DeallocatePacket(Packet* const packet_pointer) noexcept
{
    _orig_interface->DeallocatePacket(packet_pointer);
}

void RakNet::RakClientHookInterface::PingServer() noexcept
{
    _orig_interface->PingServer();
}

void RakNet::RakClientHookInterface::PingServer(const char* const hostIp,
    const uint16_t serverPort, const uint16_t clientPort,
    const bool onlyReplyOnAcceptingConnections) noexcept
{
    _orig_interface->PingServer(hostIp, serverPort, clientPort, onlyReplyOnAcceptingConnections);
}

int RakNet::RakClientHookInterface::GetAveragePing() noexcept
{
    return _orig_interface->GetAveragePing();
}

int RakNet::RakClientHookInterface::GetLastPing() noexcept
{
    return _orig_interface->GetLastPing();
}

int RakNet::RakClientHookInterface::GetLowestPing() noexcept
{
    return _orig_interface->GetLowestPing();
}

int RakNet::RakClientHookInterface::GetPlayerPing(const PlayerID playerId) noexcept
{
    return _orig_interface->GetPlayerPing(playerId);
}

void RakNet::RakClientHookInterface::StartOccasionalPing() noexcept
{
    _orig_interface->StartOccasionalPing();
}

void RakNet::RakClientHookInterface::StopOccasionalPing() noexcept
{
    _orig_interface->StopOccasionalPing();
}

bool RakNet::RakClientHookInterface::IsConnected() noexcept
{
    return _orig_interface->IsConnected();
}

uint32_t RakNet::RakClientHookInterface::GetSynchronizedRandomInteger() noexcept
{
    return _orig_interface->GetSynchronizedRandomInteger();
}

bool RakNet::RakClientHookInterface::GenerateCompressionLayer(uint32_t inputFrequencyTable[256], const bool inputLayer) noexcept
{
    return _orig_interface->GenerateCompressionLayer(inputFrequencyTable, inputLayer);
}

bool RakNet::RakClientHookInterface::DeleteCompressionLayer(const bool inputLayer) noexcept
{
    return _orig_interface->DeleteCompressionLayer(inputLayer);
}

void RakNet::RakClientHookInterface::RegisterAsRemoteProcedureCall(int* const rpcIdPointer, const RPCFunction rpcHandler) noexcept
{
    _orig_interface->RegisterAsRemoteProcedureCall(rpcIdPointer, rpcHandler);
}

void RakNet::RakClientHookInterface::RegisterClassMemberRPC(int* const rpcIdPointer, void* const rpcHandler) noexcept
{
    _orig_interface->RegisterClassMemberRPC(rpcIdPointer, rpcHandler);
}

void RakNet::RakClientHookInterface::UnregisterAsRemoteProcedureCall(int* const rpcIdPointer) noexcept
{
    _orig_interface->UnregisterAsRemoteProcedureCall(rpcIdPointer);
}

bool RakNet::RakClientHookInterface::RPC(int* const rpcIdPointer, const char* const dataPointer,
    const uint32_t bitLength, const PacketPriority priority, const PacketReliability reliability,
    const char orderingChannel, const bool shiftTimestamp) noexcept
{
    const uint32_t byte_length = (bitLength >> 3) + ((bitLength & 7) == 0 ? 0 : 1);
    BitStream bit_stream { (uint8_t*)(dataPointer), byte_length, false };

    bool break_status = false;

    for (const auto& rpc_callback : _rpc_callbacks)
    {
        if (rpc_callback != nullptr && !rpc_callback(*rpcIdPointer, bit_stream))
        {
            break_status = true;
        }
    }

    if (break_status) return true;

    return _orig_interface->RPC(rpcIdPointer, &bit_stream, priority,
        reliability, orderingChannel, shiftTimestamp);
}

void RakNet::RakClientHookInterface::SetTrackFrequencyTable(const bool trackFrequencyTable) noexcept
{
    _orig_interface->SetTrackFrequencyTable(trackFrequencyTable);
}

bool RakNet::RakClientHookInterface::GetSendFrequencyTable(uint32_t outputFrequencyTable[256]) noexcept
{
    return _orig_interface->GetSendFrequencyTable(outputFrequencyTable);
}

float RakNet::RakClientHookInterface::GetCompressionRatio() noexcept
{
    return _orig_interface->GetCompressionRatio();
}

float RakNet::RakClientHookInterface::GetDecompressionRatio() noexcept
{
    return _orig_interface->GetDecompressionRatio();
}

void RakNet::RakClientHookInterface::AttachPlugin(void* const messageHandler) noexcept
{
    _orig_interface->AttachPlugin(messageHandler);
}

void RakNet::RakClientHookInterface::DetachPlugin(void* const messageHandler) noexcept
{
    _orig_interface->DetachPlugin(messageHandler);
}

BitStream* RakNet::RakClientHookInterface::GetStaticServerData() noexcept
{
    return _orig_interface->GetStaticServerData();
}

void RakNet::RakClientHookInterface::SetStaticServerData(const char* const dataPointer, const int dataLength) noexcept
{
    _orig_interface->SetStaticServerData(dataPointer, dataLength);
}

BitStream* RakNet::RakClientHookInterface::GetStaticClientData(const PlayerID playerId) noexcept
{
    return _orig_interface->GetStaticClientData(playerId);
}

void RakNet::RakClientHookInterface::SetStaticClientData(const PlayerID playerId,
    const char* const dataPointer, const int dataLength) noexcept
{
    _orig_interface->SetStaticClientData(playerId, dataPointer, dataLength);
}

void RakNet::RakClientHookInterface::SendStaticClientDataToServer() noexcept
{
    _orig_interface->SendStaticClientDataToServer();
}

PlayerID RakNet::RakClientHookInterface::GetServerID() noexcept
{
    return _orig_interface->GetServerID();
}

PlayerID RakNet::RakClientHookInterface::GetPlayerID() noexcept
{
    return _orig_interface->GetPlayerID();
}

PlayerID RakNet::RakClientHookInterface::GetInternalID() noexcept
{
    return _orig_interface->GetInternalID();
}

const char* RakNet::RakClientHookInterface::PlayerIDToDottedIP(const PlayerID playerId) noexcept
{
    return _orig_interface->PlayerIDToDottedIP(playerId);
}

void RakNet::RakClientHookInterface::PushBackPacket(Packet* const packet_pointer, const bool pushAtHead) noexcept
{
    _orig_interface->PushBackPacket(packet_pointer, pushAtHead);
}

void RakNet::RakClientHookInterface::SetRouterInterface(void* const routerInterface) noexcept
{
    _orig_interface->SetRouterInterface(routerInterface);
}

void RakNet::RakClientHookInterface::RemoveRouterInterface(void* const routerInterface) noexcept
{
    _orig_interface->RemoveRouterInterface(routerInterface);
}

void RakNet::RakClientHookInterface::SetTimeoutTime(const RakNetTime timeMs) noexcept
{
    _orig_interface->SetTimeoutTime(timeMs);
}

bool RakNet::RakClientHookInterface::SetMTUSize(const int mtuSize) noexcept
{
    return _orig_interface->SetMTUSize(mtuSize);
}

int RakNet::RakClientHookInterface::GetMTUSize() noexcept
{
    return _orig_interface->GetMTUSize();
}

void RakNet::RakClientHookInterface::AllowConnectionResponseIPMigration(const bool allowConnectionResponseIpMigration) noexcept
{
    _orig_interface->AllowConnectionResponseIPMigration(allowConnectionResponseIpMigration);
}

void RakNet::RakClientHookInterface::AdvertiseSystem(const char* const hostIp,
    const uint16_t hostPort, const char* const dataPointer, const int dataLength) noexcept
{
    _orig_interface->AdvertiseSystem(hostIp, hostPort, dataPointer, dataLength);
}

RakNetStatisticsStruct* RakNet::RakClientHookInterface::GetStatistics() noexcept
{
    return _orig_interface->GetStatistics();
}

void RakNet::RakClientHookInterface::ApplyNetworkSimulator(const double maxSendBps,
    const uint16_t minExtraPing, const uint16_t extraPingVariance) noexcept
{
    _orig_interface->ApplyNetworkSimulator(maxSendBps, minExtraPing, extraPingVariance);
}

bool RakNet::RakClientHookInterface::IsNetworkSimulatorActive() noexcept
{
    return _orig_interface->IsNetworkSimulatorActive();
}

PlayerIndex RakNet::RakClientHookInterface::GetPlayerIndex() noexcept
{
    return _orig_interface->GetPlayerIndex();
}

bool RakNet::RakClientHookInterface::RPC_(int* const rpcIdPointer, BitStream* const bit_stream,
    const PacketPriority priority, const PacketReliability reliability, const char orderingChannel,
    const bool shiftTimestamp, const NetworkID networkId) noexcept
{
    bool break_status = false;

    for (const auto& rpc_callback : _rpc_callbacks)
    {
        if (rpc_callback != nullptr && !rpc_callback(*rpcIdPointer, *bit_stream))
        {
            break_status = true;
        }
    }

    if (break_status) return true;

    return _orig_interface->RPC_(rpcIdPointer, bit_stream, priority,
        reliability, orderingChannel, shiftTimestamp, networkId);
}

void __declspec(naked) RakNet::HookSampDestruct() noexcept
{
    static LPVOID ret_addr = nullptr;

    __asm
    {
        pushad
        mov ebp, esp
        sub esp, __LOCAL_SIZE
    }

    ret_addr = _samp_destruct_hook->GetPatch().GetAddr();
    _samp_destruct_hook = {};

    Free();

    __asm
    {
        mov esp, ebp
        popad
        jmp ret_addr
    }
}

void __declspec(naked) RakNet::HookRaknetInit() noexcept
{
    static LPVOID ret_addr = nullptr;
    static RakClientInterface* ret_iface = nullptr;

    __asm
    {
        pushad
        mov ebp, esp
        sub esp, __LOCAL_SIZE
        mov _rak_client_interface, eax
    }

    Logger::LogToFile("[dbg:raknet:hookinit] : module loading...");

    ret_addr = _rak_client_init_hook->GetPatch().GetAddr();
    _rak_client_init_hook = {};

    __asm
    {
        mov eax, rak_client_offset
        add eax, esi
        mov _rak_client_interface_ptr, eax
    }

    ret_iface = new (std::nothrow) RakClientHookInterface(_rak_client_interface);
    if (ret_iface == nullptr)
    {
        Logger::LogToFile("[err:raknet:hookinit] : failed to load module");
        ret_iface = _rak_client_interface;
        Free();
    }
    else
    {
        Logger::LogToFile("[dbg:raknet:hookinit] : module loaded");
        _load_status = true;
    }

    __asm
    {
        mov esp, ebp
        popad
        mov eax, ret_iface
        jmp ret_addr
    }
}

bool RakNet::_init_status = false;
bool RakNet::_load_status = false;

bool RakNet::_connect_status = false;

std::vector<RakNet::ConnectCallback>    RakNet::_connect_callbacks;
std::vector<RakNet::ReceiveCallback>    RakNet::_receive_callbacks;
std::vector<RakNet::SendCallback>       RakNet::_send_callbacks;
std::vector<RakNet::RpcCallback>        RakNet::_rpc_callbacks;
std::vector<RakNet::DisconnectCallback> RakNet::_disconnect_callbacks;

RakClientInterface*  RakNet::_rak_client_interface     = nullptr;
RakClientInterface** RakNet::_rak_client_interface_ptr = nullptr;

Memory::JumpHook RakNet::_rak_client_init_hook;
Memory::JumpHook RakNet::_samp_destruct_hook;
