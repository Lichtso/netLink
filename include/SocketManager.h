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

#ifndef NL_SOCKET_GROUP
#define NL_SOCKET_GROUP

#include "Socket.h"

NL_NAMESPACE

//! Manages a group of Sockets
class SocketManager {
    public:
    std::set<Socket*> sockets; //!< Sockets which are managed
    
    //! Event which is called if a TCP_SERVER accepts a new connection (if false is returned the connection will be closed immediately)
    std::function<bool(SocketManager* group, Socket* serverSocket, Socket* newSocket)> onAcceptRequest;
    //! Event which is called if a socket can or can not send more data (also called if nonblocking connect succeeded)
    std::function<void(SocketManager* group, Socket* socket, SocketSendStatus prev)> onStateChanged;
    //! Event which is called if a socket is disconnected (or if nonblocking connect failed)
    std::function<void(SocketManager* group, Socket* socket)> onDisconnect;
    //! Event which is called if a socket receives new data
    std::function<void(SocketManager* group, Socket* socket)> onReceive;
    
    ~SocketManager();
    
    /*! Listens a periode time
     @param sec Time to wait in seconds
     */
    void listen(double sec = 0.0);
};

NL_NAMESPACE_END

#endif
