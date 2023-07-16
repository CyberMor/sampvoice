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

};

using IPv4Address = Address<AF_INET>;

// ----------------------------------------------------------

template <>
struct Address<AF_INET6> : private sockaddr_in6 {

    Address() noexcept { sin6_family = AF_UNSPEC; }
    ~Address() noexcept = default;
    Address(const Address&) noexcept = default;
    Address(Address&&) noexcept = default;
    Address& operator=(const Address&) noexcept = default;
    Address& operator=(Address&&) noexcept = default;

public:

    template <bool ConvertToNetworkEndian = true>
    static Address FromNumber(const cptr_t host, const uword_t port = 0,
        const udword_t scope_id = 0, const udword_t flowinfo = 0) noexcept
    {
        Address result;

        result.sin6_family = AF_INET6;
        result.sin6_addr = *static_cast<const in6_addr*>(host);

        if constexpr (HostEndian != NetEndian && ConvertToNetworkEndian)
            result.sin6_port = utils::bswap(port);
        else result.sin6_port = port;

        result.sin6_scope_id = scope_id;
        result.sin6_flowinfo = flowinfo;

        return result;
    }

    template <bool ConvertToNetworkEndian = true>
    static Address FromString(const cstr_t host, const uword_t port = 0,
        const udword_t scope_id = 0, const udword_t flowinfo = 0) noexcept
    {
        Address result;

        if (inet_pton(AF_INET6, host, &result.sin6_addr) > 0)
        {
            result.sin6_family = AF_INET6;
            
            if constexpr (HostEndian != NetEndian && ConvertToNetworkEndian)
                result.sin6_port = utils::bswap(port);
            else result.sin6_port = port;

            result.sin6_scope_id = scope_id;
            result.sin6_flowinfo = flowinfo;
        }

        return result;
    }

public:

    static Address Empty() noexcept
    {
        Address result;

        result.sin6_family = AF_INET6;
        result.sin6_addr = in6addr_any;
        result.sin6_port = 0;
        result.sin6_scope_id = 0;
        result.sin6_flowinfo = 0;

        return result;
    }

    template <bool ConvertToNetworkEndian = true>
    static Address Loopback(const uword_t port = 0,
        const udword_t scope_id = 0, const udword_t flowinfo = 0) noexcept
    {
        Address result;

        result.sin6_family = AF_INET6;
        result.sin6_addr = in6addr_loopback;
        
        if constexpr (HostEndian != NetEndian && ConvertToNetworkEndian)
            result.sin6_port = utils::bswap(port);
        else result.sin6_port = port;

        result.sin6_scope_id = scope_id;
        result.sin6_flowinfo = flowinfo;

        return result;
    }

    template <bool ConvertToNetworkEndian = true>
    static Address Any(const uword_t port = 0,
        const udword_t scope_id = 0, const udword_t flowinfo = 0) noexcept
    {
        Address result;

        result.sin6_family = AF_INET6;
        result.sin6_addr = in6addr_any;

        if constexpr (HostEndian != NetEndian && ConvertToNetworkEndian)
            result.sin6_port = utils::bswap(port);
        else result.sin6_port = port;

        result.sin6_scope_id = scope_id;
        result.sin6_flowinfo = flowinfo;

        return result;
    }

public:

    bool Valid() const noexcept
    {
        return sin6_family == AF_INET6;
    }

    bool Invalid() const noexcept
    {
        return sin6_family != AF_INET6;
    }

public:

    bool IsEmpty() const noexcept
    {
        assert(sin6_family == AF_INET6);

        return std::memcmp(sin6_addr.s6_addr, in6addr_any.s6_addr, sizeof(in6_addr)) == 0 && sin6_port == 0;
    }

    bool IsLoopback() const noexcept
    {
        assert(sin6_family == AF_INET6);

        return std::memcmp(sin6_addr.s6_addr, in6addr_loopback.s6_addr, sizeof(in6_addr)) == 0;
    }

    bool IsAny() const noexcept
    {
        assert(sin6_family == AF_INET6);

        return std::memcmp(sin6_addr.s6_addr, in6addr_any.s6_addr, sizeof(in6_addr)) == 0;
    }

public:

    bool operator<(const Address& address) const noexcept
    {
        assert(sin6_family         == AF_UNSPEC || sin6_family         == AF_INET6);
        assert(address.sin6_family == AF_UNSPEC || address.sin6_family == AF_INET6);

        return address.sin6_family != AF_UNSPEC &&
            (sin6_family == AF_UNSPEC || (std::memcmp(sin6_addr.s6_addr, address.sin6_addr.s6_addr, sizeof(in6_addr)) <= 0 &&
                sin6_port < address.sin6_port));
    }

    bool operator>(const Address& address) const noexcept
    {
        assert(sin6_family         == AF_UNSPEC || sin6_family         == AF_INET6);
        assert(address.sin6_family == AF_UNSPEC || address.sin6_family == AF_INET6);

        return sin6_family != AF_UNSPEC &&
            (address.sin6_family == AF_UNSPEC || (std::memcmp(address.sin6_addr.s6_addr, sin6_addr.s6_addr, sizeof(in6_addr)) <= 0 &&
                address.sin6_port < sin6_port));
    }

    bool operator<=(const Address& address) const noexcept
    {
        static_assert(AF_UNSPEC == 0 && AF_INET6 != 0);

        assert(sin6_family         == AF_UNSPEC || sin6_family         == AF_INET6);
        assert(address.sin6_family == AF_UNSPEC || address.sin6_family == AF_INET6);

        return (sin6_family * address.sin6_family == 0 &&
            sin6_family == AF_UNSPEC) || (std::memcmp(sin6_addr.s6_addr, address.sin6_addr.s6_addr, sizeof(in6_addr)) <= 0 &&
                sin6_port <= address.sin6_port);
    }

    bool operator>=(const Address& address) const noexcept
    {
        static_assert(AF_UNSPEC == 0 && AF_INET6 != 0);

        assert(sin6_family         == AF_UNSPEC || sin6_family         == AF_INET6);
        assert(address.sin6_family == AF_UNSPEC || address.sin6_family == AF_INET6);

        return (address.sin6_family * sin6_family == 0 &&
            address.sin6_family == AF_UNSPEC) || (std::memcmp(address.sin6_addr.s6_addr, sin6_addr.s6_addr, sizeof(in6_addr)) <= 0 &&
                address.sin6_port <= sin6_port);
    }

    bool operator==(const Address& address) const noexcept
    {
        assert(sin6_family         == AF_UNSPEC || sin6_family         == AF_INET6);
        assert(address.sin6_family == AF_UNSPEC || address.sin6_family == AF_INET6);

        return sin6_family == address.sin6_family &&
            (sin6_family == AF_UNSPEC || (std::memcmp(sin6_addr.s6_addr, address.sin6_addr.s6_addr, sizeof(in6_addr)) == 0 &&
                sin6_port == address.sin6_port));
    }

    bool operator!=(const Address& address) const noexcept
    {
        assert(sin6_family         == AF_UNSPEC || sin6_family         == AF_INET6);
        assert(address.sin6_family == AF_UNSPEC || address.sin6_family == AF_INET6);

        return sin6_family != address.sin6_family ||
            (sin6_family != AF_UNSPEC && (std::memcmp(sin6_addr.s6_addr, address.sin6_addr.s6_addr, sizeof(in6_addr)) != 0 ||
                sin6_port != address.sin6_port));
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
        return sizeof(sockaddr_in6);
    }

public:

    cptr_t GetHost() const noexcept
    {
        assert(sin6_family == AF_INET6);

        return sin6_addr.s6_addr;
    }

    template <bool ConvertToHostEndian = false>
    uword_t GetPort() const noexcept
    {
        assert(sin6_family == AF_INET6);

        if constexpr (HostEndian != NetEndian && ConvertToHostEndian)
            return utils::bswap(static_cast<uword_t>(sin6_port));
        else return static_cast<uword_t>(sin6_port);
    }

    udword_t GetScopeId() const noexcept
    {
        assert(sin6_family == AF_INET6);

        return sin6_scope_id;
    }

    udword_t GetFlowInfo() const noexcept
    {
        assert(sin6_family == AF_INET6);

        return sin6_flowinfo;
    }

public:

    void SetHost(const cptr_t host) noexcept
    {
        assert(sin6_family == AF_INET6);

        std::memcpy(sin6_addr.s6_addr, host, sizeof(in6_addr));
    }

    bool SetHost(const cstr_t host) noexcept
    {
        assert(sin6_family == AF_INET6);

        return inet_pton(AF_INET6, host, &sin6_addr) > 0;
    }

    template <bool ConvertToNetworkEndian = true>
    void SetPort(const uword_t port) noexcept
    {
        assert(sin6_family == AF_INET6);

        if constexpr (HostEndian != NetEndian && ConvertToNetworkEndian)
            sin6_port = utils::bswap(port);
        else sin6_port = port;
    }

    void SetScopeId(const udword_t scope_id) noexcept
    {
        assert(sin6_family == AF_INET6);

        sin6_scope_id = scope_id;
    }

    void SetFlowInfo(const udword_t flowinfo) noexcept
    {
        assert(sin6_family == AF_INET6);

        sin6_flowinfo = flowinfo;
    }

public:

    static constexpr size_t HostLengthLimit = 45;
    static constexpr size_t PortLengthLimit = 5;

public:

    bool PrintHost(const str_t buffer) const noexcept
    {
        return inet_ntop(AF_INET6, const_cast<in6_addr*>(&sin6_addr),
            buffer, HostLengthLimit + 1) != nullptr;
    }

    bool PrintPort(const str_t buffer) const noexcept
    {
        const auto [p, e] = std::to_chars(buffer, buffer + PortLengthLimit, GetPort<true>());
        return e == std::errc() ? (*p = '\0', true) : false;
    }

};

using IPv6Address = Address<AF_INET6>;

// ----------------------------------------------------------

template <>
struct Address<AF_UNSPEC> : private sockaddr_storage {

    Address() noexcept { ss_family = AF_UNSPEC; }
    ~Address() noexcept = default;
    Address(const Address&) noexcept = default;
    Address(Address&&) noexcept = default;
    Address& operator=(const Address&) noexcept = default;
    Address& operator=(Address&&) noexcept = default;

public:

    uword_t Domain() const noexcept
    {
        return ss_family;
    }

public:

    const sockaddr* Data() const noexcept
    {
        switch (ss_family)
        {
            case AF_INET  : return Cast<AF_INET>().Data();
            case AF_INET6 : return Cast<AF_INET6>().Data();
        }

        return reinterpret_cast<const sockaddr*>(this);
    }

    sockaddr* Data() noexcept
    {
        switch (ss_family)
        {
            case AF_INET  : return Cast<AF_INET>().Data();
            case AF_INET6 : return Cast<AF_INET6>().Data();
        }

        return reinterpret_cast<sockaddr*>(this);
    }

public:

    socklen_t Size() const noexcept
    {
        switch (ss_family)
        {
            case AF_INET  : return Cast<AF_INET>().Size();
            case AF_INET6 : return Cast<AF_INET6>().Size();
        }

        return sizeof(sockaddr_storage);
    }

public:

    template <int Domain>
    const Address<Domain>& Cast() const noexcept
    {
        assert(ss_family == Domain);

        return *reinterpret_cast<const Address<Domain>*>(this);
    }

    template <int Domain>
    Address<Domain>& Cast() noexcept
    {
        assert(ss_family == Domain);

        return *reinterpret_cast<Address<Domain>*>(this);
    }

};

using AnyAddress = Address<AF_UNSPEC>;

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
