/*
    netLink: c++ 11 networking library
    Copyright (C) 2013-2023 Alexander Meißner

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
         @param waitUpToSeconds Maximum time to wait for incoming data in seconds or negative values to wait indefinitely
         */
        void listen(double waitUpToSeconds = 0.0);
    };

};
