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

bool Network::Init(const AddressesBase& addr_base) noexcept
{
    if (_init_status) return false;

    Logger::LogToFile("[sv:dbg:network:init] : module initializing...");

    if (const auto error = WSAStartup(MAKEWORD(2, 2), &WSADATA()))
    {
        Logger::LogToFile("[sv:err:network:init] : wsastartup error (code:%d)", error);
        return false;
    }

    _socket_handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (_socket_handle == INVALID_SOCKET)
    {
        Logger::LogToFile("[sv:err:network:init] : socket error (code:%d)", WSAGetLastError());
        WSACleanup();
        return false;
    }

    if (setsockopt(_socket_handle, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<PCCH>
            (&kSendBufferSize), sizeof(kSendBufferSize)) == SOCKET_ERROR ||
        setsockopt(_socket_handle, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<PCCH>
            (&kRecvBufferSize), sizeof(kRecvBufferSize)) == SOCKET_ERROR)
    {
        Logger::LogToFile("[sv:err:network:init] : setsockopt error (code:%d)", WSAGetLastError());
        closesocket(_socket_handle);
        _socket_handle = INVALID_SOCKET;
        WSACleanup();
        return false;
    }

    if (!RakNet::Init(addr_base))
    {
        Logger::LogToFile("[sv:err:network:init] : failed to init raknet");
        closesocket(_socket_handle);
        _socket_handle = INVALID_SOCKET;
        WSACleanup();
        return false;
    }

    RakNet::SetConnectCallback(OnRaknetConnect);
    RakNet::SetReceiveCallback(OnRaknetReceive);
    RakNet::SetRpcCallback(OnRaknetRpc);
    RakNet::SetDisconnectCallback(OnRaknetDisconnect);

    _connect_status = ConnectionStatus::Disconnected;

    Logger::LogToFile("[sv:dbg:network:init] : module initialized");

    _init_status = true;

    return true;
}

void Network::Free() noexcept
{
    if (_init_status)
    {
        Logger::LogToFile("[sv:dbg:network:free] : module releasing...");

        if (_connect_status != ConnectionStatus::Disconnected)
        {
            if (_disconnect_callback != nullptr) _disconnect_callback();
        }

        _connect_status = ConnectionStatus::Disconnected;

        if (_voice_thread.joinable())
            _voice_thread.detach();

        closesocket(_socket_handle);
        _socket_handle = INVALID_SOCKET;

        _server_ip  = {};
        _server_key = NULL;

        _connect_callback    = nullptr;
        _sv_connect_callback = nullptr;
        _sv_init_callback    = nullptr;
        _disconnect_callback = nullptr;

        ZeroMemory(_input_voice_packet.GetData(), _input_voice_packet.GetSize());
        ZeroMemory(_output_voice_packet.GetData(), _output_voice_packet.GetSize());

        while (!_control_queue.empty()) _control_queue.pop();
        while (!_voice_queue.empty()) _voice_queue.pop();

        RakNet::Free();
        WSACleanup();

        Logger::LogToFile("[sv:dbg:network:free] : module released");

        _init_status = false;
    }
}

bool Network::SendControlPacket(const WORD packet, const LPCVOID data, const WORD size) noexcept
{
    if (_connect_status != ConnectionStatus::Connected) return false;

    BitStream bit_stream { sizeof(BYTE) + sizeof(ControlPacket) + size };

    bit_stream.Write<BYTE>(kRaknetPacketId);

    bit_stream.Write<WORD>(packet);
    bit_stream.Write<WORD>(size);

    if (data != nullptr && size != 0)
    {
        bit_stream.Write(static_cast<PCCH>(data), size);
    }

    return RakNet::Send(&bit_stream);
}

bool Network::SendVoicePacket(const LPCVOID data, const WORD size) noexcept
{
    if (data == nullptr || size == 0 || size > kMaxVoiceDataSize)
        return false;

    if (_connect_status != ConnectionStatus::Connected)
        return false;

    _output_voice_packet->length = size;
    _output_voice_packet->CalcHash();

    std::memcpy(_output_voice_packet->data, data, size);

    const auto voice_packet_data = reinterpret_cast<PCCH>(&_output_voice_packet);
    const auto voice_packet_size = static_cast<int>(_output_voice_packet->GetFullSize());

    const auto sended = send(_socket_handle, voice_packet_data, voice_packet_size, NULL);

    ++_output_voice_packet->packid;

    return sended == voice_packet_size;
}

void Network::EndSequence() noexcept
{
    if (_init_status)
    {
        _output_voice_packet->packid = NULL;
    }
}

bool Network::ReceiveControlPacket(ControlPacketContainer& buffer) noexcept
{
    if (!_init_status || _control_queue.empty())
        return false;

    buffer = std::move(*_control_queue.front());
    _control_queue.pop();

    return true;
}

bool Network::ReceiveVoicePacket(VoicePacketContainer& buffer) noexcept
{
    if (!_init_status || _voice_queue.empty())
        return false;

    buffer = std::move(*_voice_queue.front());
    _voice_queue.pop();

    return true;
}

void Network::SetConnectCallback(ConnectCallback&& callback) noexcept
{
    if (_init_status) _connect_callback = std::move(callback);
}

void Network::SetSvConnectCallback(SvConnectCallback&& callback) noexcept
{
    if (_init_status) _sv_connect_callback = std::move(callback);
}

void Network::SetSvInitCallback(SvInitCallback&& callback) noexcept
{
    if (_init_status) _sv_init_callback = std::move(callback);
}

void Network::SetDisconnectCallback(DisconnectCallback&& callback) noexcept
{
    if (_init_status) _disconnect_callback = std::move(callback);
}

void Network::VoiceThread() noexcept
{
    VoicePacket keep_alive_packet;
    ZeroMemory(&keep_alive_packet, sizeof(keep_alive_packet));
    keep_alive_packet.svrkey = _server_key;
    keep_alive_packet.packet = SV::VoicePacketType::keepAlive;
    keep_alive_packet.packid = NULL;
    keep_alive_packet.length = NULL;
    keep_alive_packet.sender = NULL;
    keep_alive_packet.stream = NULL;
    keep_alive_packet.CalcHash();

    Timer::time_t keep_alive_last_time = 0;

    while (true)
    {
        const auto current_status = _connect_status;

        if (current_status == ConnectionStatus::Disconnected) break;

        // Sending keep-alive packets...
        // -----------------------------------------------------------------

        if (const auto current_time = Timer::Get(); current_time - keep_alive_last_time >= kKeepAliveInterval)
        {
            send(_socket_handle, reinterpret_cast<PCCH>(&keep_alive_packet),
                sizeof(keep_alive_packet), NULL);
            keep_alive_last_time = current_time;
        }

        // Receiving voice packets...
        // -----------------------------------------------------------------

        if (current_status != ConnectionStatus::Connected)
        {
            SleepForMilliseconds(100);
            continue;
        }

        const auto received = recv(_socket_handle,
            static_cast<PCH>(_input_voice_packet.GetData()),
            _input_voice_packet.GetSize(), NULL);

        if (received == SOCKET_ERROR) break;
        if (received < static_cast<decltype(received)>(sizeof(VoicePacket))) continue;

        if (!_input_voice_packet->CheckHeader()) continue;
        if (received != _input_voice_packet->GetFullSize()) continue;
        if (_input_voice_packet->packet == SV::VoicePacketType::keepAlive) continue;
        if (!GameUtil::IsGameActive()) continue;

        _voice_queue.try_emplace(_input_voice_packet);
    }
}

void Network::OnRaknetConnect(const PCCH ip, const WORD port) noexcept
{
    if (_init_status)
    {
        _server_ip = ip;

        if (_connect_status == ConnectionStatus::Disconnected)
        {
            if (_connect_callback != nullptr) _connect_callback(_server_ip, port);
        }

        _connect_status = ConnectionStatus::RNConnecting;
    }
}

bool Network::OnRaknetRpc(const int id, BitStream& parameters) noexcept
{
    if (_init_status && id == kRaknetConnectRcpId)
    {
        if (_sv_connect_callback != nullptr)
        {
            SV::ConnectPacket connect_data {};

            _sv_connect_callback(connect_data);

            parameters.Write(reinterpret_cast<const char*>(&connect_data), sizeof(connect_data));

            Logger::LogToFile("[sv:dbg:network:connect] : raknet connecting... "
                "(version:%hhu;micro:%hhu)", connect_data.version, connect_data.micro);
        }
    }

    return true;
}

bool Network::OnRaknetReceive(Packet& packet) noexcept
{
    if (!_init_status)
        return true;

    if (packet.length < sizeof(BYTE) + sizeof(ControlPacket))
        return true;

    if (*packet.data != kRaknetPacketId)
        return true;

    const auto  control_packet_ptr  = reinterpret_cast<ControlPacket*>(packet.data + sizeof(BYTE));
    const DWORD control_packet_size = packet.length - sizeof(BYTE);

    if (control_packet_size != control_packet_ptr->GetFullSize()) return false;

    switch (control_packet_ptr->packet)
    {
        case SV::ControlPacketType::serverInfo:
        {
            const auto& server_data = *reinterpret_cast<SV::ServerInfoPacket*>(control_packet_ptr->data);
            if (control_packet_ptr->length != sizeof(server_data)) return false;

            Logger::LogToFile("[sv:dbg:network:serverInfo] : connecting to voiceserver "
                "'%s:%hu'...", _server_ip.c_str(), server_data.serverPort);

            sockaddr_in server_address {};

            server_address.sin_family      = AF_INET;
            server_address.sin_addr.s_addr = inet_addr(_server_ip.c_str());
            server_address.sin_port        = htons(server_data.serverPort);

            if (connect(_socket_handle, reinterpret_cast<const sockaddr*>(&server_address),
                sizeof(server_address)) == SOCKET_ERROR)
            {
                Logger::LogToFile("[sv:err:network:serverInfo] : connect error (code:%d)", WSAGetLastError());
                return false;
            }

            _server_key = server_data._server_key;

            _output_voice_packet->svrkey = _server_key;
            _output_voice_packet->packet = SV::VoicePacketType::voicePacket;
            _output_voice_packet->packid = NULL;
            _output_voice_packet->length = NULL;
            _output_voice_packet->sender = NULL;
            _output_voice_packet->stream = NULL;
            _output_voice_packet->CalcHash();

            _connect_status = ConnectionStatus::SVConnecting;

            _voice_thread = std::thread(VoiceThread);
        }
        break;
        case SV::ControlPacketType::pluginInit:
        {
            const auto& plugin_data = *reinterpret_cast<SV::PluginInitPacket*>(control_packet_ptr->data);
            if (control_packet_ptr->length != sizeof(plugin_data)) return false;

            Logger::LogToFile("[sv:dbg:network:pluginInit] : plugin init packet "
                "(bitrate:%u;mute:%hhu)", plugin_data.bitrate, plugin_data.mute);

            if (_sv_init_callback != nullptr) _sv_init_callback(plugin_data);

            _connect_status = ConnectionStatus::Connected;
        }
        break;
        default:
        {
            _control_queue.try_emplace(control_packet_ptr, control_packet_size);
        }
    }

    return false;
}

void Network::OnRaknetDisconnect() noexcept
{
    if (_init_status)
    {
        Logger::LogToFile("[sv:dbg:network:disconnect] : raknet disconnected");

        if (_connect_status != ConnectionStatus::Disconnected)
        {
            if (_disconnect_callback != nullptr) _disconnect_callback();
        }

        _connect_status = ConnectionStatus::Disconnected;

        if (_voice_thread.joinable())
            _voice_thread.detach();

        _server_ip.clear();
        _server_key = NULL;

        ZeroMemory(_input_voice_packet.GetData(), _input_voice_packet.GetSize());
        ZeroMemory(_output_voice_packet.GetData(), _output_voice_packet.GetSize());

        while (!_control_queue.empty()) _control_queue.pop();
        while (!_voice_queue.empty()) _voice_queue.pop();
    }
}

bool Network::_init_status = false;

SOCKET      Network::_socket_handle = INVALID_SOCKET;
int         Network::_connect_status = ConnectionStatus::Disconnected;
std::thread Network::_voice_thread;
std::string Network::_server_ip;
DWORD       Network::_server_key = NULL;

Network::ConnectCallback    Network::_connect_callback = nullptr;
Network::SvConnectCallback  Network::_sv_connect_callback = nullptr;
Network::SvInitCallback     Network::_sv_init_callback = nullptr;
Network::DisconnectCallback Network::_disconnect_callback = nullptr;

SPSCQueue<ControlPacketContainer> Network::_control_queue { 128 };
SPSCQueue<VoicePacketContainer>   Network::_voice_queue   { 512 };

VoicePacketContainer Network::_input_voice_packet  { kMaxVoiceDataSize };
VoicePacketContainer Network::_output_voice_packet { kMaxVoiceDataSize };
