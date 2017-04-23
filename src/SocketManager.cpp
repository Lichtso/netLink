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

#include "netLink.h"

#define foreach_e(c, i) for(auto end##i = (c).end(), next##i = (c).begin(), \
    i = (next##i==end##i)?end##i:next##i++; \
    i != next##i; \
    i = (next##i==end##i)?end##i:next##i++)

#define checkSocketStillValid(socketsSet, iterator, socket) \
    if(socket->status == Socket::Status::NOT_CONNECTED) { \
        socketsSet.erase(*iterator); \
        continue; \
    }

#define removeSocket() { \
    if(onStatusChange) onStatusChange(this, *iterator, prev); \
    sockets.erase(*iterator); \
    selection.erase(*iterator); \
    continue; \
}

#define forEachSocket() \
    Socket* socket = (*iterator).get(); \
    MsgPackSocket* msgPackSocket = dynamic_cast<MsgPackSocket*>(socket); \
    Socket::Status prev = socket->getStatus(); \
    socket->disconnectOnError(); \
    if(socket->getStatus() == Socket::Status::NOT_CONNECTED) \
        removeSocket()

namespace netLink {

std::shared_ptr<Socket> SocketManager::newSocket() {
    std::shared_ptr<Socket> socket(new Socket());
    sockets.insert(socket);
    return socket;
}

std::shared_ptr<Socket> SocketManager::newMsgPackSocket() {
    std::shared_ptr<Socket> socket(new MsgPackSocket());
    sockets.insert(socket);
    return socket;
}

void SocketManager::listen(double waitUpToSeconds) {
    fd_set readfds, writefds;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);

    int maxHandle = -1;
    std::set<std::shared_ptr<Socket>> selection;
    foreach_e(sockets, iterator) {
        Socket* socket = (*iterator).get();
        checkSocketStillValid(sockets, iterator, socket);

        // Add the socket to the listen set
        maxHandle = std::max(maxHandle, socket->handle);
        FD_SET(socket->handle, &readfds);
        selection.insert(*iterator);

        if(socket->type == Socket::Type::TCP_SERVER) {
            // Iterate all TCP_SERVERS_CLIENTs
            foreach_e(socket->clients, clientIterator) {
                Socket* client = (*clientIterator).get();
                checkSocketStillValid(socket->clients, clientIterator, client)

                // Add client to the listen set
                maxHandle = std::max(maxHandle, client->handle);
                FD_SET(client->handle, &readfds);
                FD_SET(client->handle, &writefds);
                selection.insert(*clientIterator);
            }
        } else
            FD_SET(socket->handle, &writefds);
    }
    if(selection.empty())
        return;

    struct timeval timeout, *timeoutPtr = &timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    #ifdef WINVER
    if(writefds.fd_count > 0)
    #endif
    if(select(maxHandle+1, NULL, &writefds, NULL, timeoutPtr) == -1)
        throw Exception(Exception::ERROR_SELECT);

    foreach_e(selection, iterator) {
        forEachSocket()

        if(socket->type == Socket::Type::TCP_SERVER)
            continue;

        if(FD_ISSET(socket->handle, &writefds))
            socket->status = Socket::Status::READY;
        else if(socket->status != Socket::Status::CONNECTING)
            socket->status = Socket::Status::BUSY;

        if(onStatusChange && socket->status != prev)
            onStatusChange(this, *iterator, prev);
        checkSocketStillValid(sockets, iterator, socket)

        // Try to send data of MsgPack queue in socket
        if(socket->status != Socket::Status::READY)
            continue;

        if(msgPackSocket)
            while(msgPackSocket->queue.size()) {
                std::unique_ptr<MsgPack::Element>& element = msgPackSocket->queue.front();
                msgPackSocket->serializer << element;
                if(element)
                    break;
                msgPackSocket->queue.pop();
            }
        socket->pubsync();
    }
    if(selection.empty())
        return;

    if(waitUpToSeconds >= 0.0) {
        timeout.tv_sec = waitUpToSeconds;
        timeout.tv_usec = fmod(waitUpToSeconds, 1.0) * 1000000.0;
    } else
        timeoutPtr = NULL;

    if(select(maxHandle+1, &readfds, NULL, NULL, timeoutPtr) == -1)
        throw Exception(Exception::ERROR_SELECT);

    foreach_e(selection, iterator) {
        forEachSocket()

        if(!FD_ISSET(socket->handle, &readfds))
            continue;

        if(socket->type == Socket::Type::TCP_SERVER) {
            // Server got a new client
            std::shared_ptr<Socket> newSocket = socket->accept();
            if(onConnectRequest && !onConnectRequest(this, *iterator, newSocket)) {
                newSocket->disconnect();
                socket->clients.erase(newSocket);
            }
        } else {
            // Can not read: disconnect
            if(socket->showmanyc() <= 0) {
                socket->disconnect();
                removeSocket()
            }

            // Ensure that we only read data from the incoming packet (UDP)
            if(socket->type == Socket::Type::UDP_PEER)
                socket->advanceInputBuffer();
            // Received new data
            if(msgPackSocket && onReceiveMsgPack) {
                std::unique_ptr<MsgPack::Element> element;
                while(true) {
                    msgPackSocket->deserializer >> element;
                    if(!element)
                        break;
                    onReceiveMsgPack(this, *iterator, std::move(element));
                }
            } else if(onReceiveRaw)
                onReceiveRaw(this, *iterator);
            checkSocketStillValid(sockets, iterator, socket)
        }
    }
}

};
