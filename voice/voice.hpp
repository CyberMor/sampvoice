/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <atomic>
#include <cassert>
#include <thread>
#include <mutex>

#include <system/types.hpp>
#include <system/clock.hpp>
#include <memory/structures/pool.hpp>
#include <network/address.hpp>
#include <network/socket.hpp>
#include <other/utils.hpp>

#include "logger.hpp"
#include "main.hpp"

// Packets
// --------------------------------------------

#pragma pack(push, 1)

struct IncomingVoiceHeader
{
    udword_t key;
    udword_t packet;
    udword_t channels;
};

/*
    struct KeepAlivePacket : IncomingVoiceHeader
    {
        udword_t key
        udword_t packet   = player_id
        udword_t channels = 0
    }

    sizeof(KeepAlivePacket) == sizeof(IncomingVoiceHeader)
*/

struct OutcomingVoiceHeader
{
    udword_t key;
    udword_t packet;
     uword_t source;
     uword_t streams[];
};

/*
    struct ChangeKeyPacket : OutcomingVoiceHeader
    {
        udword_t key
        udword_t packet = new_key
         uword_t source = 0
    }

    sizeof(ChangeKeyPacket) == sizeof(OutcomingVoiceHeader)
*/

#pragma pack(pop)

// Constants
// --------------------------------------------

constexpr size_t kVoicePacketLimit = 512;

constexpr size_t kMinimumStreamsInPacket = 4;
constexpr size_t kMaximumStreamsInPacket = (kVoicePacketLimit -
    sizeof(OutcomingVoiceHeader)) / sizeof(uword_t) - 1;

constexpr size_t kVoiceIncomingContentLimit = kVoicePacketLimit - sizeof(IncomingVoiceHeader);
constexpr size_t kVoiceOutcomingContentLimit = kVoicePacketLimit -
    (sizeof(OutcomingVoiceHeader) + (kMinimumStreamsInPacket + 1) * sizeof(uword_t));

constexpr size_t kVoiceContentLimit = std::min(kVoiceIncomingContentLimit, kVoiceOutcomingContentLimit);

constexpr size_t kVoiceIncomingPacketLimit = kVoiceContentLimit + sizeof(IncomingVoiceHeader);

// --------------------------------------------

constexpr Time kKeepAliveInterval = Time::Seconds(1);

// --------------------------------------------

struct AddressToPlayerTable {

    AddressToPlayerTable() noexcept = default;
    ~AddressToPlayerTable() noexcept = default;
    AddressToPlayerTable(const AddressToPlayerTable&) = delete;
    AddressToPlayerTable(AddressToPlayerTable&&) = delete;
    AddressToPlayerTable& operator=(const AddressToPlayerTable&) = delete;
    AddressToPlayerTable& operator=(AddressToPlayerTable&&) = delete;

public:

    bool Add(const IPv4Address& address, const uword_t player) noexcept
    {
        const uqword_t tag    = MakeTag    (address.GetHost(), address.GetPort());
        const uqword_t record = MakeRecord (address.GetHost(), address.GetPort(), player);

        // --------------------------------

        while (_lock.test_and_set(std::memory_order_acquire))
            std::this_thread::yield();

        while (_writer != 0)
        {
            _lock.clear(std::memory_order_release);
            do std::this_thread::yield();
            while (_lock.test_and_set(std::memory_order_acquire));
        }

        const bool is_full = _length[_index] == kMaxPlayers;
        if (is_full == false)
        {
            _writer = 1;

            while (_readers[_index ^ 1] != 0)
            {
                _lock.clear(std::memory_order_release);
                do std::this_thread::yield();
                while (_lock.test_and_set(std::memory_order_acquire));
            }
        }

        _lock.clear(std::memory_order_release);

        if (is_full == true) return false;

        // --------------------------------

        const auto length = _length[_index];
        const auto offset = SearchByTag(tag, _buffer[_index], length);
        const bool finded = offset != length && GetTag(_buffer[_index][offset]) == tag;
        if (finded == false)
        {
            std::copy(_buffer[_index], _buffer[_index] + offset, _buffer[_index ^ 1]);
            std::copy(_buffer[_index] + offset, _buffer[_index] + length,
                _buffer[_index ^ 1] + offset + 1);

            _buffer[_index ^ 1][offset] = record;
        }

        // --------------------------------

        while (_lock.test_and_set(std::memory_order_acquire))
            std::this_thread::yield();

        if (finded == false)
        {
            _length[_index ^ 1] = length + 1;
            _index = _index ^ 1;
        }

        _writer = 0;

        _lock.clear(std::memory_order_release);

        // --------------------------------

        return finded == false;
    }

    uword_t Find(const IPv4Address& address) const noexcept
    {
        const uqword_t tag = MakeTag(address.GetHost(), address.GetPort());

        // --------------------------------

        while (_lock.test_and_set(std::memory_order_acquire))
            std::this_thread::yield();

        const uword_t index = _index;

        const bool is_empty = _length[index] == 0;
        if (is_empty == false) ++_readers[index];

        _lock.clear(std::memory_order_release);

        if (is_empty == true) return None<uword_t>;

        // --------------------------------

        const auto length = _length[index];
        const auto offset = SearchByTag(tag, _buffer[index], length);
        const bool finded = offset != length && GetTag(_buffer[index][offset]) == tag;

        const uword_t player = (finded ? GetPlayer(_buffer[index][offset]) : None<uword_t>);

        // --------------------------------

        while (_lock.test_and_set(std::memory_order_acquire))
            std::this_thread::yield();

        --_readers[index];

        _lock.clear(std::memory_order_release);

        // --------------------------------

        return player;
    }

    bool Remove(const IPv4Address& address) noexcept
    {
        const uqword_t tag = MakeTag(address.GetHost(), address.GetPort());

        // --------------------------------

        while (_lock.test_and_set(std::memory_order_acquire))
            std::this_thread::yield();

        while (_writer != 0)
        {
            _lock.clear(std::memory_order_release);
            do std::this_thread::yield();
            while (_lock.test_and_set(std::memory_order_acquire));
        }

        const bool is_empty = _length[_index] == 0;
        if (is_empty == false)
        {
            _writer = 1;

            while (_readers[_index ^ 1] != 0)
            {
                _lock.clear(std::memory_order_release);
                do std::this_thread::yield();
                while (_lock.test_and_set(std::memory_order_acquire));
            }
        }

        _lock.clear(std::memory_order_release);

        if (is_empty == true) return false;

        // --------------------------------

        const auto length = _length[_index];
        const auto offset = SearchByTag(tag, _buffer[_index], length);
        const bool finded = offset != length && GetTag(_buffer[_index][offset]) == tag;
        if (finded == true)
        {
            std::copy(_buffer[_index], _buffer[_index] + offset, _buffer[_index ^ 1]);
            std::copy(_buffer[_index] + offset + 1, _buffer[_index] + length,
                _buffer[_index ^ 1] + offset);
        }

        // --------------------------------

        while (_lock.test_and_set(std::memory_order_acquire))
            std::this_thread::yield();

        if (finded == true)
        {
            _length[_index ^ 1] = length - 1;
            _index = _index ^ 1;
        }

        _writer = 0;

        _lock.clear(std::memory_order_release);

        // --------------------------------

        return finded == true;
    }

    bool Remove(const uword_t player) noexcept
    {
        // --------------------------------

        while (_lock.test_and_set(std::memory_order_acquire))
            std::this_thread::yield();

        while (_writer != 0)
        {
            _lock.clear(std::memory_order_release);
            do std::this_thread::yield();
            while (_lock.test_and_set(std::memory_order_acquire));
        }

        const bool is_empty = _length[_index] == 0;
        if (is_empty == false)
        {
            _writer = 1;

            while (_readers[_index ^ 1] != 0)
            {
                _lock.clear(std::memory_order_release);
                do std::this_thread::yield();
                while (_lock.test_and_set(std::memory_order_acquire));
            }
        }

        _lock.clear(std::memory_order_release);

        if (is_empty == true) return false;

        // --------------------------------

        const auto length = _length[_index];
        const auto offset = SearchByPlayer(player, _buffer[_index], length);
        const bool finded = offset != length;
        if (finded == true)
        {
            std::copy(_buffer[_index], _buffer[_index] + offset, _buffer[_index ^ 1]);
            std::copy(_buffer[_index] + offset + 1, _buffer[_index] + length,
                _buffer[_index ^ 1] + offset);
        }

        // --------------------------------

        while (_lock.test_and_set(std::memory_order_acquire))
            std::this_thread::yield();

        if (finded == true)
        {
            _length[_index ^ 1] = length - 1;
            _index = _index ^ 1;
        }

        _writer = 0;

        _lock.clear(std::memory_order_release);

        // --------------------------------

        return finded == true;
    }

public:

    void Clear() noexcept
    {
        while (_lock.test_and_set(std::memory_order_acquire))
            std::this_thread::yield();

        while (_writer != 0)
        {
            _lock.clear(std::memory_order_release);
            do std::this_thread::yield();
            while (_lock.test_and_set(std::memory_order_acquire));
        }

        _writer = 1;

        while (_readers[_index ^ 1] != 0)
        {
            _lock.clear(std::memory_order_release);
            do std::this_thread::yield();
            while (_lock.test_and_set(std::memory_order_acquire));
        }

        _length[_index ^ 1] = 0;
        _index = _index ^ 1;

        _writer = 0;

        _lock.clear(std::memory_order_release);
    }

private:

    static uword_t SearchByTag(const uqword_t tag, const uqword_t* buffer, uword_t length) noexcept
    {
        assert(buffer != nullptr);

        const auto begin = buffer;

        while (length != 0)
        {
            const auto remainder = length % 2;

            length /= 2;

            if (GetTag(buffer[length]) < tag)
            {
                buffer += length + remainder;
            }
        }

        return buffer - begin;
    }

    static uword_t SearchByPlayer(const uword_t player, const uqword_t* const buffer, const uword_t length) noexcept
    {
        assert(buffer != nullptr);

        uword_t offset = 0;

        while (offset != length && GetPlayer(buffer[offset]) != player)
            ++offset;

        return offset;
    }

private:

    static constexpr uqword_t MakeRecord(const udword_t host, const uword_t port, const uword_t player) noexcept
    {
        return static_cast<uqword_t>(player) << 48 |
               static_cast<uqword_t>(port)   << 32 |
               static_cast<uqword_t>(host);
    }

    static constexpr uqword_t MakeTag(const udword_t host, const uword_t port) noexcept
    {
        return static_cast<uqword_t>(port) << 32 |
               static_cast<uqword_t>(host);
    }

    static constexpr uqword_t GetTag(const uqword_t record) noexcept
    {
        return record & static_cast<uqword_t>(0xFFFFFFFFFFFF);
    }

    static constexpr uword_t GetPlayer(const uqword_t record) noexcept
    {
        return static_cast<uword_t>(record >> 48);
    }

private:

    alignas (CACHE_LINE)
    mutable std::atomic_flag _lock = ATOMIC_FLAG_INIT;

    mutable uword_t _writer = false;
    mutable uword_t _readers[2] = {};

    uword_t _index = 0;
    uword_t _length[2] = {};

    alignas (CACHE_LINE)
    uqword_t _buffer[2][kMaxPlayers];

};

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

private:

    struct alignas(CACHE_LINE) PlayerInfo {

        PlayerInfo() noexcept = default;
        ~PlayerInfo() noexcept = default;

    public:

        PlayerInfo(const udword_t key) noexcept
            : key { key, key }
        {}

    public:

        udword_t GetActiveKey() const noexcept
        {
            return key[0];
        }

        udword_t GetRequiredKey() const noexcept
        {
            return key[1];
        }

    public:

        bool IsKeyReplacement() const noexcept
        {
            return key[0] != key[1];
        }

    public:

        void ChangeKey(const udword_t new_key) noexcept
        {
            key[1] = new_key;
        }

        void ApplyRequiredKey() noexcept
        {
            key[0] = key[1];
        }

    public:

        mutable std::mutex lock;

        IPv4Address address;

        Clock last_packet_clock;

        Clock    key_reset_clock;
        udword_t key[2];

    };

public:

    bool Initialize(const IPv4Address& voice) noexcept
    {
        assert(voice.Valid());

        constexpr int kSndBufferSize = 4 * 1024 * 1024; // 4 MB (if possible)
        constexpr int kRcvBufferSize = 4 * 1024 * 1024; // 4 MB (if possible)

        _socket.Deinitialize();

        _a2p_table.Clear();
        _players.Clear();

        IPv4UdpSocket socket;

        if (!socket.Initialize(false))
        {
            if (Logger != nullptr)
            {
                Logger::Instance().Log("[Voice] Failed to initialize socket (%d)", GetSocketError());
            }

            return false;
        }

        if (!socket.SetOption(SOL_SOCKET, SO_REUSEADDR, 1))
        {
            if (Logger != nullptr)
            {
                Logger::Instance().Log("[Voice] Failed to set option(SO_REUSEADDR) (%d)", GetSocketError());
            }

            return false;
        }

        if (int current, value = kSndBufferSize; socket.GetOption(SOL_SOCKET, SO_SNDBUF, current) == 1)
            while (value > current && !socket.SetOption(SOL_SOCKET, SO_SNDBUF, value)) value /= 2;
        if (int current, value = kRcvBufferSize; socket.GetOption(SOL_SOCKET, SO_RCVBUF, current) == 1)
            while (value > current && !socket.SetOption(SOL_SOCKET, SO_RCVBUF, value)) value /= 2;

        if (!socket.Bind(voice))
        {
            if (Logger != nullptr)
            {
                char buffer[IPv4Address::LengthLimit + 1];

                if (!voice.Print(buffer))
                {
                    buffer[0] = 'I'; buffer[1] = 'N'; buffer[2] = 'V'; buffer[3] = 'A';
                    buffer[4] = 'L'; buffer[5] = 'I'; buffer[6] = 'D'; buffer[7] = '\0';
                }

                Logger::Instance().Log("[Voice] Failed to bind(%s) (%d)", buffer, GetSocketError());
            }

            return false;
        }

        if (Logger != nullptr)
        {
            if (int size; socket.GetOption(SOL_SOCKET, SO_SNDBUF, size) == 1)
                Logger::Instance().Log("[Voice] set option(SO_SNDBUF) to %d bytes.", size);
            if (int size; socket.GetOption(SOL_SOCKET, SO_RCVBUF, size) == 1)
                Logger::Instance().Log("[Voice] set option(SO_RCVBUF) to %d bytes.", size);
        }

        _socket = std::move(socket);

        return true;
    }

    void Deinitialize() noexcept
    {
        _socket.Deinitialize();

        _a2p_table.Clear();
        _players.Clear();
    }

public:

    bool RegisterPlayer(const uword_t player, const udword_t key) noexcept
    {
        assert(key != 0);

        return _players.EmplaceAt(player, true, key);
    }

    void RemovePlayer(const uword_t player) noexcept
    {
        _players.Remove(player, true);
        _a2p_table.Remove(player);
    }

public:

    size_t ReceivePacket(const ptr_t buffer, uword_t& sender) noexcept
    {
        assert(buffer != nullptr);

        IPv4Address address;

    NextPacket:

        const auto size = _socket.ReceiveFrom(buffer, kVoiceIncomingPacketLimit, address);
        if (size <= 0)
        {
#ifdef _WIN32
            if (GetSocketError() == WSAEMSGSIZE  ||
                GetSocketError() == WSAEINTR     ||
                GetSocketError() == WSAENETRESET ||
                GetSocketError() == WSAECONNRESET)
#else
            if (GetSocketError() == EMSGSIZE ||
                GetSocketError() == EINTR)
#endif
                goto NextPacket;

            if (Logger != nullptr)
            {
                Logger::Instance().Log("[Voice] Failed to recvfrom (%d)", GetSocketError());
            }

            return 0;
        }

        if (size < sizeof(IncomingVoiceHeader) || size > kVoiceIncomingPacketLimit)
            goto NextPacket;

        const auto header = static_cast<IncomingVoiceHeader*>(buffer);
        if (header->key == 0) goto NextPacket;

        if constexpr (HostEndian != NetEndian)
        {
            utils::bswap(&header->key);
            utils::bswap(&header->packet);
            utils::bswap(&header->channels);
        }

        const Time moment = Clock::Now();

        sender = _a2p_table.Find(address);
        if (sender == None<uword_t>)
        {
            if (size == sizeof(IncomingVoiceHeader)) // Only Keep-Alive packet
            {
                if (header->packet < kMaxPlayers)
                {
                    if (_players.Acquire(header->packet))
                    {
                        _players[header->packet].lock.lock();

                        if (_players[header->packet].address.Invalid())
                        {
                            if (header->key == _players[header->packet].GetActiveKey() ||
                                header->key == _players[header->packet].GetRequiredKey())
                            {
                                if (_a2p_table.Add(address, header->packet))
                                {
                                    _players[header->packet].last_packet_clock.Restart(moment);
                                    _players[header->packet].address = address;
                                }
                            }
                        }

                        _players[header->packet].lock.unlock();

                        _players.Release(header->packet);
                    }
                }
            }

            goto NextPacket;
        }

        if (_players.Acquire(sender) == false)
            goto NextPacket;

        bool barrier = true;

        udword_t old_key;
        udword_t new_key = 0;

        _players[sender].lock.lock();

        if (header->key == _players[sender].GetActiveKey())
        {
            _players[sender].last_packet_clock.Restart(moment);

            if (_players[sender].IsKeyReplacement())
            {
                old_key = _players[sender].GetActiveKey();
                new_key = _players[sender].GetRequiredKey();
            }

            barrier = false;
        }
        else if (header->key == _players[sender].GetRequiredKey())
        {
            _players[sender].last_packet_clock.Restart(moment);

            _players[sender].ApplyRequiredKey();

            barrier = false;
        }
        else
        {
            constexpr Time kResetKeyCooldown = Time::Seconds(2);

            if (_players[sender].key_reset_clock.ExpiredAndRestart(kResetKeyCooldown, moment))
            {
                old_key = _players[sender].GetActiveKey();
                do new_key = utils::crypto::random();
                while (new_key == 0 || new_key == old_key);
                _players[sender].ChangeKey(new_key);
            }
        }

        _players[sender].lock.unlock();

        _players.Release(sender);

        if (new_key != 0)
        {
            OutcomingVoiceHeader change_key_packet;

            if constexpr (HostEndian != NetEndian)
                change_key_packet.key = utils::bswap(old_key);
            else change_key_packet.key = old_key;

            if constexpr (HostEndian != NetEndian)
                change_key_packet.packet = utils::bswap(new_key);
            else change_key_packet.packet = new_key;

            change_key_packet.source = 0;

            _socket.SendValueTo(change_key_packet, address);
        }

        if (barrier) goto NextPacket;
        if (size == sizeof(IncomingVoiceHeader)) // Keep-Alive packet
            goto NextPacket;

        return size;
    }

    bool SendPacket(const uword_t player, const ptr_t data, const size_t size) noexcept
    {
        assert(size == 0 || data != nullptr);

        if (_players.Acquire(player) == false)
            return false;

        _players[player].lock.lock();

        const udword_t    key     = _players[player].GetActiveKey();
        const IPv4Address address = _players[player].address;

        _players[player].lock.unlock();

        _players.Release(player);

        if (address.Invalid()) return false;

        if constexpr (HostEndian != NetEndian)
            static_cast<OutcomingVoiceHeader*>(data)->key = utils::bswap(key);
        else static_cast<OutcomingVoiceHeader*>(data)->key = key;

        return _socket.SendTo(data, size, address) == size;
    }

public:

    void Tick(const Time moment = Clock::Now()) noexcept
    {
        constexpr Time kSilenceTimeout = kKeepAliveInterval * 3;

        for (size_t player = 0; player != kMaxPlayers; ++player)
        {
            if (_players.Acquire<0>(player))
            {
                _players[player].lock.lock();

                if (_players[player].address.Valid())
                {
                    if (_players[player].last_packet_clock.Expired(kSilenceTimeout, moment))
                    {
                        _a2p_table.Remove(_players[player].address);
                        _players[player].address = {};
                    }
                }

                _players[player].lock.unlock();
            }
        }
    }

public:

    std::FILE* Logger = nullptr;

private:

    IPv4UdpSocket _socket;
    Pool<PlayerInfo, kMaxPlayers> _players;
    AddressToPlayerTable _a2p_table;

};
