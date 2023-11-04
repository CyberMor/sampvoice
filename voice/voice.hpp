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

#include <system/types.hpp>
#include <system/clock.hpp>
#include <memory/structures/pool.hpp>
#include <network/address.hpp>
#include <network/socket.hpp>
#include <other/utils.hpp>

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

        const auto [finded, offset] = Search(tag, _buffer[_index], _length[_index]);
        if (finded == false)
        {
            std::copy(_buffer[_index], _buffer[_index] + offset, _buffer[_index ^ 1]);
            std::copy(_buffer[_index] + offset, _buffer[_index] + _length[_index],
                _buffer[_index ^ 1] + offset + 1);

            _buffer[_index ^ 1][offset] = record;
        }

        // --------------------------------

        while (_lock.test_and_set(std::memory_order_acquire))
            std::this_thread::yield();

        if (finded == false)
        {
            _length[_index ^ 1] = _length[_index] + 1;
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

        const auto [finded, offset] = Search(tag, _buffer[index], _length[index]);
        const uword_t player = (finded == true ? GetPlayer(_buffer[index][offset]) : None<uword_t>);

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

        const auto [finded, offset] = Search(tag, _buffer[_index], _length[_index]);
        if (finded == true)
        {
            std::copy(_buffer[_index], _buffer[_index] + offset, _buffer[_index ^ 1]);
            std::copy(_buffer[_index] + offset + 1, _buffer[_index] + _length[_index],
                _buffer[_index ^ 1] + offset);
        }

        // --------------------------------

        while (_lock.test_and_set(std::memory_order_acquire))
            std::this_thread::yield();

        if (finded == true)
        {
            _length[_index ^ 1] = _length[_index] - 1;
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

        const auto offset = Search(player, _buffer[_index], _length[_index]);
        if (offset != None<decltype(offset)>)
        {
            std::copy(_buffer[_index], _buffer[_index] + offset, _buffer[_index ^ 1]);
            std::copy(_buffer[_index] + offset + 1, _buffer[_index] + _length[_index],
                _buffer[_index ^ 1] + offset);
        }

        // --------------------------------

        while (_lock.test_and_set(std::memory_order_acquire))
            std::this_thread::yield();

        if (offset != None<decltype(offset)>)
        {
            _length[_index ^ 1] = _length[_index] - 1;
            _index = _index ^ 1;
        }

        _writer = 0;

        _lock.clear(std::memory_order_release);

        // --------------------------------

        return offset != None<decltype(offset)>;
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

    static std::pair<bool, uword_t> Search(const uqword_t tag,
        const uqword_t* const buffer, const uword_t length) noexcept
    {
        assert(buffer != nullptr);

        if (length == 0) return { false, 0 };
        uword_t l = 0, r = length - 1; while (l != r)
        {
            const  uword_t m = l + (r - l) / 2;
            const uqword_t t = GetTag(buffer[m]);
            if (t == tag) return { true, m };
            if (t < tag) l = m + 1;
            else r = m - 1;
        } {
            const uqword_t t = GetTag(buffer[l]);
            return { t == tag, t < tag ? l + 1 : l };
        }
    }

    static uword_t Search(const uword_t player,
        const uqword_t* const buffer, const uword_t length) noexcept
    {
        assert(buffer != nullptr);

        for (uword_t i = 0; i != length; ++i)
        {
            if (GetPlayer(buffer[i]) == player)
                return i;
        }

        return None<uword_t>;
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

        mutable Spinlock lock;

        IPv4Address address;

        Clock last_packet_clock;

        Clock    key_reset_clock;
        udword_t key[2];

    };

public:

    bool Initialize(const IPv4Address& voice, std::FILE* const log = nullptr) noexcept
    {
        constexpr int kSndBufferSize = 4 * 1024 * 1024; // 4 MB (if possible)
        constexpr int kRcvBufferSize = 4 * 1024 * 1024; // 4 MB (if possible)

        _socket.Deinitialize();

        IPv4UdpSocket socket;

        if (!socket.Initialize(false)) return false;
        if (!socket.SetOption(SOL_SOCKET, SO_REUSEADDR, 1)) return false;
        if (int size; socket.GetOption(SOL_SOCKET, SO_SNDBUF, size) == 1)
            do size *= 2; while (size <= kSndBufferSize && socket.SetOption(SOL_SOCKET, SO_SNDBUF, size));
        if (int size; socket.GetOption(SOL_SOCKET, SO_RCVBUF, size) == 1)
            do size *= 2; while (size <= kRcvBufferSize && socket.SetOption(SOL_SOCKET, SO_RCVBUF, size));
        if (!socket.Bind(voice)) return false;

        if (log != nullptr)
        {
            if (int size; socket.GetOption(SOL_SOCKET, SO_SNDBUF, size) == 1)
                std::fprintf(log, "SO_SNDBUF set to %d bytes.\n", size);
            if (int size; socket.GetOption(SOL_SOCKET, SO_RCVBUF, size) == 1)
                std::fprintf(log, "SO_RCVBUF set to %d bytes.\n", size);
        }

        _socket = std::move(socket);

        _a2p_table.Clear();
        _players.Clear();

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
        constexpr Time kResetKeyCooldown = Time::Seconds(2);

        IPv4Address address;

    NextPacket:

        const auto size = _socket.ReceiveFrom(buffer, kVoiceIncomingPacketLimit, address);
        if (size == SOCKET_ERROR)
        {
#ifdef _WIN32
            if (GetSocketError() == WSAEMSGSIZE  ||
                GetSocketError() == WSAENETRESET ||
                GetSocketError() == WSAETIMEDOUT)
#else
            if (GetSocketError() == EMSGSIZE)
#endif
                goto NextPacket;

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

        sender = _a2p_table.Find(address);
        if (sender == None<uword_t>)
        {
            if (size == sizeof(IncomingVoiceHeader)) // Only Keep-Alive packet
            {
                if (header->packet < kMaxPlayers)
                {
                    if (_players.Acquire(header->packet))
                    {
                        const Time moment = Clock::Now();

                        _players[header->packet].lock.Lock();

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

                        _players[header->packet].lock.Unlock();

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

        const Time moment = Clock::Now();

        _players[sender].lock.Lock();

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
            if (_players[sender].key_reset_clock.ExpiredAndRestart(kResetKeyCooldown, moment))
            {
                old_key = _players[sender].GetActiveKey();
                do new_key = utils::crypto::random();
                while (new_key == 0 || new_key == old_key);
                _players[sender].ChangeKey(new_key);
            }
        }

        _players[sender].lock.Unlock();

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
        if (_players.Acquire(player) == false)
            return false;

        _players[player].lock.Lock();

        const udword_t    key     = _players[player].GetActiveKey();
        const IPv4Address address = _players[player].address;

        _players[player].lock.Unlock();

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
                _players[player].lock.Lock();

                if (_players[player].address.Valid())
                {
                    if (_players[player].last_packet_clock.Expired(kSilenceTimeout, moment))
                    {
                        _a2p_table.Remove(_players[player].address);

                        _players[player].address = {};
                    }
                }

                _players[player].lock.Unlock();
            }
        }
    }

private:

    IPv4UdpSocket _socket;
    Pool<PlayerInfo, kMaxPlayers> _players;
    AddressToPlayerTable _a2p_table;

};
