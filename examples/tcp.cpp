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

#include <iostream>
#include "../include/netLink.h"

int main(int argc, char** argv) {
    netLink::SocketManager socketManager;

    //Alloc a new socket, insert it into the SocketManager
    std::shared_ptr<netLink::Socket> socket(new netLink::Socket());
    socketManager.sockets.insert(socket);

    //Prepare a Deserializer which has to be persistent over serval 'onReceive' calls
    std::unique_ptr<MsgPack::Deserializer> deserializer;

    //Get hostname
    char hostname[256];
    FILE* f = popen("hostname -s", "r");
    fgets(hostname, 128, f);
    pclose(f);
    int len = strlen(hostname)-1;
    hostname[len] = 0;

    //Define a callback, fired when a new client tries to connect
    socketManager.onConnectRequest = [&deserializer](netLink::SocketManager* manager, netLink::Socket* serverSocket, netLink::Socket* socket) {
        std::cout << "Accepted connection from " << socket->hostRemote << ":" << socket->portRemote << "\n";

        //Alloc memory and a Deserializer for incoming messages
        socket->setInputBufferSize(10000);
        deserializer.reset(new MsgPack::Deserializer(socket));

        //Accept all new connections
        return true;
    };

    //Define a callback, fired when a sockets state changes
    socketManager.onStateChanged = [&hostname](netLink::SocketManager* manager, netLink::Socket* socket, netLink::SocketStatus prev) {
        if(prev == netLink::NOT_CONNECTED) {
            std::cout << "Connection got accepted at " << socket->hostRemote << ":" << socket->portRemote << "\n";

            //Prepare a MsgPack encoded message
            MsgPack::Serializer serializer(socket);
            serializer << new MsgPack::MapHeaderObject(2);
            serializer << "name";
            serializer << hostname;
            serializer << "message";
            serializer << "Hello World!";
            
            //Write all elements of the queue into the allocated output buffer of the socket and flush it
            socket->setOutputBufferSize(10000);
            serializer.serialize();
            socket->pubsync();
        }
    };

    //Define a callback, fired when a socket disconnects
    socketManager.onDisconnect = [](netLink::SocketManager* manager, netLink::Socket* socket) {
        std::cout << "Lost connection of " << socket->hostRemote << ":" << socket->portRemote << "\n";

        //Quit if the connection to the server is lost
        if(socket->getType() == netLink::TCP_CLIENT) {
            std::cout << "Quit\n";
            exit(0);
        }
    };

    //Define a callback, fired when a socket receives data
    socketManager.onReceive = [&deserializer](netLink::SocketManager* manager, netLink::Socket* socket) {
        try {
            //hostRemote and portRemote are now set to the origin of the last received message
            std::cout << "Received data from " << socket->hostRemote << ":" << socket->portRemote << "\n";

            //Let a MsgPack::Deserializer parse all data at once
            deserializer->deserialize([](std::unique_ptr<MsgPack::Object> parsedObject) {
                std::cout << *parsedObject << "\n";

                //Don't stop yet, try to parse more data
                return false;
            });
        }catch(netLink::Exception exc) {
            std::cout << "Exception " << exc.code << "\n";
        }
    };

    //Ask user for a nice IP address
    std::cout << "Enter a IP-Adress to connect to a sever or '*' to start a server:\n";
    while(socket->getStatus() == netLink::NOT_INITIALIZED) {
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

    while(true) {
        //Let the SocketManager poll from all sockets, events will be triggered here
        socketManager.listen();
    }

    return 0;
}