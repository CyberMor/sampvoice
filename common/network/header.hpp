/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

// Includes
// ------------------------------------------

#ifdef _WIN32
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <Ws2def.h>
#else
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <linux/sockios.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <netdb.h>
#include <errno.h>
#include <poll.h>
#endif

#include <system/types.hpp>

// Types
// ------------------------------------------

#ifdef _WIN32
using nfds_t = ULONG;
#else
using SOCKET = int;
#endif

// Constants
// ------------------------------------------

#ifdef _WIN32
#define SHUT_RD   SD_RECEIVE
#define SHUT_WR   SD_SEND
#define SHUT_RDWR SD_BOTH
#else
#define INVALID_SOCKET -1
#define  SOCKET_ERROR  -1
#endif

// Functions
// ------------------------------------------

#ifdef _WIN32
#define GetSocketError() WSAGetLastError()
#define PrintSocketError(error, buffer, length) (strerror_s(buffer, length, error), buffer)
#define PollSockets(events, count, timeout) WSAPoll(events, count, timeout)
#define CloseSocket(handle) closesocket(handle)
#else
#define GetSocketError() (errno)
#define PrintSocketError(error, buffer, length) strerror_r(error, buffer, length)
#define PollSockets(events, count, timeout) poll(events, count, timeout)
#define CloseSocket(handle) close(handle)
#endif

// Special
// ------------------------------------------

#ifdef _WIN32
#define SocketLibraryStartup() (WSAStartup(MAKEWORD(2, 2), &WSADATA()) == 0)
#define SocketLibraryCleanup() (WSACleanup() == 0)
#else
#define SocketLibraryStartup() true
#define SocketLibraryCleanup() true
#endif

// Conflicts Resolving
// ------------------------------------------

#ifdef _WIN32
#undef SetPort
#endif

// Linking
// ------------------------------------------

#ifdef _WIN32
#pragma comment(lib, "WS2_32.lib")
#endif

// ------------------------------------------

constexpr size_t kEthernetMinimumSize = 576;
constexpr size_t kEthernetOptimalSize = 1492;
constexpr size_t kEthernetMaximumSize = 65535;

constexpr size_t kIPHeaderMinimumSize = 20;
constexpr size_t kIPHeaderMaximumSize = 60;

constexpr size_t kTCPHeaderMinimumSize = 20;
constexpr size_t kTCPHeaderMaximumSize = 60;
constexpr size_t kUDPHeaderMaximumSize = 8;

// ----------------------------------------------------------

constexpr size_t kTCPContentMinimumSize = kEthernetMinimumSize -
    (kIPHeaderMaximumSize + kTCPHeaderMaximumSize);
constexpr size_t kUDPContentMinimumSize = kEthernetMinimumSize -
    (kIPHeaderMaximumSize + kUDPHeaderMaximumSize);

constexpr size_t kTCPContentOptimalSize = kEthernetOptimalSize -
    (kIPHeaderMinimumSize + kTCPHeaderMinimumSize);
constexpr size_t kUDPContentOptimalSize = kEthernetOptimalSize -
    (kIPHeaderMinimumSize + kUDPHeaderMaximumSize);

constexpr size_t kTCPContentMaximumSize = kEthernetMaximumSize -
    (kIPHeaderMinimumSize + kTCPHeaderMinimumSize);
constexpr size_t kUDPContentMaximumSize = kEthernetMaximumSize -
    (kIPHeaderMinimumSize + kUDPHeaderMaximumSize);
