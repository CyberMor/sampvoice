/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cassert>
#include <cstdlib>
#include <cstring>

#include <charconv>

#include <system/types.hpp>
#include <other/utils.hpp>

#include "header.hpp"

// ----------------------------------------------------------

template <int Domain>
struct Address;

// ----------------------------------------------------------

template <>
struct Address<AF_INET> : private sockaddr_in {

    Address() noexcept { sin_family = AF_UNSPEC; }
    ~Address() noexcept = default;
    Address(const Address&) noexcept = default;
    Address(Address&&) noexcept = default;
    Address& operator=(const Address&) noexcept = default;
    Address& operator=(Address&&) noexcept = default;

public:

    template <bool ConvertToNetworkEndian = true>
    static Address FromNumber(const udword_t host, const uword_t port = 0) noexcept
    {
        Address result;

        result.sin_family = AF_INET;

        if constexpr (HostEndian != NetEndian && ConvertToNetworkEndian)
            result.sin_addr.s_addr = utils::bswap(host);
        else result.sin_addr.s_addr = host;

        if constexpr (HostEndian != NetEndian && ConvertToNetworkEndian)
            result.sin_port = utils::bswap(port);
        else result.sin_port = port;

        return result;
    }

    template <bool ConvertToNetworkEndian = true>
    static Address FromString(const cstr_t host, const uword_t port = 0) noexcept
    {
        Address result;

        if (inet_pton(AF_INET, host, &result.sin_addr) > 0)
        {
            result.sin_family = AF_INET;

            if constexpr (HostEndian != NetEndian && ConvertToNetworkEndian)
                result.sin_port = utils::bswap(port);
            else result.sin_port = port;
        }

        return result;
    }

public:

    static Address Empty() noexcept
    {
        Address result;

        result.sin_family = AF_INET;
        result.sin_addr.s_addr = INADDR_ANY;
        result.sin_port = 0;

        return result;
    }

    template <bool ConvertToNetworkEndian = true>
    static Address Loopback(const uword_t port = 0) noexcept
    {
        Address result;

        result.sin_family = AF_INET;
        result.sin_addr.s_addr = (HostEndian != NetEndian ?
            utils::ct::bswap(static_cast<udword_t>(INADDR_LOOPBACK)) :
                static_cast<udword_t>(INADDR_LOOPBACK));

        if constexpr (HostEndian != NetEndian && ConvertToNetworkEndian)
            result.sin_port = utils::bswap(port);
        else result.sin_port = port;

        return result;
    }

    template <bool ConvertToNetworkEndian = true>
    static Address Any(const uword_t port = 0) noexcept
    {
        Address result;

        result.sin_family = AF_INET;
        result.sin_addr.s_addr = INADDR_ANY;

        if constexpr (HostEndian != NetEndian && ConvertToNetworkEndian)
            result.sin_port = utils::bswap(port);
        else result.sin_port = port;

        return result;
    }

public:

    bool Valid() const noexcept
    {
        return sin_family == AF_INET;
    }

    bool Invalid() const noexcept
    {
        return sin_family != AF_INET;
    }

public:

    bool IsEmpty() const noexcept
    {
        assert(sin_family == AF_INET);

        return sin_addr.s_addr == INADDR_ANY && sin_port == 0;
    }

    bool IsLoopback() const noexcept
    {
        assert(sin_family == AF_INET);

        return sin_addr.s_addr == (HostEndian != NetEndian ?
            utils::ct::bswap(static_cast<udword_t>(INADDR_LOOPBACK)) :
                static_cast<udword_t>(INADDR_LOOPBACK));
    }

    bool IsAny() const noexcept
    {
        assert(sin_family == AF_INET);

        return sin_addr.s_addr == INADDR_ANY;
    }

public:

    bool operator<(const Address& address) const noexcept
    {
        assert(sin_family         == AF_UNSPEC || sin_family         == AF_INET);
        assert(address.sin_family == AF_UNSPEC || address.sin_family == AF_INET);

        return address.sin_family != AF_UNSPEC &&
            (sin_family == AF_UNSPEC || (sin_addr.s_addr <= address.sin_addr.s_addr &&
                sin_port < address.sin_port));
    }

    bool operator>(const Address& address) const noexcept
    {
        assert(sin_family         == AF_UNSPEC || sin_family         == AF_INET);
        assert(address.sin_family == AF_UNSPEC || address.sin_family == AF_INET);

        return sin_family != AF_UNSPEC &&
            (address.sin_family == AF_UNSPEC || (address.sin_addr.s_addr <= sin_addr.s_addr &&
                address.sin_port < sin_port));
    }

    bool operator<=(const Address& address) const noexcept
    {
        static_assert(AF_UNSPEC == 0 && AF_INET != 0);

        assert(sin_family         == AF_UNSPEC || sin_family         == AF_INET);
        assert(address.sin_family == AF_UNSPEC || address.sin_family == AF_INET);

        return (sin_family * address.sin_family == 0 &&
            sin_family == AF_UNSPEC) || (sin_addr.s_addr <= address.sin_addr.s_addr &&
                sin_port <= address.sin_port);
    }

    bool operator>=(const Address& address) const noexcept
    {
        static_assert(AF_UNSPEC == 0 && AF_INET != 0);

        assert(sin_family         == AF_UNSPEC || sin_family         == AF_INET);
        assert(address.sin_family == AF_UNSPEC || address.sin_family == AF_INET);

        return (address.sin_family * sin_family == 0 &&
            address.sin_family == AF_UNSPEC) || (address.sin_addr.s_addr <= sin_addr.s_addr &&
                address.sin_port <= sin_port);
    }

    bool operator==(const Address& address) const noexcept
    {
        assert(sin_family         == AF_UNSPEC || sin_family         == AF_INET);
        assert(address.sin_family == AF_UNSPEC || address.sin_family == AF_INET);

        return sin_family == address.sin_family &&
            (sin_family == AF_UNSPEC || (sin_addr.s_addr == address.sin_addr.s_addr &&
                sin_port == address.sin_port));
    }

    bool operator!=(const Address& address) const noexcept
    {
        assert(sin_family         == AF_UNSPEC || sin_family         == AF_INET);
        assert(address.sin_family == AF_UNSPEC || address.sin_family == AF_INET);

        return sin_family != address.sin_family ||
            (sin_family != AF_UNSPEC && (sin_addr.s_addr != address.sin_addr.s_addr ||
                sin_port != address.sin_port));
    }

public:

    const sockaddr* Data() const noexcept
    {
        return reinterpret_cast<const sockaddr*>(this);
    }

    sockaddr* Data() noexcept
    {
        return reinterpret_cast<sockaddr*>(this);
    }

public:

    static constexpr socklen_t Size() noexcept
    {
        return sizeof(sockaddr_in);
    }

public:

    template <bool ConvertToHostEndian = false>
    udword_t GetHost() const noexcept
    {
        assert(sin_family == AF_INET);

        if constexpr (HostEndian != NetEndian && ConvertToHostEndian)
            return utils::bswap(static_cast<udword_t>(sin_addr.s_addr));
        else return static_cast<udword_t>(sin_addr.s_addr);
    }

    template <bool ConvertToHostEndian = false>
    uword_t GetPort() const noexcept
    {
        assert(sin_family == AF_INET);

        if constexpr (HostEndian != NetEndian && ConvertToHostEndian)
            return utils::bswap(static_cast<uword_t>(sin_port));
        else return static_cast<uword_t>(sin_port);
    }

public:

    template <bool ConvertToNetworkEndian = true>
    void SetHost(const udword_t host) noexcept
    {
        assert(sin_family == AF_INET);

        if constexpr (HostEndian != NetEndian && ConvertToNetworkEndian)
            sin_addr.s_addr = utils::bswap(host);
        else sin_addr.s_addr = host;
    }

    bool SetHost(const cstr_t host) noexcept
    {
        assert(sin_family == AF_INET);

        return inet_pton(AF_INET, host, &sin_addr) > 0;
    }

    template <bool ConvertToNetworkEndian = true>
    void SetPort(const uword_t port) noexcept
    {
        assert(sin_family == AF_INET);

        if constexpr (HostEndian != NetEndian && ConvertToNetworkEndian)
            sin_port = utils::bswap(port);
        else sin_port = port;
    }

public:

    static constexpr size_t HostLengthLimit = 15;
    static constexpr size_t PortLengthLimit = 5;

public:

    bool PrintHost(const str_t buffer) const noexcept
    {
        return inet_ntop(AF_INET, const_cast<in_addr*>(&sin_addr),
            buffer, HostLengthLimit + 1) != nullptr;
    }

    bool PrintPort(const str_t buffer) const noexcept
    {
        const auto [p, e] = std::to_chars(buffer, buffer + PortLengthLimit, GetPort<true>());
        return e == std::errc() ? (*p = '\0', true) : false;
    }

public:

    static constexpr size_t LengthLimit = HostLengthLimit + PortLengthLimit + 1;

public:

    bool Print(str_t buffer) const noexcept
    {
        if (!PrintHost(buffer))
            return false;

        buffer += std::strlen(buffer);
        *buffer++ = ':';

        if (!PrintPort(buffer))
            return false;

        return true;
    }

};

using IPv4Address = Address<AF_INET>;

// ----------------------------------------------------------

constexpr bool IsInternetFamily(const int domain) noexcept
{
    return domain == AF_INET || domain == AF_INET6;
}

constexpr int GetDefaultProtocol(const int domain, const int type) noexcept
{
    if (IsInternetFamily(domain))
    {
        if (type == SOCK_STREAM) return IPPROTO_TCP;
        if (type == SOCK_DGRAM)  return IPPROTO_UDP;
    }

    return 0;
}

/*
    Callback = bool(int domain, int type, int protocol, const Address<Domain>& address)
*/
template <int Domain = AF_UNSPEC, int Type = 0, int Protocol = GetDefaultProtocol(Domain, Type), class Callback>
bool ResolveAddress(const cstr_t host, const uword_t port, Callback&& callback) noexcept
{
    const addrinfo hints
    {
        .ai_flags     = AI_PASSIVE | AI_NUMERICSERV,
        .ai_family    = Domain,
        .ai_socktype  = Type,
        .ai_protocol  = Protocol,
        .ai_addrlen   = 0,
#ifdef _WIN32
        .ai_canonname = nullptr,
        .ai_addr      = nullptr,
#else
        .ai_addr      = nullptr,
        .ai_canonname = nullptr,
#endif
        .ai_next      = nullptr
    };

    char service[6];

    const auto [p, e] = std::to_chars(service, service + 5, port);
    if (e != std::errc()) return false; else *p = '\0';

    if (addrinfo* output; getaddrinfo(host, service, &hints, &output) == 0)
    {
        for (addrinfo* iterator = output; iterator != nullptr; iterator = iterator->ai_next)
        {
            if (callback(iterator->ai_family, iterator->ai_socktype, iterator->ai_protocol,
                std::cref(*reinterpret_cast<const Address<Domain>*>(iterator->ai_addr))))
            {
                return true;
            }
        }

        freeaddrinfo(output);
    }

    return false;
}
