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

#include <iostream>
#include "../include/netLink.h"

int main(int argc, char** argv) {
    netLink::SocketManager socketManager;

    //Allocate a new socket and insert it into the SocketManager
    std::shared_ptr<netLink::Socket> socket = socketManager.newMsgPackSocket();

    //Get hostname
    char hostname[256];
    FILE* f = popen("hostname -s", "r");
    fgets(hostname, 128, f);
    pclose(f);
    int len = strlen(hostname)-1;
    hostname[len] = 0;

    //Define a callback, fired when a new client tries to connect
    socketManager.onConnectRequest = [](netLink::SocketManager* manager, std::shared_ptr<netLink::Socket> serverSocket, std::shared_ptr<netLink::Socket> clientSocket) {
        std::cout << "Accepted connection from " << clientSocket->hostRemote << ":" << clientSocket->portRemote << "\n";

        //Accept all new connections
        return true;
    };

    //Define a callback, fired when a sockets state changes
    socketManager.onStatusChanged = [&hostname](netLink::SocketManager* manager, std::shared_ptr<netLink::Socket> socket, netLink::SocketStatus prev) {
        if(prev == netLink::CONNECTING) {
            std::cout << "Connection got accepted at " << socket->hostRemote << ":" << socket->portRemote << "\n";

            //Prepare a MsgPack encoded message
            netLink::MsgPackSocket& msgPackSocket = *static_cast<netLink::MsgPackSocket*>(socket.get());
            msgPackSocket << new MsgPack::MapHeader(2);
            msgPackSocket << new MsgPack::String("name");
            msgPackSocket << new MsgPack::String(hostname);
            msgPackSocket << new MsgPack::String("message");
            msgPackSocket << new MsgPack::String("Hello World!");
        }
    };

    //Define a callback, fired when a socket disconnects
    socketManager.onDisconnect = [](netLink::SocketManager* manager, std::shared_ptr<netLink::Socket> socket) {
        std::cout << "Lost connection of " << socket->hostRemote << ":" << socket->portRemote << "\n";
    };

    //Define a callback, fired when a socket receives data
    socketManager.onReceiveMsgPack = [](netLink::SocketManager* manager, std::shared_ptr<netLink::Socket> socket, std::unique_ptr<MsgPack::Element> element) {
        //hostRemote and portRemote are now set to the origin of the last received message
        std::cout << "Received data from " << socket->hostRemote << ":" << socket->portRemote << ": " << *element << "\n";
    };

    //Ask user for a nice IP address
    std::cout << "Enter a IP-Adress to connect to a sever or '*' to start a server:\n";
    while(socket->getStatus() == netLink::NOT_CONNECTED) {
        try {
            std::cin >> socket->hostRemote;
        
            //Init socket as TCP server or client on port 3823
            if(socket->hostRemote == "*")
                socket->initAsTcpServer("*", 3823);
            else
                socket->initAsTcpClient(socket->hostRemote, 3823);
        }catch(netLink::Exception exc) {
            std::cout << "Could not resolve address, please try again...\n";
        }
    }

    while(socket->getStatus() != netLink::NOT_CONNECTED) {
        //Let the SocketManager poll from all sockets, events will be triggered here
        socketManager.listen();
    }

    std::cout << "Quit\n";

    return 0;
}