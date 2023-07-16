/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <system/types.hpp>
#include <memory/packet.hpp>

#include "header.hpp"
#include "address.hpp"
#include "poll.hpp"

// ----------------------------------------------------------

template <int Domain, int Type, int Protocol = GetDefaultProtocol(Domain, Type)>
struct Socket {

    Socket() noexcept = default;
    ~Socket() noexcept
    {
        if (_handle != INVALID_SOCKET)
            CloseSocket(_handle);
    }

    Socket(const Socket&) = delete;
    Socket(Socket&& object) noexcept
        : _handle { object._handle }
    {
        object._handle = INVALID_SOCKET;
    }

    Socket& operator=(const Socket&) = delete;
    Socket& operator=(Socket&& object) noexcept
    {
        if (&object != this)
        {
            if (_handle != INVALID_SOCKET)
                CloseSocket(_handle);

            _handle = object._handle;

            object._handle = INVALID_SOCKET;
        }

        return *this;
    }

private:

    Socket(const SOCKET handle) noexcept
        : _handle { handle }
    {}

public:

    Socket(const bool nonblock) noexcept
        : _handle
        {
            socket
            (
                Domain,
                Type
#ifndef _WIN32
                | (nonblock ? SOCK_NONBLOCK : 0)
#endif
                , Protocol
            )
        }
    {
#ifdef _WIN32
        if (nonblock)
        {
            if (_handle != INVALID_SOCKET)
            {
                if (auto value = static_cast<u_long>(true); ioctlsocket(_handle, FIONBIO, &value) != 0)
                {
                    CloseSocket(_handle);
                    _handle = INVALID_SOCKET;
                }
            }
        }
#endif
    }

public:

    bool Valid() const noexcept
    {
        return _handle != INVALID_SOCKET;
    }

    bool Invalid() const noexcept
    {
        return _handle == INVALID_SOCKET;
    }

public:

    SOCKET Handle() const noexcept
    {
        return _handle;
    }

public:

    bool Initialize(const bool nonblock) noexcept
    {
        if (_handle != INVALID_SOCKET)
            CloseSocket(_handle);
#ifdef _WIN32
        _handle = socket(Domain, Type, Protocol);
        if (nonblock)
        {
            if (_handle != INVALID_SOCKET)
            {
                if (auto value = static_cast<u_long>(true); ioctlsocket(_handle, FIONBIO, &value) != 0)
                {
                    CloseSocket(_handle);
                    _handle = INVALID_SOCKET;
                }
            }
        }
#else
        _handle = socket(Domain, Type | (nonblock ? SOCK_NONBLOCK : 0), Protocol);
#endif
        return _handle != INVALID_SOCKET;
    }

    void Deinitialize() noexcept
    {
        if (_handle != INVALID_SOCKET)
        {
            CloseSocket(_handle);
            _handle = INVALID_SOCKET;
        }
    }

    void Release() noexcept
    {
        _handle = INVALID_SOCKET;
    }

public:

    bool SetOption(const int level, const int option, const cptr_t data, const socklen_t size) noexcept
    {
        return setsockopt(_handle, level, option, static_cast<const char*>(data), size) == 0;
    }

    template <class DataType>
    bool SetOption(const int level, const int option, const DataType* const data, const socklen_t size) noexcept
    {
        return SetOption(level, option, static_cast<cptr_t>(data), size * sizeof(DataType));
    }

    template <class Value>
    bool SetOption(const int level, const int option, const Value& value) noexcept
    {
        return SetOption(level, option, reinterpret_cast<const std::remove_all_extents_t<Value>*>(&value),
            sizeof(value) / sizeof(std::remove_all_extents_t<Value>));
    }

    socklen_t GetOption(const int level, const int option, const ptr_t buffer, socklen_t length) const noexcept
    {
        if (getsockopt(_handle, level, option, static_cast<char*>(buffer), &length) == 0)
            return length;

        return 0;
    }

    template <class DataType, typename = std::enable_if_t<!std::is_const_v<DataType>>>
    socklen_t GetOption(const int level, const int option, DataType* const buffer, const socklen_t length) const noexcept
    {
        return GetOption(level, option, static_cast<ptr_t>(buffer), length * sizeof(DataType)) / sizeof(DataType);
    }

    template <class Buffer, typename = std::enable_if_t<!std::is_const_v<std::remove_all_extents_t<Buffer>>>>
    socklen_t GetOption(const int level, const int option, Buffer& buffer) const noexcept
    {
        return GetOption(level, option, reinterpret_cast<std::remove_all_extents_t<Buffer>*>(&buffer),
            sizeof(buffer) / sizeof(std::remove_all_extents_t<Buffer>));
    }

public:

    bool GetLocalAddress(Address<Domain>& address) const noexcept
    {
        socklen_t address_size = address.Size();
        return getsockname(_handle, address.Data(), &address_size) == 0 &&
            address_size == address.Size();
    }

    bool GetRemoteAddress(Address<Domain>& address) const noexcept
    {
        socklen_t address_size = address.Size();
        return getpeername(_handle, address.Data(), &address_size) == 0 &&
            address_size == address.Size();
    }

public:

    bool SetNonBlocking(const bool state) noexcept
    {
#ifdef _WIN32
        auto value = static_cast<u_long>(state);

        return ioctlsocket(_handle, FIONBIO, &value) == 0;
#else
        int flags = fcntl(_handle, F_GETFL);
        if (flags == -1) return false;

        if (state) flags |=  O_NONBLOCK;
        else       flags &= ~O_NONBLOCK;

        return fcntl(_handle, F_SETFL, flags) != -1;
#endif
    }

public:

    bool Bind(const Address<Domain>& address) noexcept
    {
        return bind(_handle, address.Data(), address.Size()) == 0;
    }

    bool Bind(const cstr_t host, const uword_t port) noexcept
    {
        const auto callback = [this](int, int, int, const Address<Domain>& address) noexcept -> bool
            { return Bind(address); };

        return ResolveAddress<Domain, Type, Protocol>(host, port, callback);
    }

    bool Connect(const Address<Domain>& address) noexcept
    {
        return connect(_handle, address.Data(), address.Size()) == 0;
    }

    bool Connect(const cstr_t host, const uword_t port) noexcept
    {
        const auto callback = [this](int, int, int, const Address<Domain>& address) noexcept -> bool
            { return Connect(address); };

        return ResolveAddress<Domain, Type, Protocol>(host, port, callback);
    }

    bool Connect(const Address<Domain>& address, const Time timeout) noexcept
    {
#ifdef _WIN32
        return Connect(address) || ((GetSocketError() == WSAEWOULDBLOCK || GetSocketError() == WSAEINPROGRESS) &&
            Wait(timeout, POLLOUT) == 1 && Status() == 0);
#else
        return Connect(address) || (GetSocketError() == EINPROGRESS &&
            Wait(timeout, POLLOUT) == 1 && Status() == 0);
#endif
    }

    bool Connect(const cstr_t host, const uword_t port, const Time timeout) noexcept
    {
        const auto callback = [this, timeout](int, int, int, const Address<Domain>& address) noexcept -> bool
            { return Connect(address, timeout); };

        return ResolveAddress<Domain, Type, Protocol>(host, port, callback);
    }

    bool Listen(const int queue = SOMAXCONN) noexcept
    {
        return listen(_handle, queue) == 0;
    }

public:

    int ReadingAvailable() const noexcept
    {
#ifdef _WIN32
        if (u_long bytes; ioctlsocket(_handle, FIONREAD, &bytes) == 0)
            return static_cast<int>(bytes);
#else
        if (int bytes; ioctl(_handle, SIOCINQ, &bytes) != -1)
            return bytes;
#endif
        return -1;
    }

    int WritingAvailable() const noexcept
    {
#ifndef _WIN32
        if (int bytes; ioctl(_handle, SIOCOUTQ, &bytes) != -1)
            return bytes;
#endif
        return -1;
    }

public:

    Socket Accept(const bool nonblock) noexcept
    {
#ifdef _WIN32
        SOCKET handle = accept(_handle, nullptr, nullptr);
        if (nonblock)
        {
            if (handle != INVALID_SOCKET)
            {
                if (auto value = static_cast<u_long>(true); ioctlsocket(handle, FIONBIO, &value) != 0)
                {
                    CloseSocket(handle);
                    handle = INVALID_SOCKET;
                }
            }
        }
#else
        const SOCKET handle = accept4(_handle, nullptr, nullptr, nonblock ? SOCK_NONBLOCK : 0);
#endif
        return handle;
    }

    Socket Accept(Address<Domain>& address, const bool nonblock) noexcept
    {
        socklen_t address_size = address.Size();
#ifdef _WIN32
        SOCKET handle = accept(_handle, address.Data(), &address_size);
        if (nonblock)
        {
            if (handle != INVALID_SOCKET)
            {
                if (auto value = static_cast<u_long>(true); ioctlsocket(handle, FIONBIO, &value) != 0)
                {
                    CloseSocket(handle);
                    handle = INVALID_SOCKET;
                }
            }
        }
#else
        const SOCKET handle = accept4(_handle, address.Data(), &address_size, nonblock ? SOCK_NONBLOCK : 0);
#endif
        assert(handle == INVALID_SOCKET || address_size <= address.Size());
        return handle;
    }

public:

    auto Peek(const ptr_t buffer, const size_t length, const int flags = 0) const noexcept
    {
        return recv(_handle, static_cast<char*>(buffer), length, flags | MSG_PEEK);
    }

    template <class DataType, typename = std::enable_if_t<!std::is_const_v<DataType>>>
    auto Peek(DataType* const buffer, const size_t length, const int flags = 0) const noexcept
    {
        const auto result = Peek(static_cast<ptr_t>(buffer), length * sizeof(DataType), flags);
        return result != static_cast<decltype(result)>(SOCKET_ERROR)
             ? result / static_cast<decltype(result)>(sizeof(DataType))
             : static_cast<decltype(result)>(SOCKET_ERROR);
    }

    template <class Buffer, typename = std::enable_if_t<!std::is_const_v<std::remove_all_extents_t<Buffer>>>>
    auto PeekValue(Buffer& buffer, const int flags = 0) const noexcept
    {
        return Peek(reinterpret_cast<std::remove_all_extents_t<Buffer>*>(&buffer),
            sizeof(buffer) / sizeof(std::remove_all_extents_t<Buffer>), flags);
    }

public:

    auto Receive(const ptr_t buffer, const size_t length, const int flags = 0) noexcept
    {
        return recv(_handle, static_cast<char*>(buffer), length, flags);
    }

    template <class DataType, typename = std::enable_if_t<!std::is_const_v<DataType>>>
    auto Receive(DataType* const buffer, const size_t length, const int flags = 0) noexcept
    {
        const auto result = Receive(static_cast<ptr_t>(buffer), length * sizeof(DataType), flags);
        return result != static_cast<decltype(result)>(SOCKET_ERROR)
             ? result / static_cast<decltype(result)>(sizeof(DataType))
             : static_cast<decltype(result)>(SOCKET_ERROR);
    }

    template <class Buffer, typename = std::enable_if_t<!std::is_const_v<std::remove_all_extents_t<Buffer>>>>
    auto ReceiveValue(Buffer& buffer, const int flags = 0) noexcept
    {
        return Receive(reinterpret_cast<std::remove_all_extents_t<Buffer>*>(&buffer),
            sizeof(buffer) / sizeof(std::remove_all_extents_t<Buffer>), flags);
    }

public:

    auto Send(const cptr_t data, const size_t size, const int flags = 0) noexcept
    {
        return send(_handle, static_cast<const char*>(data), size, flags);
    }

    template <class DataType>
    auto Send(const DataType* const data, const size_t size, const int flags = 0) noexcept
    {
        const auto result = Send(static_cast<cptr_t>(data), size * sizeof(DataType), flags);
        return result != static_cast<decltype(result)>(SOCKET_ERROR)
             ? result / static_cast<decltype(result)>(sizeof(DataType))
             : static_cast<decltype(result)>(SOCKET_ERROR);
    }

    template <class Value>
    auto SendValue(const Value& value, const int flags = 0) noexcept
    {
        return Send(reinterpret_cast<const std::remove_all_extents_t<Value>*>(&value),
            sizeof(value) / sizeof(std::remove_all_extents_t<Value>), flags);
    }

public:

    template <bool Endian = NetEndian, class... Buffers>
    int PeekPacket(Buffers&... buffers) const noexcept
    {
        DataPacket<Buffers...> packet;
        { const auto result = PeekValue(packet, MSG_WAITALL);
            if (result == SOCKET_ERROR) return -1;
            if (result != 1) return 0; }
        packet.template Get<Endian>(buffers...);
        return 1;
    }

    template <bool Endian = NetEndian, class... Buffers>
    int ReceivePacket(Buffers&... buffers) noexcept
    {
        DataPacket<Buffers...> packet;
        { const auto result = ReceiveValue(packet, MSG_WAITALL);
            if (result == SOCKET_ERROR) return -1;
            if (result != 1) return 0; }
        packet.template Get<Endian>(buffers...);
        return 1;
    }

    template <bool Endian = NetEndian, class... Values>
    int SendPacket(const Values... values) noexcept
    {
        DataPacket<Values...> packet;
        packet.template Set<Endian>(values...);
        { const auto result = SendValue(packet);
            if (result == SOCKET_ERROR) return -1;
            if (result != 1) return 0; }
        return 1;
    }

public:

    auto PeekFrom(const ptr_t buffer, const size_t length,
        Address<Domain>& address, const int flags = 0) const noexcept
    {
        socklen_t address_size = address.Size();
        const auto bytes = recvfrom(_handle, static_cast<char*>(buffer),
            length, flags | MSG_PEEK, address.Data(), &address_size);
        assert(bytes == SOCKET_ERROR || address_size <= address.Size());
        return bytes;
    }

    template <class DataType, typename = std::enable_if_t<!std::is_const_v<DataType>>>
    auto PeekFrom(DataType* const buffer, const size_t length,
        Address<Domain>& address, const int flags = 0) const noexcept
    {
        const auto result = PeekFrom(static_cast<ptr_t>(buffer), length * sizeof(DataType), address, flags);
        return result != static_cast<decltype(result)>(SOCKET_ERROR)
             ? result / static_cast<decltype(result)>(sizeof(DataType))
             : static_cast<decltype(result)>(SOCKET_ERROR);
    }

    template <class Buffer, typename = std::enable_if_t<!std::is_const_v<std::remove_all_extents_t<Buffer>>>>
    auto PeekValueFrom(Buffer& buffer, Address<Domain>& address, const int flags = 0) const noexcept
    {
        return PeekFrom(reinterpret_cast<std::remove_all_extents_t<Buffer>*>(&buffer),
            sizeof(buffer) / sizeof(std::remove_all_extents_t<Buffer>), address, flags);
    }

public:

    auto ReceiveFrom(const ptr_t buffer, const size_t length,
        Address<Domain>& address, const int flags = 0) noexcept
    {
        socklen_t address_size = address.Size();
        const auto bytes = recvfrom(_handle, static_cast<char*>(buffer),
            length, flags, address.Data(), &address_size);
        assert(bytes == SOCKET_ERROR || address_size <= address.Size());
        return bytes;
    }

    template <class DataType, typename = std::enable_if_t<!std::is_const_v<DataType>>>
    auto ReceiveFrom(DataType* const buffer, const size_t length,
        Address<Domain>& address, const int flags = 0) noexcept
    {
        const auto result = ReceiveFrom(static_cast<ptr_t>(buffer), length * sizeof(DataType), address, flags);
        return result != static_cast<decltype(result)>(SOCKET_ERROR)
             ? result / static_cast<decltype(result)>(sizeof(DataType))
             : static_cast<decltype(result)>(SOCKET_ERROR);
    }

    template <class Buffer, typename = std::enable_if_t<!std::is_const_v<std::remove_all_extents_t<Buffer>>>>
    auto ReceiveValueFrom(Buffer& buffer, Address<Domain>& address, const int flags = 0) noexcept
    {
        return ReceiveFrom(reinterpret_cast<std::remove_all_extents_t<Buffer>*>(&buffer),
            sizeof(buffer) / sizeof(std::remove_all_extents_t<Buffer>), address, flags);
    }

public:

    auto SendTo(const cptr_t data, const size_t size,
        const Address<Domain>& address, const int flags = 0) noexcept
    {
        return sendto(_handle, static_cast<const char*>(data),
            size, flags, address.Data(), address.Size());
    }

    template <class DataType>
    auto SendTo(const DataType* const data, const size_t size,
        const Address<Domain>& address, const int flags = 0) noexcept
    {
        const auto result = SendTo(static_cast<cptr_t>(data), size * sizeof(DataType), address, flags);
        return result != static_cast<decltype(result)>(SOCKET_ERROR)
             ? result / static_cast<decltype(result)>(sizeof(DataType))
             : static_cast<decltype(result)>(SOCKET_ERROR);
    }

    template <class Value>
    auto SendValueTo(const Value& value, Address<Domain>& address, const int flags = 0) noexcept
    {
        return SendTo(reinterpret_cast<const std::remove_all_extents_t<Value>*>(&value),
            sizeof(value) / sizeof(std::remove_all_extents_t<Value>), address, flags);
    }

public:

    template <bool Endian = NetEndian, class... Buffers>
    int PeekPacketFrom(Address<Domain>& address, Buffers&... buffers) const noexcept
    {
        DataPacket<Buffers...> packet;
        { const auto result = PeekValueFrom(packet, address);
            if (result == SOCKET_ERROR) return -1;
            if (result != 1) return 0; }
        packet.template Get<Endian>(buffers...);
        return 1;
    }

    template <bool Endian = NetEndian, class... Buffers>
    int ReceivePacketFrom(Address<Domain>& address, Buffers&... buffers) noexcept
    {
        DataPacket<Buffers...> packet;
        { const auto result = ReceiveValueFrom(packet, address);
            if (result == SOCKET_ERROR) return -1;
            if (result != 1) return 0; }
        packet.template Get<Endian>(buffers...);
        return 1;
    }

    template <bool Endian = NetEndian, class... Values>
    int SendPacketTo(const Address<Domain>& address, const Values... values) noexcept
    {
        DataPacket<Values...> packet;
        packet.template Set<Endian>(values...);
        { const auto result = SendValueTo(packet, address);
            if (result == SOCKET_ERROR) return -1;
            if (result != 1) return 0; }
        return 1;
    }

public:

    bool Shutdown(const int mode) noexcept
    {
        return shutdown(_handle, mode) == 0;
    }

public:

    int Wait(const Time timeout, const sword_t events) noexcept
    {
        Poll::Event event { _handle, events };

        if (const int result = Poll::Wait(event, timeout); result != 1)
            return result;

        if (event.Events() & (POLLERR | POLLHUP | POLLNVAL))
            return -1;

        return 1;
    }

public:

    int Status() const noexcept
    {
        int result;

        if (socklen_t length = sizeof(result);
            getsockopt(_handle, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&result), &length) != 0)
            result = -1;

        return result;
    }

private:

    SOCKET _handle = INVALID_SOCKET;

};

// ----------------------------------------------------------

template <int Type, int Protocol = GetDefaultProtocol(AF_INET, Type)>
using IPv4Socket = Socket<AF_INET, Type, Protocol>;

template <int Type, int Protocol = GetDefaultProtocol(AF_INET6, Type)>
using IPv6Socket = Socket<AF_INET6, Type, Protocol>;

// ----------------------------------------------------------

template <int Domain, int Protocol = GetDefaultProtocol(Domain, SOCK_STREAM)>
using StreamSocket = Socket<Domain, SOCK_STREAM, Protocol>;

template <int Domain, int Protocol = GetDefaultProtocol(Domain, SOCK_DGRAM)>
using DatagramSocket = Socket<Domain, SOCK_DGRAM, Protocol>;

// ----------------------------------------------------------

template <int Domain>
using TcpSocket = StreamSocket<Domain, IPPROTO_TCP>;

template <int Domain>
using UdpSocket = DatagramSocket<Domain, IPPROTO_UDP>;

// ----------------------------------------------------------

using IPv4TcpSocket = TcpSocket<AF_INET>;
using IPv6TcpSocket = TcpSocket<AF_INET6>;

using IPv4UdpSocket = UdpSocket<AF_INET>;
using IPv6UdpSocket = UdpSocket<AF_INET6>;
