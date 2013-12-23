/*
    netLink: c++ 11 networking library
    Copyright 2013 Alexander Meißner (lichtso@gamefortec.net)

    This software is provided 'as-is', without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the use of this software.
    Permission is granted to anyone to use this software for any purpose, 
    including commercial applications, and to alter it and redistribute it freely, 
    subject to the following restrictions:
    
    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
*/

#ifndef netLink_Core
#define netLink_Core

#include "MsgPack.h"

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

#define foreach_e(c,i) for(auto end##i = (c).end(), next##i = (c).begin(), \
i = (next##i==end##i)?end##i:next##i++; \
i != next##i; \
i = (next##i==end##i)?end##i:next##i++)

namespace netLink {

    /*! Defines the version of IP.
     @enum IPVer
     */
    enum IPVer {
        IPv4,
        IPv6,
        ANY
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
        SOCKET_STATUS_NOT_CONNECTED, //!< Socket is not even connected and can not send data
        SOCKET_STATUS_OPEN, //!< Socket is connected and can send data
        SOCKET_STATUS_BUSY //!< Socket is connected but can not send data (at the moment)
    };
    
    //! netLink Exceptions
    class Exception {
        public:
        enum CODE {
            //System calls
            ERROR_IOCTL,
            ERROR_SELECT,
            ERROR_SET_SOCK_OPT,
            ERROR_GET_SOCK_NAME,
            ERROR_RESOLVING_ADDRESS,
            
            //Functionality
            ERROR_READ,
            ERROR_SEND,
            ERROR_INIT,
            
            //Wrong parameters
            BAD_PROTOCOL,
            BAD_TYPE
        } code; //!< netLinks error code
        
        Exception(CODE _code): code(_code) { }
    };
    
};

#endif
