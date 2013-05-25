/*
    NetLink Sockets: Networking C++ library
    Copyright 2012 Pedro Francisco Pareja Ruiz (PedroPareja@Gmail.com)
    Modified 2013 Alexander Meißner (lichtso@gamefortec.net)

    This file is part of NetLink Sockets.

    NetLink Sockets is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    NetLink Sockets is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with NetLink Sockets. If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef NL_CORE
#define NL_CORE

#define NL_NAMESPACE_NAME NL
#define NL_NAMESPACE namespace NL_NAMESPACE_NAME {
#define NL_NAMESPACE_END };


#if defined(_WIN32) || defined(__WIN32__) || defined(_MSC_VER)

#define OS_WIN32
//#define _WIN32_WINNT 0x0600

#include <winsock2.h>
#include <ws2tcpip.h>

// Requires Win7 or Vista
// Link to Ws2_32.lib library
#define snprintf _snprintf

#else

#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <sys/time.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>

#endif

#include "MsgPack.h"

#define foreach_e(c,i) for(auto end##i = (c).end(), next##i = (c).begin(), \
i = (next##i==end##i)?end##i:next##i++; \
i != next##i; \
i = (next##i==end##i)?end##i:next##i++)


NL_NAMESPACE

void init();

/*! Defines the version of IP.
 @enum IPVer
 */
enum IPVer {
    ANY,
    IPv4,
    IPv6
};

/*! Defines the nature of a socket.
 @enum SocketType
 */
enum SocketType {
    NONE, //!< No type defined
    TCP_CLIENT, //!< TCP socket connecting to a server
    TCP_SERVER, //!< TCP socket waiting for TCP_CLIENT to connect
    TCP_SERVERS_CLIENT, //!< TCP socket to represent a TCP_CLIENT connection at the TCP_SERVER
    UDP_PEER //!< UDP socket
};

/*! Defines the send status of a socket.
 @enum SocketSendStatus
 */
enum SocketSendStatus {
    SOCKET_STATUS_OPEN, //!< Socket is connected and can send data
    SOCKET_STATUS_BUSY, //!< Socket is connected but can not send data (at the moment)
    SOCKET_STATUS_NOT_CONNECTED //!< Socket is not even connected and can not send data
};

NL_NAMESPACE_END

#endif
