/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cassert>

#include <system/types.hpp>
#include <system/time.hpp>
#include <system/clock.hpp>
#include <memory/view.hpp>
#include <network/socket.hpp>

#include <util/logger.hpp>

#include "main.hpp"

// Packets
// --------------------------------------------

#pragma pack(push, 1)

struct OutcomingVoiceHeader
{
    udword_t key;
    udword_t packet;
    udword_t channels;
};

/*
    struct KeepAlivePacket : OutcomingVoiceHeader
    {
        udword_t key
        udword_t packet   = player_id
        udword_t channels = 0
    }

    sizeof(KeepAlivePacket) == sizeof(OutcomingVoiceHeader)
*/

struct IncomingVoiceHeader
{
    udword_t key;
    udword_t packet;
     uword_t source;
     uword_t streams[];
};

/*
    struct ChangeKeyPacket : IncomingVoiceHeader
    {
        udword_t key
        udword_t packet = new_key
         uword_t source = 0
    }

    sizeof(ChangeKeyPacket) == sizeof(IncomingVoiceHeader)
*/

#pragma pack(pop)

// Constants
// --------------------------------------------

constexpr size_t kVoicePacketLimit = 512;

constexpr size_t kMinimumStreamsInPacket = 4;
constexpr size_t kMaximumStreamsInPacket = (kVoicePacketLimit -
    sizeof(IncomingVoiceHeader)) / sizeof(uword_t) - 1;

constexpr size_t kVoiceOutcomingContentLimit = kVoicePacketLimit - sizeof(OutcomingVoiceHeader);
constexpr size_t kVoiceIncomingContentLimit = kVoicePacketLimit -
    (sizeof(IncomingVoiceHeader) + (kMinimumStreamsInPacket + 1) * sizeof(uword_t));

constexpr size_t kVoiceContentLimit = std::min(kVoiceOutcomingContentLimit, kVoiceIncomingContentLimit);

constexpr size_t kVoiceOutcomingPacketLimit = kVoiceContentLimit + sizeof(OutcomingVoiceHeader);

// --------------------------------------------

constexpr Time kKeepAliveInterval = Time::Seconds(1);

// --------------------------------------------

struct VoiceService {

    VoiceService(const VoiceService&) = delete;
    VoiceService(VoiceService&&) = delete;
    VoiceService& operator=(const VoiceService&) = delete;
    VoiceService& operator=(VoiceService&&) = delete;

private:

    VoiceService() noexcept = default;
    ~VoiceService() noexcept = default;

public:

    static VoiceService& Instance() noexcept
    {
        static VoiceService instance;
        return instance;
    }

public:

    void Initiailize(const IPv4Address& address, const uword_t player) noexcept
    {
        _player = player;
        _address = address;
    }

    bool IsInitialized() const noexcept
    {
        return _player != None<uword_t>;
    }

    void Deinitialize() noexcept
    {
        _socket.Deinitialize();

        _player = None<uword_t>;
        _address = {};
    }

public:

    bool Open() noexcept
    {
        _socket.Deinitialize();

        IPv4UdpSocket socket;

        if (!socket.Initialize(true))
        {
            Logger::Instance().LogToFile("[err:voice:open] failed to initialize socket (%d)", GetSocketError());
            return false;
        }

        if (!socket.SetOption(SOL_SOCKET, SO_REUSEADDR, 1))
        {
            Logger::Instance().LogToFile("[err:voice:open] failed to set option(SO_REUSEADDR) (%d)", GetSocketError());
            return false;
        }

        if (!socket.Connect(_address))
        {
            char buffer[IPv4Address::LengthLimit + 1];
            if (!_address.Print(buffer)) std::strcpy(buffer, "INVALID");
            Logger::Instance().LogToFile("[err:voice:open] failed to connect(%s) (%d)", buffer, GetSocketError());
            return false;
        }

        _socket = std::move(socket);

        return true;
    }

    bool IsOpened() const noexcept
    {
        return _socket.Valid();
    }

    void Close() noexcept
    {
        _socket.Deinitialize();
    }

public:

    bool IsActive() const noexcept
    {
        return _channels != 0;
    }

public:

    void SetKey(const udword_t key) noexcept
    {
        if constexpr (HostEndian != NetEndian)
            reinterpret_cast<OutcomingVoiceHeader*>(_buffer)->key = utils::bswap(key);
        else reinterpret_cast<OutcomingVoiceHeader*>(_buffer)->key = key;
    }

    udword_t GetKey() const noexcept
    {
        if constexpr (HostEndian != NetEndian)
            return utils::bswap(reinterpret_cast<const OutcomingVoiceHeader*>(_buffer)->key);
        else return reinterpret_cast<const OutcomingVoiceHeader*>(_buffer)->key;
    }

public:

    DataView<ubyte_t> Buffer() noexcept
    {
        return { _buffer + sizeof(OutcomingVoiceHeader),
            sizeof(_buffer) - sizeof(OutcomingVoiceHeader) };
    }

public:

    void SetChannels(const udword_t channels) noexcept
    {
        if (_channels != 0 && channels == 0)
        {
            _packet &= HighByte<udword_t>;
            _packet += HighBit<udword_t> >> (CHAR_BIT - 1);
        }

        _channels = channels;
    }

    udword_t GetChannels() const noexcept
    {
        return _channels;
    }

public:

    bool SendPacket(const size_t length) noexcept
    {
        if (length > kVoiceContentLimit)
            return false;

        reinterpret_cast<OutcomingVoiceHeader*>(_buffer)->packet = _packet;
        reinterpret_cast<OutcomingVoiceHeader*>(_buffer)->channels = _channels;

        if constexpr (HostEndian != NetEndian)
        {
            utils::bswap(&reinterpret_cast<OutcomingVoiceHeader*>(_buffer)->packet);
            utils::bswap(&reinterpret_cast<OutcomingVoiceHeader*>(_buffer)->channels);
        }

        _packet += 1;

        return _socket.Send(_buffer, sizeof(OutcomingVoiceHeader) + length) ==
            sizeof(OutcomingVoiceHeader) + length;
    }

    size_t ReceivePacket(const ptr_t buffer) noexcept
    {
    NextPacket:

        const auto size = _socket.Receive(buffer, kVoicePacketLimit);
        if (size == SOCKET_ERROR)
        {
            if (GetSocketError() == WSAEWOULDBLOCK)
                return 0;

            if (GetSocketError() == WSAEMSGSIZE)
                goto NextPacket;

            return None<size_t>;
        }

        if (size < sizeof(IncomingVoiceHeader))
            return 0;

        assert(size <= kVoicePacketLimit);

        if constexpr (HostEndian != NetEndian)
        {
            utils::bswap(&static_cast<IncomingVoiceHeader*>(buffer)->key);
            utils::bswap(&static_cast<IncomingVoiceHeader*>(buffer)->packet);
            utils::bswap(&static_cast<IncomingVoiceHeader*>(buffer)->source);
        }

        if (static_cast<const IncomingVoiceHeader*>(buffer)->key != GetKey())
            goto NextPacket;

        if (size == sizeof(IncomingVoiceHeader)) // Change Key Packet
        {
            SetKey(static_cast<const IncomingVoiceHeader*>(buffer)->packet);
            goto NextPacket;
        }

        if constexpr (HostEndian != NetEndian)
        {
            for (auto iterator = static_cast<IncomingVoiceHeader*>(buffer)->streams;
                *iterator != None<uword_t>; ++iterator) utils::bswap(iterator);
        }

        return size;
    }

public:

    void Tick(const Time moment = Clock::Now()) noexcept
    {
        if (_clock.ExpiredAndRestart(kKeepAliveInterval, moment))
        {
            reinterpret_cast<OutcomingVoiceHeader*>(_buffer)->packet = _player;
            reinterpret_cast<OutcomingVoiceHeader*>(_buffer)->channels = 0;

            if constexpr (HostEndian != NetEndian)
            {
                utils::bswap(&reinterpret_cast<OutcomingVoiceHeader*>(_buffer)->packet);
            }

            _socket.Send(_buffer, sizeof(OutcomingVoiceHeader));
        }
    }

private:

    uword_t _player = None<uword_t>;

    udword_t _channels = 0;
    udword_t _packet = 0;

    ubyte_t _buffer[kVoiceOutcomingPacketLimit];

    IPv4Address _address;
    IPv4UdpSocket _socket;

    Clock _clock;

};
