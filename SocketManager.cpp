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

#include "include/SocketManager.h"

namespace netLink {

std::shared_ptr<Socket> SocketManager::generateSocket() {
    std::shared_ptr<Socket> socket(new Socket());
    sockets.insert(socket);
    return socket;
}

void SocketManager::listen(double secLeft) {
    fd_set readfds, writefds, exceptfds;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
    
    int maxHandle = -1;
    std::set<std::shared_ptr<Socket>> selection;
    foreach_e(sockets, iterator) {
        Socket* socket = (*iterator).get();

        //Garbage collect disconnected sockets
        if(socket->type == NONE) {
            sockets.erase(iterator);
            continue;
        }

        //Add the socket to the listen set
        maxHandle = std::max(maxHandle, socket->handle);
        FD_SET(socket->handle, &readfds);
        FD_SET(socket->handle, &exceptfds);
        selection.insert(*iterator);

        if(socket->type == TCP_SERVER) {
            //Iterate all TCP_SERVERS_CLIENTs
            foreach_e(socket->clients, clientIterator) {
                Socket* client = (*clientIterator).get();
    
                //Garbage collect disconnected clients
                if(client->type == NONE) {
                    socket->clients.erase(clientIterator);
                    continue;
                }
    
                //Add client to the listen set
                maxHandle = std::max(maxHandle, client->handle);
                FD_SET(client->handle, &readfds);
                FD_SET(client->handle, &writefds);
                FD_SET(client->handle, &exceptfds);
                selection.insert(*clientIterator);
            }
        }else
            FD_SET(socket->handle, &writefds);
    }
    if(maxHandle == -1) return;
    
    struct timeval timeout, *timeoutPtr;
    if(secLeft >= 0.0) {
        timeout.tv_sec = secLeft;
        timeout.tv_usec = fmod(secLeft, 1.0) * 1000000.0;
        timeoutPtr = &timeout;
    }else
        timeoutPtr = NULL;

    if(select(maxHandle + 1, &readfds, &writefds, &exceptfds, timeoutPtr) == -1)
        throw Exception(Exception::ERROR_SELECT);
    
    foreach_e(selection, iterator) {
        Socket* socket = (*iterator).get();

        //Exception occured: disconnect
        if(FD_ISSET(socket->handle, &exceptfds)) {
            if(onDisconnect) onDisconnect(this, *iterator);
            socket->disconnect();
            break;
        }

        if(socket->type == TCP_SERVER) {
            if(FD_ISSET(socket->handle, &readfds)) {
                std::shared_ptr<Socket> newSocket = socket->accept();
                //New client accepted
                if(onConnectRequest && !onConnectRequest(this, *iterator, newSocket)) {
                    newSocket->disconnect();
                    socket->clients.erase(newSocket);
                }
            }
        }else{
            if(FD_ISSET(socket->handle, &readfds) && onReceive) {
                //Unable to read: disconnect
                if(socket->showmanyc() <= 0) {
                    if(onDisconnect) onDisconnect(this, *iterator);
                    socket->disconnect();
                    break;
                }
                onReceive(this, *iterator);
            }
            
            SocketStatus prev = (SocketStatus) socket->status;
            
            if(FD_ISSET(socket->handle, &writefds))
                socket->status = READY;
            else
                socket->status = (prev == NOT_CONNECTED) ? NOT_CONNECTED : BUSY;

            if(onStateChanged && socket->status != prev)
                onStateChanged(this, *iterator, prev);
        }
    }
}

};