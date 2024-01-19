/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <functional>

#include <Windows.h>

#include <memory/hacks/jump_hook.hpp>
#include <memory/view.hpp>

#include <raknet/bitstream.h>
#include <raknet/rakclient.h>

#include "addresses.hpp"
#include "logger.hpp"

namespace
{
    constexpr udword_t kConnectSignature = 0xDEADC0DE;
    constexpr    int   kConnectRpcId     = 25;
}

struct RakNet {

    RakNet(const RakNet&) = delete;
    RakNet(RakNet&&) = delete;
    RakNet& operator=(const RakNet&) = delete;
    RakNet& operator=(RakNet&&) = delete;

private:

    RakNet() noexcept = default;
    ~RakNet() noexcept = default;

public:

    static RakNet& Instance() noexcept
    {
        static RakNet instance;
        return instance;
    }

public:

    bool Initialize() noexcept
    {
        Logger::Instance().LogToFile("[dbg:raknet:initialize] module initializing...");

        if (*static_cast<const uword_t*>(Addresses::Instance().RakClientInitialization()) == static_cast<uword_t>(0x4689))
        {
            _rakclient_offset = *reinterpret_cast<const ubyte_t*>
                (static_cast<cadr_t>(Addresses::Instance().RakClientInitialization()) + sizeof(uword_t));
        }
        if (*static_cast<const uword_t*>(Addresses::Instance().RakClientInitialization()) == static_cast<uword_t>(0x8689))
        {
            _rakclient_offset = *reinterpret_cast<const udword_t*>
                (static_cast<cadr_t>(Addresses::Instance().RakClientInitialization()) + sizeof(uword_t));
        }
        if (_rakclient_offset == None<udword_t>)
        {
            Logger::Instance().LogToFile("[err:raknet:initialize] failed to find rakclient interface");
            return false;
        }

        Logger::Instance().LogToFile("[dbg:raknet:initialize] finded rakclient interface "
            "offset (value:0x%X)", _rakclient_offset);

        if (!_rak_client_init_hook.Initialize(Addresses::Instance().RakClientInitialization(), RakClientInitializationHook) ||
            !_samp_destruct_hook.Initialize(Addresses::Instance().SampDestruction(), SampDestructionHook))
        {
            Logger::Instance().LogToFile("[err:raknet:initialize] failed to initialize hooks");
            _rakclient_offset = None<udword_t>;
            _rak_client_init_hook.Deinitialize();
            _samp_destruct_hook.Deinitialize();
            return false;
        }

        Logger::Instance().LogToFile("[dbg:raknet:initialize] module initialized");

        return true;
    }

    void Deinitialize() noexcept
    {
        Logger::Instance().LogToFile("[dbg:raknet:deinitialize] module releasing...");

        _rak_client_init_hook.Deinitialize();
        _samp_destruct_hook.Deinitialize();

        if (_is_connected)
        {
            if (OnDisconnect != nullptr)
                OnDisconnect();

            _is_connected = false;
        }

        if (_is_loaded)
        {
            const auto rak_client_interface_hook =
                reinterpret_cast<RakClientHookInterface*>(*_rak_client_interface_pointer);
            *_rak_client_interface_pointer = _rak_client_interface;
            delete rak_client_interface_hook;

            _is_loaded = false;
        }

        _rak_client_interface_pointer = nullptr;
        _rak_client_interface = nullptr;

        _rakclient_offset = None<udword_t>;

        Logger::Instance().LogToFile("[dbg:raknet:deinitialize] module released");
    }

public:

    bool IsLoaded() const noexcept
    {
        return _is_loaded;
    }

    bool IsConnected() const noexcept
    {
        return _is_connected;
    }

public:

    bool SendPacket(const adr_t packet, const uint_t length) noexcept
    {
        return _rak_client_interface != nullptr && _rak_client_interface->Send(&BitStream(packet, length, false),
            PacketPriority::MEDIUM_PRIORITY, PacketReliability::RELIABLE_ORDERED, 0);
    }

public:

    RakClientInterface* GetInterface() const noexcept
    {
        return _rak_client_interface;
    }

private:

    interface RakClientHookInterface : RakClientInterface {

        RakClientHookInterface() = delete;
        ~RakClientHookInterface() noexcept = default;
        RakClientHookInterface(const RakClientHookInterface&) = delete;
        RakClientHookInterface(RakClientHookInterface&&) = delete;
        RakClientHookInterface& operator=(const RakClientHookInterface&) = delete;
        RakClientHookInterface& operator=(RakClientHookInterface&&) = delete;

    public:

        RakClientHookInterface(RakClientInterface* const orig_interface) noexcept
            : _orig_interface { orig_interface }
        {}

    public:

        bool RPC(int* const rpc_id_pointer, BitStream* const parameters,
            const PacketPriority priority, const PacketReliability reliability, const char ordering_channel,
            const bool shift_timestamp) noexcept override
        {
            if (*rpc_id_pointer == kConnectRpcId && Instance().OnHandshake != nullptr)
            {
                ubyte_t buffer[sizeof(kConnectSignature) + sizeof(ubyte_t) + 0xFF];

                *reinterpret_cast<std::remove_const_t<decltype(kConnectSignature)>*>(buffer) =
                    (HostEndian != NetEndian ? utils::ct::bswap(kConnectSignature) : kConnectSignature);

                *reinterpret_cast<ubyte_t*>(buffer + sizeof(kConnectSignature)) =
                    Instance().OnHandshake(buffer + sizeof(kConnectSignature) + sizeof(ubyte_t));

                parameters->Write(reinterpret_cast<cstr_t>(buffer), (sizeof(kConnectSignature) + sizeof(ubyte_t)) +
                    *reinterpret_cast<const ubyte_t*>(buffer + sizeof(kConnectSignature)));
            }

            return (Instance().OnOutcomingRpc != nullptr && !Instance().OnOutcomingRpc
                (*rpc_id_pointer, parameters->GetData(), parameters->GetNumberOfBytesUsed())) ||
                _orig_interface->RPC(rpc_id_pointer, parameters, priority,
                    reliability, ordering_channel, shift_timestamp);
        }

        bool Send(BitStream* const bit_stream, const PacketPriority priority,
            const PacketReliability reliability, const char ordering_channel) noexcept override
        {
            return (Instance().OnOutcomingPacket != nullptr && !Instance().OnOutcomingPacket
                (bit_stream->GetData(), bit_stream->GetNumberOfBytesUsed())) ||
                _orig_interface->Send(bit_stream, priority, reliability, ordering_channel);
        }

        Packet* Receive() noexcept override
        {
            Packet* packet = _orig_interface->Receive();

            if (Instance().OnPacket != nullptr)
            {
                while (packet != nullptr && Instance().OnPacket
                    (packet->playerIndex, packet->data, packet->length))
                {
                    _orig_interface->DeallocatePacket(packet);
                    packet = _orig_interface->Receive();
                }
            }

            return packet;
        }

        bool Connect(const cstr_t host_ip, const uword_t server_port,
            const uword_t client_port, const udword_t depreciated, const int thread_sleep_timer) noexcept override
        {
            Logger::Instance().LogToFile("[dbg:raknet:client:connect] connecting to game server '%s:%hu'...",
                host_ip, server_port);

            Instance()._is_connected = _orig_interface->Connect(host_ip, server_port,
                client_port, depreciated, thread_sleep_timer);
            if (Instance()._is_connected)
            {
                Logger::Instance().LogToFile("[dbg:raknet:client:connect] connected");

                if (Instance().OnConnect != nullptr)
                    Instance().OnConnect(host_ip, server_port);
            }

            return Instance()._is_connected;
        }

        void Disconnect(const udword_t block_duration, const ubyte_t ordering_channel) noexcept override
        {
            Logger::Instance().LogToFile("[dbg:raknet:client:disconnect] disconnecting from server...");

            if (Instance()._is_connected)
            {
                if (Instance().OnDisconnect != nullptr)
                    Instance().OnDisconnect();

                Instance()._is_connected = false;
            }

            _orig_interface->Disconnect(block_duration, ordering_channel);
        }

        void InitializeSecurity(const cstr_t private_key_p, const cstr_t private_key_q) noexcept override
        {
            _orig_interface->InitializeSecurity(private_key_p, private_key_q);
        }

        void SetPassword(const cstr_t password) noexcept override
        {
            _orig_interface->SetPassword(password);
        }

        bool HasPassword() noexcept override
        {
            return _orig_interface->HasPassword();
        }

        bool Send(const cstr_t data_pointer, const int data_length,
            const PacketPriority priority, const PacketReliability reliability,
            const char ordering_channel) noexcept override
        {
            return (Instance().OnOutcomingPacket != nullptr && !Instance().OnOutcomingPacket
                (reinterpret_cast<adr_t>(const_cast<str_t>(data_pointer)), data_length)) ||
                _orig_interface->Send(data_pointer, data_length, priority, reliability, ordering_channel);
        }

        void DeallocatePacket(Packet* const packet) noexcept override
        {
            _orig_interface->DeallocatePacket(packet);
        }

        void PingServer() noexcept override
        {
            _orig_interface->PingServer();
        }

        void PingServer(const cstr_t host_ip,
            const uword_t server_port, const uword_t client_port,
            const bool only_reply_on_accepting_connections) noexcept override
        {
            _orig_interface->PingServer(host_ip, server_port, client_port, only_reply_on_accepting_connections);
        }

        int GetAveragePing() noexcept override
        {
            return _orig_interface->GetAveragePing();
        }

        int GetLastPing() noexcept override
        {
            return _orig_interface->GetLastPing();
        }

        int GetLowestPing() noexcept override
        {
            return _orig_interface->GetLowestPing();
        }

        int GetPlayerPing(const PlayerID player_id) noexcept override
        {
            return _orig_interface->GetPlayerPing(player_id);
        }

        void StartOccasionalPing() noexcept override
        {
            _orig_interface->StartOccasionalPing();
        }

        void StopOccasionalPing() noexcept override
        {
            _orig_interface->StopOccasionalPing();
        }

        bool IsConnected() noexcept override
        {
            return _orig_interface->IsConnected();
        }

        udword_t GetSynchronizedRandomInteger() noexcept override
        {
            return _orig_interface->GetSynchronizedRandomInteger();
        }

        bool GenerateCompressionLayer(udword_t input_frequency_table[256], const bool input_layer) noexcept override
        {
            return _orig_interface->GenerateCompressionLayer(input_frequency_table, input_layer);
        }

        bool DeleteCompressionLayer(const bool input_layer) noexcept override
        {
            return _orig_interface->DeleteCompressionLayer(input_layer);
        }

        void RegisterAsRemoteProcedureCall(int* const rpc_id_pointer, const RPCFunction rpc_handler) noexcept override
        {
            _orig_interface->RegisterAsRemoteProcedureCall(rpc_id_pointer, rpc_handler);
        }

        void RegisterClassMemberRPC(int* const rpc_id_pointer, const ptr_t rpc_handler) noexcept override
        {
            _orig_interface->RegisterClassMemberRPC(rpc_id_pointer, rpc_handler);
        }

        void UnregisterAsRemoteProcedureCall(int* const rpc_id_pointer) noexcept override
        {
            _orig_interface->UnregisterAsRemoteProcedureCall(rpc_id_pointer);
        }

        bool RPC(int* const rpc_id_pointer, const cstr_t data_pointer,
            const udword_t bit_length, const PacketPriority priority, const PacketReliability reliability,
            const char ordering_channel, const bool shift_timestamp) noexcept override
        {
            assert(*rpc_id_pointer != kConnectRpcId);

            return (Instance().OnOutcomingRpc != nullptr && !Instance().OnOutcomingRpc(*rpc_id_pointer,
                reinterpret_cast<adr_t>(const_cast<str_t>(data_pointer)), BITS_TO_BYTES(bit_length))) ||
                _orig_interface->RPC(rpc_id_pointer, data_pointer, bit_length, priority, reliability,
                    ordering_channel, shift_timestamp);
        }

        void SetTrackFrequencyTable(const bool track_frequency_table) noexcept override
        {
            _orig_interface->SetTrackFrequencyTable(track_frequency_table);
        }

        bool GetSendFrequencyTable(udword_t output_frequency_table[256]) noexcept override
        {
            return _orig_interface->GetSendFrequencyTable(output_frequency_table);
        }

        float GetCompressionRatio() noexcept override
        {
            return _orig_interface->GetCompressionRatio();
        }

        float GetDecompressionRatio() noexcept override
        {
            return _orig_interface->GetDecompressionRatio();
        }

        void AttachPlugin(const ptr_t message_handler) noexcept override
        {
            _orig_interface->AttachPlugin(message_handler);
        }

        void DetachPlugin(const ptr_t message_handler) noexcept override
        {
            _orig_interface->DetachPlugin(message_handler);
        }

        BitStream* GetStaticServerData() noexcept override
        {
            return _orig_interface->GetStaticServerData();
        }

        void SetStaticServerData(const cstr_t data_pointer, const int data_length) noexcept override
        {
            _orig_interface->SetStaticServerData(data_pointer, data_length);
        }

        BitStream* GetStaticClientData(const PlayerID player_id) noexcept override
        {
            return _orig_interface->GetStaticClientData(player_id);
        }

        void SetStaticClientData(const PlayerID player_id,
            const cstr_t data_pointer, const int data_length) noexcept override
        {
            _orig_interface->SetStaticClientData(player_id, data_pointer, data_length);
        }

        void SendStaticClientDataToServer() noexcept override
        {
            _orig_interface->SendStaticClientDataToServer();
        }

        PlayerID GetServerID() noexcept override
        {
            return _orig_interface->GetServerID();
        }

        PlayerID GetPlayerID() noexcept override
        {
            return _orig_interface->GetPlayerID();
        }

        PlayerID GetInternalID() noexcept override
        {
            return _orig_interface->GetInternalID();
        }

        cstr_t PlayerIDToDottedIP(const PlayerID player_id) noexcept override
        {
            return _orig_interface->PlayerIDToDottedIP(player_id);
        }

        void PushBackPacket(Packet* const packet, const bool push_at_head) noexcept override
        {
            _orig_interface->PushBackPacket(packet, push_at_head);
        }

        void SetRouterInterface(const ptr_t router_interface) noexcept override
        {
            _orig_interface->SetRouterInterface(router_interface);
        }

        void RemoveRouterInterface(const ptr_t router_interface) noexcept override
        {
            _orig_interface->RemoveRouterInterface(router_interface);
        }

        void SetTimeoutTime(const RakNetTime time_ms) noexcept override
        {
            _orig_interface->SetTimeoutTime(time_ms);
        }

        bool SetMTUSize(const int mtu_size) noexcept override
        {
            return _orig_interface->SetMTUSize(mtu_size);
        }

        int GetMTUSize() noexcept override
        {
            return _orig_interface->GetMTUSize();
        }

        void AllowConnectionResponseIPMigration(const bool allow_connection_response_ip_migration) noexcept override
        {
            _orig_interface->AllowConnectionResponseIPMigration(allow_connection_response_ip_migration);
        }

        void AdvertiseSystem(const cstr_t host_ip, const uword_t host_port,
            const cstr_t data_pointer, const int data_length) noexcept override
        {
            _orig_interface->AdvertiseSystem(host_ip, host_port, data_pointer, data_length);
        }

        RakNetStatisticsStruct* GetStatistics() noexcept override
        {
            return _orig_interface->GetStatistics();
        }

        void ApplyNetworkSimulator(const double max_send_bps,
            const uword_t min_extra_ping, const uword_t extra_ping_variance) noexcept override
        {
            _orig_interface->ApplyNetworkSimulator(max_send_bps, min_extra_ping, extra_ping_variance);
        }

        bool IsNetworkSimulatorActive() noexcept override
        {
            return _orig_interface->IsNetworkSimulatorActive();
        }

        PlayerIndex GetPlayerIndex() noexcept override
        {
            return _orig_interface->GetPlayerIndex();
        }

        bool RPC_(int* const rpc_id_pointer, BitStream* const bit_stream,
            const PacketPriority priority, const PacketReliability reliability, const char ordering_channel,
            const bool shift_timestamp, const NetworkID network_id) noexcept override
        {
            assert(*rpc_id_pointer != kConnectRpcId);

            return (Instance().OnOutcomingRpc != nullptr && !Instance().OnOutcomingRpc
                (*rpc_id_pointer, bit_stream->GetData(), bit_stream->GetNumberOfBytesUsed())) ||
                _orig_interface->RPC_(rpc_id_pointer, bit_stream, priority, reliability,
                    ordering_channel, shift_timestamp, network_id);
        }

    private:

        RakClientInterface* const _orig_interface;

    };

private:

    static void RakClientInitializationHook() noexcept;
    static void SampDestructionHook() noexcept;

private:

    volatile bool _is_loaded = false;
    volatile bool _is_connected = false;

public:

    std::function<void(cstr_t, uword_t)>        OnConnect;
    std::function<bool(uword_t, ptr_t, uint_t)> OnPacket;
    std::function<void()>                       OnDisconnect;

    std::function<ubyte_t(ptr_t)>               OnHandshake;

    std::function<bool(ptr_t, uint_t)>          OnOutcomingPacket;
    std::function<bool(int, ptr_t, uint_t)>     OnOutcomingRpc;

private:

    RakClientInterface*  _rak_client_interface = nullptr;
    RakClientInterface** _rak_client_interface_pointer = nullptr;

    JumpHook             _rak_client_init_hook;
    JumpHook             _samp_destruct_hook;

    udword_t             _rakclient_offset = None<udword_t>;

};

void __declspec(naked) RakNet::RakClientInitializationHook() noexcept
{
    static LPVOID              return_address;
    static RakClientInterface* return_interface;

    static uint_t              temporary_buffer;

    __asm
    {
        pushad
        mov ebp, esp
        sub esp, __LOCAL_SIZE
        mov temporary_buffer, eax
    }

    Instance()._rak_client_interface = reinterpret_cast<RakClientInterface*>(temporary_buffer);

    Logger::Instance().LogToFile("[dbg:raknet:load] module loading...");

    return_address = Instance()._rak_client_init_hook.Address();
    Instance()._rak_client_init_hook.Deinitialize();

    temporary_buffer = Instance()._rakclient_offset;

    __asm
    {
        mov eax, temporary_buffer
        add eax, esi
        mov temporary_buffer, eax
    }

    Instance()._rak_client_interface_pointer = reinterpret_cast<RakClientInterface**>(temporary_buffer);

    return_interface = new (std::nothrow) RakClientHookInterface { Instance()._rak_client_interface };
    if (return_interface == nullptr)
    {
        Logger::Instance().LogToFile("[err:raknet:load] failed to load module");
        return_interface = Instance()._rak_client_interface;
        Instance().Deinitialize();
    }
    else
    {
        Logger::Instance().LogToFile("[dbg:raknet:load] module loaded");
        Instance()._is_loaded = true;
    }

    __asm
    {
        mov esp, ebp
        popad
        mov eax, return_interface
        jmp return_address
    }
}

void __declspec(naked) RakNet::SampDestructionHook() noexcept
{
    static LPVOID return_address;

    __asm
    {
        pushad
        mov ebp, esp
        sub esp, __LOCAL_SIZE
    }

    return_address = Instance()._samp_destruct_hook.Address();
    Instance().Deinitialize();

    __asm
    {
        mov esp, ebp
        popad
        jmp return_address
    }
}
