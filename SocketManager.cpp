/*
    netLink: C++11 networking library
    Copyright 2013 Alexander Meißner (lichtso@gamefortec.net)
*/

#include "include/SocketManager.h"

namespace netLink {

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

};