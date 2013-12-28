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

#ifndef netLink_SocketManager
#define netLink_SocketManager

#include "Socket.h"

namespace netLink {

    //! Manages a group of Sockets
    class SocketManager {
        public:
        std::set<std::shared_ptr<Socket>> sockets; //!< Sockets which are managed
        
        //! Event which is called if a TCP_SERVER accepts a new connection (if false is returned the connection will be closed immediately)
        std::function<bool(SocketManager* manager, Socket* serverSocket, Socket* newSocket)> onAcceptRequest;
        //! Event which is called if a socket can or can not send more data (also called if nonblocking connect succeeded)
        std::function<void(SocketManager* manager, Socket* socket, SocketSendStatus prev)> onStateChanged;
        //! Event which is called if a socket is disconnected (or if nonblocking connect failed)
        std::function<void(SocketManager* manager, Socket* socket)> onDisconnect;
        //! Event which is called if a socket receives new data
        std::function<void(SocketManager* manager, Socket* socket)> onReceive;
        
        /*! Listens a periode time
         @param sec Time to wait in seconds
         */
        void listen(double sec = 0.0);
    };

};

#endif
