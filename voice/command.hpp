/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <system/types.hpp>
#include <network/address.hpp>
#include <network/socket.hpp>
#include <other/utils.hpp>

// Types
// --------------------------------------------

struct CommandPackets
{
    enum : ubyte_t
    {
        KeepAlive,

        PlayerCreate,
        PlayerListener,
        PlayerSpeaker,
        PlayerAttachStream,
        PlayerDetachStream,
        PlayerDelete,

        StreamCreate,
        StreamTransiter,
        StreamAttachListener,
        StreamDetachListener,
        StreamDelete,

        ENUM_END
    };
};

// Packets
// --------------------------------------------

#pragma pack(push, 1)

struct PlayerCreate
{
     uword_t player;
    udword_t key;
};

struct PlayerListener
{
    uword_t player;
    ubyte_t status;
};

struct PlayerSpeaker
{
     uword_t player;
    udword_t channels;
};

struct PlayerAttachStream
{
     uword_t player;
    udword_t channels;
     uword_t stream;
};

struct PlayerDetachStream
{
     uword_t player;
    udword_t channels;
     uword_t stream;
};

struct PlayerDelete
{
    uword_t player;
};

struct StreamCreate
{
    uword_t stream;
};

struct StreamTransiter
{
    uword_t stream;
    ubyte_t status;
};

struct StreamAttachListener
{
    uword_t stream;
    uword_t player;
};

struct StreamDetachListener
{
    uword_t stream;
    uword_t player;
};

struct StreamDelete
{
    uword_t stream;
};

#pragma pack(pop)

// Constants
// --------------------------------------------

constexpr size_t kCommandContentLimit = std::max
({
    sizeof(PlayerCreate),
    sizeof(PlayerListener),
    sizeof(PlayerSpeaker),
    sizeof(PlayerAttachStream),
    sizeof(PlayerDetachStream),
    sizeof(PlayerDelete),
    sizeof(StreamCreate),
    sizeof(StreamTransiter),
    sizeof(StreamAttachListener),
    sizeof(StreamDetachListener),
    sizeof(StreamDelete)
});

// --------------------------------------------

struct CommandService {

    CommandService(const CommandService&) = delete;
    CommandService(CommandService&&) = delete;
    CommandService& operator=(const CommandService&) = delete;
    CommandService& operator=(CommandService&&) = delete;

private:

    CommandService() noexcept = default;
    ~CommandService() noexcept = default;

public:

    static CommandService& Instance() noexcept
    {
        static CommandService instance;
        return instance;
    }

public:

    /*
        Wait for connection from 'control' host
    */
    bool Initialize(const IPv4Address& command, const IPv4Address& control) noexcept
    {
        _socket.Deinitialize();

        IPv4TcpSocket listener;
        {
            if (!listener.Initialize(false)) return false;
            if (!listener.SetOption(SOL_SOCKET, SO_REUSEADDR, 1)) return false;
            if (!listener.Bind(command)) return false;
            if (!listener.Listen(1)) return false;
        }

        IPv4TcpSocket socket;
        {
            IPv4Address address; do socket = listener.Accept(address, false);
            while (socket.Invalid() || (!control.IsEmpty() && address != control));
        }

        if (!socket.SetOption(SOL_SOCKET, SO_REUSEADDR, 1)) return false;
        if (!socket.SetOption(IPPROTO_TCP, TCP_NODELAY, 1)) return false;
        if (!socket.Shutdown(SHUT_WR)) return false;

        _socket = std::move(socket);

        return true;
    }

    void Deinitialize() noexcept
    {
        _socket.Deinitialize();
    }

public:

    static constexpr ubyte_t WaitError = ~static_cast<ubyte_t>(0);
    static constexpr ubyte_t WaitEmpty = ~static_cast<ubyte_t>(1);

public:

    ubyte_t WaitCommand(const ptr_t buffer, const Time timeout = Poll::InfiniteTimeout) noexcept
    {
        switch (_socket.Wait(timeout, POLLIN))
        {
            case Poll::WaitError: return WaitError;
            case Poll::WaitEmpty: return WaitEmpty;
        }

        goto ReceiveCommand;

    CheckCommand:

        switch (_socket.Wait(Poll::InstantTimeout, POLLIN))
        {
            case Poll::WaitError: return WaitError;
            case Poll::WaitEmpty: return WaitEmpty;
        }
        
    ReceiveCommand:

        ubyte_t command;

        if (_socket.ReceiveValue(command, MSG_WAITALL) != 1)
            return WaitError;

        switch (command)
        {
            case CommandPackets::KeepAlive:
            {
                goto CheckCommand;
            }
            case CommandPackets::PlayerCreate:
            {
                auto& content = *static_cast<PlayerCreate*>(buffer);

                if (_socket.ReceiveValue(content, MSG_WAITALL) != 1)
                    return WaitError;

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&content.player);
                    utils::bswap(&content.key);
                }

                break;
            }
            case CommandPackets::PlayerListener:
            {
                auto& content = *static_cast<PlayerListener*>(buffer);

                if (_socket.ReceiveValue(content, MSG_WAITALL) != 1)
                    return WaitError;

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&content.player);
                }

                break;
            }
            case CommandPackets::PlayerSpeaker:
            {
                auto& content = *static_cast<PlayerSpeaker*>(buffer);

                if (_socket.ReceiveValue(content, MSG_WAITALL) != 1)
                    return WaitError;

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&content.player);
                    utils::bswap(&content.channels);
                }

                break;
            }
            case CommandPackets::PlayerAttachStream:
            {
                auto& content = *static_cast<PlayerAttachStream*>(buffer);

                if (_socket.ReceiveValue(content, MSG_WAITALL) != 1)
                    return WaitError;

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&content.player);
                    utils::bswap(&content.channels);
                    utils::bswap(&content.stream);
                }

                break;
            }
            case CommandPackets::PlayerDetachStream:
            {
                auto& content = *static_cast<PlayerDetachStream*>(buffer);

                if (_socket.ReceiveValue(content, MSG_WAITALL) != 1)
                    return WaitError;

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&content.player);
                    utils::bswap(&content.channels);
                    utils::bswap(&content.stream);
                }

                break;
            }
            case CommandPackets::PlayerDelete:
            {
                auto& content = *static_cast<PlayerDelete*>(buffer);

                if (_socket.ReceiveValue(content, MSG_WAITALL) != 1)
                    return WaitError;

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&content.player);
                }

                break;
            }
            case CommandPackets::StreamCreate:
            {
                auto& content = *static_cast<StreamCreate*>(buffer);

                if (_socket.ReceiveValue(content, MSG_WAITALL) != 1)
                    return WaitError;

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&content.stream);
                }

                break;
            }
            case CommandPackets::StreamTransiter:
            {
                auto& content = *static_cast<StreamTransiter*>(buffer);

                if (_socket.ReceiveValue(content, MSG_WAITALL) != 1)
                    return WaitError;

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&content.stream);
                }

                break;
            }
            case CommandPackets::StreamAttachListener:
            {
                auto& content = *static_cast<StreamAttachListener*>(buffer);

                if (_socket.ReceiveValue(content, MSG_WAITALL) != 1)
                    return WaitError;

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&content.stream);
                    utils::bswap(&content.player);
                }

                break;
            }
            case CommandPackets::StreamDetachListener:
            {
                auto& content = *static_cast<StreamDetachListener*>(buffer);

                if (_socket.ReceiveValue(content, MSG_WAITALL) != 1)
                    return WaitError;

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&content.stream);
                    utils::bswap(&content.player);
                }

                break;
            }
            case CommandPackets::StreamDelete:
            {
                auto& content = *static_cast<StreamDelete*>(buffer);

                if (_socket.ReceiveValue(content, MSG_WAITALL) != 1)
                    return WaitError;

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&content.stream);
                }

                break;
            }
            default:
            {
                return WaitError;
            }
        }

        return command;
    }

private:

    IPv4TcpSocket _socket;

};
