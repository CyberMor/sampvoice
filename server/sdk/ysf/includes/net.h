#ifndef YSF_NET_H
#define YSF_NET_H

#ifdef _WIN32
#include <winsock2.h>
#else
typedef int SOCKET;
#define INVALID_SOCKET -1
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#endif