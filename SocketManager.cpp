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

#include "include/SocketManager.h"

NL_NAMESPACE

SocketManager::~SocketManager() {
    for(Socket* socket : sockets)
        delete socket;
}

void SocketManager::listen(double secLeft) {
    fd_set readfds, writefds, exceptfds;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
    
    int maxHandle = 0;
    for(Socket* socket : sockets) {
        maxHandle = std::max(maxHandle, socket->handle);
        FD_SET(socket->handle, &readfds);
        if(socket->type != TCP_SERVER)
            FD_SET(socket->handle, &writefds);
        if(socket->type == TCP_CLIENT)
            FD_SET(socket->handle, &exceptfds);
    }
    
    struct timeval timeout;
    timeout.tv_sec = secLeft;
    timeout.tv_usec = secLeft * 1000000.0;
    int status = select(maxHandle + 1, &readfds, &writefds, &exceptfds, &timeout);
    
    if(status == -1)
        throw Exception(Exception::ERROR_SELECT);
    
    foreach_e(sockets, iterator) {
        bool isInReadfds = FD_ISSET((*iterator)->handle, &readfds);
        
        switch((*iterator)->type) {
            case NONE:
                throw Exception(Exception::BAD_TYPE);
            case TCP_SERVER:
                if(isInReadfds && onAcceptRequest) {
                    Socket* newSocket = (*iterator)->accept();
                    if(onAcceptRequest(this, *iterator, newSocket))
                        sockets.insert(newSocket);
                    else
                        delete newSocket;
                }
            continue;
            case TCP_CLIENT:
                if(FD_ISSET((*iterator)->handle, &exceptfds)) {
                    if(onDisconnect) onDisconnect(this, *iterator);
                    delete *iterator;
                    sockets.erase(iterator);
                    continue;
                }
            case TCP_SERVERS_CLIENT:
                if(isInReadfds && (*iterator)->showmanyc() <= 0) {
                    if(onDisconnect) onDisconnect(this, *iterator);
                    delete *iterator;
                    sockets.erase(iterator);
                    continue;
                }
            case UDP_PEER:
                if(isInReadfds && onReceive)
                    onReceive(this, *iterator);
                
                SocketSendStatus prev = (SocketSendStatus) (*iterator)->recvStatus;
                (*iterator)->recvStatus = (FD_ISSET((*iterator)->handle, &writefds)) ? SOCKET_STATUS_OPEN : SOCKET_STATUS_BUSY;
                if(onStateChanged && (*iterator)->recvStatus != prev)
                    onStateChanged(this, *iterator, prev);
        }
    }
}

NL_NAMESPACE_END