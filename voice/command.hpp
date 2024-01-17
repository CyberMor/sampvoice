/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cassert>

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

    bool Initialize(const IPv4Address& command, const IPv4Address& control) noexcept
    {
        assert(command.Valid());
        assert(control.Valid());

        _socket.Deinitialize();

        IPv4TcpSocket socket;

        if (!socket.Initialize(false))
        {
            if (Logger != nullptr)
            {
                std::fprintf(Logger, "[Command] Failed to initialize socket (%d)\n", GetSocketError());
                std::fflush(Logger);
            }

            return false;
        }

        if (!socket.SetOption(SOL_SOCKET, SO_REUSEADDR, 1))
        {
            if (Logger != nullptr)
            {
                std::fprintf(Logger, "[Command] Failed to set option(SO_REUSEADDR) (%d)\n", GetSocketError());
                std::fflush(Logger);
            }

            return false;
        }

        if (!socket.SetOption(IPPROTO_TCP, TCP_NODELAY, 1))
        {
            if (Logger != nullptr)
            {
                std::fprintf(Logger, "[Command] Failed to set option(TCP_NODELAY) (%d)\n", GetSocketError());
                std::fflush(Logger);
            }

            return false;
        }

        if (!socket.Bind(command))
        {
            if (Logger != nullptr)
            {
                char buffer[IPv4Address::LengthLimit + 1];

                if (!command.Print(buffer))
                {
                    buffer[0] = 'I'; buffer[1] = 'N'; buffer[2] = 'V'; buffer[3] = 'A';
                    buffer[4] = 'L'; buffer[5] = 'I'; buffer[6] = 'D'; buffer[7] = '\0';
                }

                std::fprintf(Logger, "[Command] Failed to bind(%s) (%d)\n", buffer, GetSocketError());
                std::fflush(Logger);
            }

            return false;
        }

        constexpr Time kConnectCooldown = Time::Seconds(1);
        while (!socket.Connect(control)) // Wait until the Control Server becomes available
            utils::thread::sleep(kConnectCooldown);

        if (!socket.Shutdown(SHUT_WR))
        {
            if (Logger != nullptr)
            {
                std::fprintf(Logger, "[Command] Failed to shutdown(SHUT_WR) (%d)\n", GetSocketError());
                std::fflush(Logger);
            }

            return false;
        }

        _socket = std::move(socket);

        return true;
    }

    void Deinitialize() noexcept
    {
        _socket.Deinitialize();
    }

public:

    static constexpr ubyte_t WaitEmpty = ~static_cast<ubyte_t>(0);
    static constexpr ubyte_t WaitError = ~static_cast<ubyte_t>(1);

public:

    ubyte_t WaitCommand(const ptr_t buffer, const Time timeout = Poll::InfiniteTimeout) noexcept
    {
        assert(buffer != nullptr);

        switch (_socket.Wait(timeout, POLLIN))
        {
            case Poll::WaitEmpty: return WaitEmpty;
            case Poll::WaitError:
            {
#ifndef _WIN32
                if (GetSocketError() == EINTR)
                {
                    if (Logger != nullptr)
                    {
                        std::fprintf(Logger, "[Command] poll(%d, POLLIN) interrupted\n", static_cast<int>(timeout.Milliseconds()));
                        std::fflush(Logger);
                    }

                    return WaitEmpty;
                }
#endif
                if (Logger != nullptr)
                {
                    std::fprintf(Logger, "[Command] Failed to poll(%d, POLLIN) (%d)\n", static_cast<int>(timeout.Milliseconds()), GetSocketError());
                    std::fflush(Logger);
                }

                return WaitError;
            }
        }

        goto ReceiveCommand;

    CheckCommand:

        switch (_socket.Wait(Poll::InstantTimeout, POLLIN))
        {
            case Poll::WaitEmpty: return WaitEmpty;
            case Poll::WaitError:
            {
#ifndef _WIN32
                if (GetSocketError() == EINTR)
                {
                    if (Logger != nullptr)
                    {
                        std::fprintf(Logger, "[Command] poll(0, POLLIN) interrupted\n");
                        std::fflush(Logger);
                    }

                    return WaitEmpty;
                }
#endif
                if (Logger != nullptr)
                {
                    std::fprintf(Logger, "[Command] Failed to poll(0, POLLIN) (%d)\n", GetSocketError());
                    std::fflush(Logger);
                }

                return WaitError;
            }
        }
        
    ReceiveCommand:

        ubyte_t command;

        if (_socket.ReceiveValue(command, MSG_WAITALL) != 1)
        {
#ifdef _WIN32
            if (GetSocketError() == WSAEINTR)
#else
            if (GetSocketError() == EINTR)
#endif
            {
                if (Logger != nullptr)
                {
                    std::fprintf(Logger, "[Command] recv(MSG_WAITALL) interrupted. Repeat call.\n");
                    std::fflush(Logger);
                }

                goto ReceiveCommand;
            }

            if (Logger != nullptr)
            {
                std::fprintf(Logger, "[Command] Failed to recv(MSG_WAITALL) (%d)\n", GetSocketError());
                std::fflush(Logger);
            }

            return WaitError;
        }

    ReceiveBody:

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
                {
#ifdef _WIN32
                    if (GetSocketError() == WSAEINTR)
#else
                    if (GetSocketError() == EINTR)
#endif
                    {
                        if (Logger != nullptr)
                        {
                            std::fprintf(Logger, "[Command] recv(MSG_WAITALL) interrupted. Command(%hhu). Repeat call.\n", command);
                            std::fflush(Logger);
                        }

                        goto ReceiveBody;
                    }

                    if (Logger != nullptr)
                    {
                        std::fprintf(Logger, "[Command] Failed to recv(MSG_WAITALL) (%d)\n", GetSocketError());
                        std::fflush(Logger);
                    }

                    return WaitError;
                }

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
                {
#ifdef _WIN32
                    if (GetSocketError() == WSAEINTR)
#else
                    if (GetSocketError() == EINTR)
#endif
                    {
                        if (Logger != nullptr)
                        {
                            std::fprintf(Logger, "[Command] recv(MSG_WAITALL) interrupted. Command(%hhu). Repeat call.\n", command);
                            std::fflush(Logger);
                        }

                        goto ReceiveBody;
                    }

                    if (Logger != nullptr)
                    {
                        std::fprintf(Logger, "[Command] Failed to recv(MSG_WAITALL) (%d)\n", GetSocketError());
                        std::fflush(Logger);
                    }

                    return WaitError;
                }

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
                {
#ifdef _WIN32
                    if (GetSocketError() == WSAEINTR)
#else
                    if (GetSocketError() == EINTR)
#endif
                    {
                        if (Logger != nullptr)
                        {
                            std::fprintf(Logger, "[Command] recv(MSG_WAITALL) interrupted. Command(%hhu). Repeat call.\n", command);
                            std::fflush(Logger);
                        }

                        goto ReceiveBody;
                    }

                    if (Logger != nullptr)
                    {
                        std::fprintf(Logger, "[Command] Failed to recv(MSG_WAITALL) (%d)\n", GetSocketError());
                        std::fflush(Logger);
                    }

                    return WaitError;
                }

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
                {
#ifdef _WIN32
                    if (GetSocketError() == WSAEINTR)
#else
                    if (GetSocketError() == EINTR)
#endif
                    {
                        if (Logger != nullptr)
                        {
                            std::fprintf(Logger, "[Command] recv(MSG_WAITALL) interrupted. Command(%hhu). Repeat call.\n", command);
                            std::fflush(Logger);
                        }

                        goto ReceiveBody;
                    }

                    if (Logger != nullptr)
                    {
                        std::fprintf(Logger, "[Command] Failed to recv(MSG_WAITALL) (%d)\n", GetSocketError());
                        std::fflush(Logger);
                    }

                    return WaitError;
                }

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
                {
#ifdef _WIN32
                    if (GetSocketError() == WSAEINTR)
#else
                    if (GetSocketError() == EINTR)
#endif
                    {
                        if (Logger != nullptr)
                        {
                            std::fprintf(Logger, "[Command] recv(MSG_WAITALL) interrupted. Command(%hhu). Repeat call.\n", command);
                            std::fflush(Logger);
                        }

                        goto ReceiveBody;
                    }

                    if (Logger != nullptr)
                    {
                        std::fprintf(Logger, "[Command] Failed to recv(MSG_WAITALL) (%d)\n", GetSocketError());
                        std::fflush(Logger);
                    }

                    return WaitError;
                }

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
                {
#ifdef _WIN32
                    if (GetSocketError() == WSAEINTR)
#else
                    if (GetSocketError() == EINTR)
#endif
                    {
                        if (Logger != nullptr)
                        {
                            std::fprintf(Logger, "[Command] recv(MSG_WAITALL) interrupted. Command(%hhu). Repeat call.\n", command);
                            std::fflush(Logger);
                        }

                        goto ReceiveBody;
                    }

                    if (Logger != nullptr)
                    {
                        std::fprintf(Logger, "[Command] Failed to recv(MSG_WAITALL) (%d)\n", GetSocketError());
                        std::fflush(Logger);
                    }

                    return WaitError;
                }

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
                {
#ifdef _WIN32
                    if (GetSocketError() == WSAEINTR)
#else
                    if (GetSocketError() == EINTR)
#endif
                    {
                        if (Logger != nullptr)
                        {
                            std::fprintf(Logger, "[Command] recv(MSG_WAITALL) interrupted. Command(%hhu). Repeat call.\n", command);
                            std::fflush(Logger);
                        }

                        goto ReceiveBody;
                    }

                    if (Logger != nullptr)
                    {
                        std::fprintf(Logger, "[Command] Failed to recv(MSG_WAITALL) (%d)\n", GetSocketError());
                        std::fflush(Logger);
                    }

                    return WaitError;
                }

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
                {
#ifdef _WIN32
                    if (GetSocketError() == WSAEINTR)
#else
                    if (GetSocketError() == EINTR)
#endif
                    {
                        if (Logger != nullptr)
                        {
                            std::fprintf(Logger, "[Command] recv(MSG_WAITALL) interrupted. Command(%hhu). Repeat call.\n", command);
                            std::fflush(Logger);
                        }

                        goto ReceiveBody;
                    }

                    if (Logger != nullptr)
                    {
                        std::fprintf(Logger, "[Command] Failed to recv(MSG_WAITALL) (%d)\n", GetSocketError());
                        std::fflush(Logger);
                    }

                    return WaitError;
                }

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
                {
#ifdef _WIN32
                    if (GetSocketError() == WSAEINTR)
#else
                    if (GetSocketError() == EINTR)
#endif
                    {
                        if (Logger != nullptr)
                        {
                            std::fprintf(Logger, "[Command] recv(MSG_WAITALL) interrupted. Command(%hhu). Repeat call.\n", command);
                            std::fflush(Logger);
                        }

                        goto ReceiveBody;
                    }

                    if (Logger != nullptr)
                    {
                        std::fprintf(Logger, "[Command] Failed to recv(MSG_WAITALL) (%d)\n", GetSocketError());
                        std::fflush(Logger);
                    }

                    return WaitError;
                }

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
                {
#ifdef _WIN32
                    if (GetSocketError() == WSAEINTR)
#else
                    if (GetSocketError() == EINTR)
#endif
                    {
                        if (Logger != nullptr)
                        {
                            std::fprintf(Logger, "[Command] recv(MSG_WAITALL) interrupted. Command(%hhu). Repeat call.\n", command);
                            std::fflush(Logger);
                        }

                        goto ReceiveBody;
                    }

                    if (Logger != nullptr)
                    {
                        std::fprintf(Logger, "[Command] Failed to recv(MSG_WAITALL) (%d)\n", GetSocketError());
                        std::fflush(Logger);
                    }

                    return WaitError;
                }

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
                {
#ifdef _WIN32
                    if (GetSocketError() == WSAEINTR)
#else
                    if (GetSocketError() == EINTR)
#endif
                    {
                        if (Logger != nullptr)
                        {
                            std::fprintf(Logger, "[Command] recv(MSG_WAITALL) interrupted. Command(%hhu). Repeat call.\n", command);
                            std::fflush(Logger);
                        }

                        goto ReceiveBody;
                    }

                    if (Logger != nullptr)
                    {
                        std::fprintf(Logger, "[Command] Failed to recv(MSG_WAITALL) (%d)\n", GetSocketError());
                        std::fflush(Logger);
                    }

                    return WaitError;
                }

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&content.stream);
                }

                break;
            }
            default:
            {
                if (Logger != nullptr)
                {
                    std::fprintf(Logger, "[Command] Failed to recognize command(%hhu)\n", command);
                    std::fflush(Logger);
                }

                return WaitError;
            }
        }

        return command;
    }

public:

    std::FILE* Logger = nullptr;

private:

    IPv4TcpSocket _socket;

};
