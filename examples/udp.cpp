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

    //Define a callback, fired when a socket receives data
    socketManager.onReceiveMsgPack = [](netLink::SocketManager* manager, std::shared_ptr<netLink::Socket> socket, std::unique_ptr<MsgPack::Element> element) {
        //hostRemote and portRemote are now set to the origin of the last received message
        std::cout << "Received data from " << socket->hostRemote << ":" << socket->portRemote << ": " << *element << "\n";
    };

    //Alloc a new socket and insert it into the SocketManager
    std::shared_ptr<netLink::Socket> socket = socketManager.newMsgPackSocket();

    //Init socket as UDP listening to all incoming adresses (let the system choose IPv4 or IPv6) on port 3824
    socket->initAsUdpPeer("*", 3824);

    //Define the destination for the next sent message (depending on the choosen IP version)
    socket->hostRemote = (socket->getIPVersion() == netLink::IPv4) ? "224.0.0.1" : "FF02:0001::";
    socket->portRemote = socket->portLocal;

    //Join the multicast group to receive messages from the given address
    socket->setMulticastGroup(socket->hostRemote, true);

    //Prepare a MsgPack encoded message
    netLink::MsgPackSocket& msgPackSocket = *static_cast<netLink::MsgPackSocket*>(socket.get());
    msgPackSocket << new MsgPack::String("Test message");
    msgPackSocket << new MsgPack::ArrayHeader(3);
    msgPackSocket << new MsgPack::MapHeader(2);
    msgPackSocket << new MsgPack::String("Boolean");
    msgPackSocket << new MsgPack::Primitive(true);
    msgPackSocket << new MsgPack::String("Number");
    msgPackSocket << new MsgPack::Number(2487.348);
    msgPackSocket << new MsgPack::ArrayHeader(0);
    msgPackSocket << new MsgPack::Primitive();

    while(true) {
        //Let the SocketManager poll from all sockets, events will be triggered here
        socketManager.listen();
    }

    return 0;
}