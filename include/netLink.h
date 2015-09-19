/*
    netLink: c++ 11 networking library
    Copyright 2014 Alexander Meißner (lichtso@gamefortec.net)

    This software is provided 'as-is', without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the use of this software.
    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it freely,
    subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
*/

#pragma once

#include "MsgPackSocket.h"

namespace netLink {

    //! Manages a group of Sockets
    class SocketManager {
        public:
        //! Event which is called if a TCP_SERVER accepts a new connection (if false is returned the connection will be closed immediately)
        std::function<bool(SocketManager* manager, std::shared_ptr<Socket> serverSocket, std::shared_ptr<Socket> clientSocket)> onConnectRequest;
        //! Event which is called if a socket can or can not send more data (also called if nonblocking connect succeeded)
        std::function<void(SocketManager* manager, std::shared_ptr<Socket> socket, Socket::Status prev)> onStatusChange;
        //! Event which is called if a socket receives new raw data
        std::function<void(SocketManager* manager, std::shared_ptr<Socket> socket)> onReceiveRaw;
        //! Event which is called if a socket receives a MsgPack::Element
        std::function<void(SocketManager* manager, std::shared_ptr<Socket> socket, std::unique_ptr<MsgPack::Element> element)> onReceiveMsgPack;
        //! Sockets which are managed
        std::set<std::shared_ptr<Socket>> sockets;

        //! Allocates a new Socket, inserts it into sockets and returns it
        std::shared_ptr<Socket> newSocket();
        //! Allocates a new MsgPackSocket, inserts it into sockets and returns it
        std::shared_ptr<Socket> newMsgPackSocket();

        /*! Listens a periode time
         @param sec Maximum time to wait in seconds or negative values to wait indefinitely
         */
        void listen(double sec = 0.0);
    };

};
