/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cassert>
#include <functional>

#include <system/types.hpp>
#include <system/clock.hpp>
#include <network/address.hpp>
#include <network/socket.hpp>
#include <other/utils.hpp>

#include "player.hpp"
#include "stream.hpp"

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

struct CommandPlayerCreate
{
     uword_t player;
    udword_t key;
};

struct CommandPlayerListener
{
    uword_t player;
    ubyte_t status;
};

struct CommandPlayerSpeaker
{
     uword_t player;
    udword_t channels;
};

struct CommandPlayerAttachStream
{
     uword_t player;
    udword_t channels;
     uword_t stream;
};

struct CommandPlayerDetachStream
{
     uword_t player;
    udword_t channels;
     uword_t stream;
};

struct CommandPlayerDelete
{
    uword_t player;
};

struct CommandStreamCreate
{
    uword_t stream;
};

struct CommandStreamTransiter
{
    uword_t stream;
    ubyte_t status;
};

struct CommandStreamAttachListener
{
    uword_t stream;
    uword_t player;
};

struct CommandStreamDetachListener
{
    uword_t stream;
    uword_t player;
};

struct CommandStreamDelete
{
    uword_t stream;
};

#pragma pack(pop)

// Constants
// --------------------------------------------

constexpr size_t kCommandContentLimit = std::max
({
    sizeof(CommandPlayerCreate),
    sizeof(CommandPlayerListener),
    sizeof(CommandPlayerSpeaker),
    sizeof(CommandPlayerAttachStream),
    sizeof(CommandPlayerDetachStream),
    sizeof(CommandPlayerDelete),
    sizeof(CommandStreamCreate),
    sizeof(CommandStreamTransiter),
    sizeof(CommandStreamAttachListener),
    sizeof(CommandStreamDetachListener),
    sizeof(CommandStreamDelete)
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

    bool Initialize(const IPv4Address& control, const IPv4Address& command) noexcept
    {
        assert(control.Valid());
        assert(command.Valid());

        _socket.Deinitialize();

        IPv4TcpSocket socket;

        if (!socket.Initialize(true))
        {
            Logger::Instance().Log("[Command] Failed to initialize socket (%d)", GetSocketError());
            return false;
        }
        if (!socket.SetOption(SOL_SOCKET, SO_REUSEADDR, 1))
        {
            Logger::Instance().Log("[Command] Failed to set option(SO_REUSEADDR) (%d)", GetSocketError());
            return false;
        }
        if (!socket.Bind(control))
        {
            char buffer[IPv4Address::LengthLimit + 1];

            if (!control.Print(buffer))
            {
                buffer[0] = 'I'; buffer[1] = 'N'; buffer[2] = 'V'; buffer[3] = 'A';
                buffer[4] = 'L'; buffer[5] = 'I'; buffer[6] = 'D'; buffer[7] = '\0';
            }

            Logger::Instance().Log("[Command] Failed to bind(%s) (%d)", buffer, GetSocketError());
            return false;
        }
        if (!socket.Listen(1))
        {
            Logger::Instance().Log("[Command] Failed to listen(1) (%d)", GetSocketError());
            return false;
        }

        _command_address = command;

        _socket = std::move(socket);

        return true;
    }

    void Deinitialize() noexcept
    {
        _socket.Deinitialize();
    }

public:

    ptr_t BeginCommand(const ubyte_t command) noexcept
    {
        _command_buffer[0] = command;

        _command_length = 1;

        return _command_buffer + 1;
    }

    void EndCommand() noexcept
    {
        switch (_command_buffer[0])
        {
            case CommandPackets::PlayerCreate:
            {
                _command_length += sizeof(CommandPlayerCreate);

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&reinterpret_cast<CommandPlayerCreate*>(_command_buffer + 1)->player);
                    utils::bswap(&reinterpret_cast<CommandPlayerCreate*>(_command_buffer + 1)->key);
                }

                break;
            }
            case CommandPackets::PlayerListener:
            {
                _command_length += sizeof(CommandPlayerListener);

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&reinterpret_cast<CommandPlayerListener*>(_command_buffer + 1)->player);
                }

                break;
            }
            case CommandPackets::PlayerSpeaker:
            {
                _command_length += sizeof(CommandPlayerSpeaker);

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&reinterpret_cast<CommandPlayerSpeaker*>(_command_buffer + 1)->player);
                    utils::bswap(&reinterpret_cast<CommandPlayerSpeaker*>(_command_buffer + 1)->channels);
                }

                break;
            }
            case CommandPackets::PlayerAttachStream:
            {
                _command_length += sizeof(CommandPlayerAttachStream);

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&reinterpret_cast<CommandPlayerAttachStream*>(_command_buffer + 1)->player);
                    utils::bswap(&reinterpret_cast<CommandPlayerAttachStream*>(_command_buffer + 1)->channels);
                    utils::bswap(&reinterpret_cast<CommandPlayerAttachStream*>(_command_buffer + 1)->stream);
                }

                break;
            }
            case CommandPackets::PlayerDetachStream:
            {
                _command_length += sizeof(CommandPlayerDetachStream);

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&reinterpret_cast<CommandPlayerDetachStream*>(_command_buffer + 1)->player);
                    utils::bswap(&reinterpret_cast<CommandPlayerDetachStream*>(_command_buffer + 1)->channels);
                    utils::bswap(&reinterpret_cast<CommandPlayerDetachStream*>(_command_buffer + 1)->stream);
                }

                break;
            }
            case CommandPackets::PlayerDelete:
            {
                _command_length += sizeof(CommandPlayerDelete);

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&reinterpret_cast<CommandPlayerDelete*>(_command_buffer + 1)->player);
                }

                break;
            }
            case CommandPackets::StreamCreate:
            {
                _command_length += sizeof(CommandStreamCreate);

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&reinterpret_cast<CommandStreamCreate*>(_command_buffer + 1)->stream);
                }

                break;
            }
            case CommandPackets::StreamTransiter:
            {
                _command_length += sizeof(CommandStreamTransiter);

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&reinterpret_cast<CommandStreamTransiter*>(_command_buffer + 1)->stream);
                }

                break;
            }
            case CommandPackets::StreamAttachListener:
            {
                _command_length += sizeof(CommandStreamAttachListener);

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&reinterpret_cast<CommandStreamAttachListener*>(_command_buffer + 1)->stream);
                    utils::bswap(&reinterpret_cast<CommandStreamAttachListener*>(_command_buffer + 1)->player);
                }

                break;
            }
            case CommandPackets::StreamDetachListener:
            {
                _command_length += sizeof(CommandStreamDetachListener);

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&reinterpret_cast<CommandStreamDetachListener*>(_command_buffer + 1)->stream);
                    utils::bswap(&reinterpret_cast<CommandStreamDetachListener*>(_command_buffer + 1)->player);
                }

                break;
            }
            case CommandPackets::StreamDelete:
            {
                _command_length += sizeof(CommandStreamDelete);

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&reinterpret_cast<CommandStreamDelete*>(_command_buffer + 1)->stream);
                }

                break;
            }
        }
    }

    bool SendCommand() noexcept
    {
        return _socket.Send(_command_buffer, _command_length) == _command_length;
    }

public:

    int Tick(const Time moment = Clock::Now()) noexcept
    {
        if (_socket.IsListening() == 1)
        {
            if (_socket.Wait(Poll::InstantTimeout, POLLIN) == 1)
            {
                IPv4Address address;
                IPv4TcpSocket socket = _socket.Accept(address, false);
                if (socket.Valid() && (_command_address.IsEmpty() || address == _command_address))
                {
                    if (!socket.SetOption(SOL_SOCKET, SO_REUSEADDR, 1))
                    {
                        Logger::Instance().Log("[Command] Failed to set option(SO_REUSEADDR) (%d)", GetSocketError());
                        return -1;
                    }
                    if (!socket.SetOption(IPPROTO_TCP, TCP_NODELAY, 1))
                    {
                        Logger::Instance().Log("[Command] Failed to set option(TCP_NODELAY) (%d)", GetSocketError());
                        return -1;
                    }
                    if (!socket.Shutdown(SHUT_RD))
                    {
                        Logger::Instance().Log("[Command] Failed to shutdown(SHUT_RD) (%d)", GetSocketError());
                        return -1;
                    }

                    _socket = std::move(socket);

                    OnSynchronize();
                }
            }

            return 0;
        }

        constexpr Time kKeepAliveInterval = Time::Seconds(5);

        if (_clock.ExpiredAndRestart(kKeepAliveInterval, moment))
        {
            BeginCommand(CommandPackets::KeepAlive);
            SendCommand();
        }

        return 1;
    }

public:

    std::function<void()> OnSynchronize;

private:

    IPv4TcpSocket _socket;

private:

    IPv4Address _command_address;

private:

    ubyte_t _command_buffer[1 + kCommandContentLimit];
    uword_t _command_length = 0;

private:

    Clock _clock;

};
