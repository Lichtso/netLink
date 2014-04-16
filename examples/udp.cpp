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

    //Define a callback, fired when a socket receives data
    socketManager.onReceive = [](netLink::SocketManager* manager, std::shared_ptr<netLink::Socket> socket) {
        try {
            //Because we are using UDP we want to ensure that we only read data from the incoming packet
            socket->advanceInputBuffer();

            //hostRemote and portRemote are now set to the origin of the last received message
            std::cout << "Received data from " << socket->hostRemote << ":" << socket->portRemote << "\n";

            //Let a MsgPack::Deserializer parse all data at once
            MsgPack::Deserializer deserializer(socket.get());
            deserializer.deserialize([](std::unique_ptr<MsgPack::Element> element) {
                std::cout << *element << "\n";

                //Don't stop yet, try to parse more data
                return false;
            });
        }catch(netLink::Exception exc) {
            std::cout << "Exception " << exc.code << "\n";
        }
    };

    //Alloc a new socket and insert it into the SocketManager
    std::shared_ptr<netLink::Socket> socket = socketManager.generateSocket();

    //Init socket as UDP listening to all incoming adresses (let the system choose IPv4 or IPv6) on port 3824
    socket->initAsUdpPeer("*", 3824);

    //Set the size of the intermediate buffers needed when using streams of UDP sockets
    socket->setInputBufferSize(10000);
    socket->setOutputBufferSize(10000);

    //Define the destination for the next sent message (depending on the choosen IP version)
    socket->hostRemote = (socket->getIPVersion() == netLink::IPv4) ? "224.0.0.1" : "FF02:0001::";
    socket->portRemote = socket->portLocal;

    //Join the multicast group to receive messages from the given address
    socket->setMulticastGroup(socket->hostRemote, true);

    //Prepare a MsgPack encoded message
    MsgPack::Serializer serializer(socket.get());
    serializer << "Test message";
    serializer << new MsgPack::ArrayHeader(3);
    serializer << new MsgPack::MapHeader(2);
    serializer << "Boolean";
    serializer << true;
    serializer << "Number";
    serializer << 2487.348;
    serializer << new MsgPack::ArrayHeader(0);
    serializer << new MsgPack::Primitive();

    //Write all elements of the queue into the output buffer of the socket and flush it
    serializer.serialize();
    socket->pubsync();

    while(true) {
        //Let the SocketManager poll from all sockets, events will be triggered here
        socketManager.listen();
    }

    return 0;
}