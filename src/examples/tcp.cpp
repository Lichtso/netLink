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

#include <iostream>
#include "../include/netLink.h"

int main(int argc, char** argv) {
    #ifdef WINVER
    netLink::init();
    #endif

    netLink::SocketManager socketManager;

    // Allocate a new socket and insert it into the SocketManager
    std::shared_ptr<netLink::Socket> socket = socketManager.newMsgPackSocket();

    // Define a callback, fired when a new client tries to connect
    socketManager.onConnectRequest = [](netLink::SocketManager* manager, std::shared_ptr<netLink::Socket> serverSocket, std::shared_ptr<netLink::Socket> clientSocket) {
        std::cout << "Accepted connection from " << clientSocket->hostRemote << ":" << clientSocket->portRemote << std::endl;

        // Accept all new connections
        return true;
    };

    // Define a callback, fired when a sockets state changes
    socketManager.onStatusChange = [](netLink::SocketManager* manager, std::shared_ptr<netLink::Socket> socket, netLink::Socket::Status prev) {
        netLink::MsgPackSocket& msgPackSocket = *static_cast<netLink::MsgPackSocket*>(socket.get());

        switch(socket->getStatus()) {
            case netLink::Socket::Status::READY:
                std::cout << "Connection got accepted at " << socket->hostRemote << ":" << socket->portRemote << std::endl;

                // Prepare a MsgPack encoded message
                msgPackSocket << MsgPack__Factory(MapHeader(2));
                msgPackSocket << MsgPack::Factory("type");
                msgPackSocket << MsgPack::Factory("post");
                msgPackSocket << MsgPack::Factory("message");
                msgPackSocket << MsgPack::Factory("Hello World!");
            break;
            case netLink::Socket::Status::NOT_CONNECTED:
                if(prev == netLink::Socket::Status::CONNECTING)
                    std::cout << "Connecting to " << socket->hostRemote << ":" << socket->portRemote << " failed" << std::endl;
                else
                    std::cout << "Lost connection of " << socket->hostRemote << ":" << socket->portRemote << std::endl;
            break;
            default:
                std::cout << "Status of " << socket->hostRemote << ":" << socket->portRemote << " changed from " << prev << " to " << socket->getStatus() << std::endl;
            break;
        }
    };

    // Define a callback, fired when a socket receives data
    socketManager.onReceiveMsgPack = [](netLink::SocketManager* manager, std::shared_ptr<netLink::Socket> socket, std::unique_ptr<MsgPack::Element> element) {
        // hostRemote and portRemote are now set to the origin of the last received message
        std::cout << "Received data from " << socket->hostRemote << ":" << socket->portRemote << ": " << *element << std::endl;

        // Parse the *element
        auto elementMap = dynamic_cast<MsgPack::Map*>(element.get());
        if(elementMap) { // The received element is a map
            // Iterate them sequentially
            std::cout << "Iterate them sequentially:" << std::endl;
            auto container = elementMap->getElementsMap();
            for(auto& pair : container)
                std::cout << pair.first << " : " << *pair.second << std::endl;
            // Or just access them directly
            if(elementMap->getLength() >= 2) {
                std::cout << "Access them directly:" << std::endl;
                std::cout << elementMap->getKey(0)->stdString() << " : " << *elementMap->getValue(0) << std::endl;
                std::cout << elementMap->getKey(1)->stdString() << " : " << *elementMap->getValue(1) << std::endl;
                // ... and so on
            }
        }
    };

    // Ask user for a nice IP address
    std::cout << "Enter a IP-Adress to connect to a sever or '*' to start a server:" << std::endl;
    while(1) {
        try {
            std::cin >> socket->hostRemote;
            // Init socket as TCP server or client on port 3823
            if(socket->hostRemote == "*")
                socket->initAsTcpServer("*", 3823);
            else
                socket->initAsTcpClient(socket->hostRemote, 3823);
            break;
        } catch(netLink::Exception exc) {
            std::cout << "Address is already in use, please try again..." << std::endl;
        }
    }

    // Let the SocketManager poll from all sockets, events will be triggered here
    while(socket->getStatus() != netLink::Socket::Status::NOT_CONNECTED)
        socketManager.listen();

    std::cout << "Quit" << std::endl;

    return 0;
}
