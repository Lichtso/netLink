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

    // Define a callback, fired when a socket receives data
    socketManager.onReceiveMsgPack = [](netLink::SocketManager* manager, std::shared_ptr<netLink::Socket> socket, std::unique_ptr<MsgPack::Element> element) {
        // hostRemote and portRemote are now set to the origin of the last received message
        std::cout << "Received data from " << socket->hostRemote << ":" << socket->portRemote << ": " << *element << std::endl;
    };

    // Alloc a new socket and insert it into the SocketManager
    std::shared_ptr<netLink::Socket> socket = socketManager.newMsgPackSocket();

    try {
        // Init socket as UDP listening to all incoming adresses (let the system choose IPv4 or IPv6) on port 3824
        socket->initAsUdpPeer("*", 3824);
    } catch(netLink::Exception exc) {
        std::cout << "Address is already in use" << std::endl;
        return 1;
    }

    // Define the destination for the next sent message (depending on the choosen IP version)
    socket->hostRemote = (socket->getIPVersion() == netLink::Socket::IPv4) ? "224.0.0.100" : "FF02:0001::";
    socket->portRemote = socket->portLocal;

    // Join the multicast group to receive messages from the given address
    socket->setMulticastGroup(socket->hostRemote, true);

    // Prepare a MsgPack encoded message
    netLink::MsgPackSocket& msgPackSocket = *static_cast<netLink::MsgPackSocket*>(socket.get());
    msgPackSocket << MsgPack::Factory("Hello, Multicast Group!");
    msgPackSocket << MsgPack__Factory(ArrayHeader(3));
    msgPackSocket << MsgPack__Factory(MapHeader(2));
    msgPackSocket << MsgPack::Factory("Boolean");
    msgPackSocket << MsgPack::Factory(true);
    msgPackSocket << MsgPack::Factory("Number");
    msgPackSocket << MsgPack::Factory(2487.348);
    msgPackSocket << MsgPack__Factory(ArrayHeader(0));
    msgPackSocket << MsgPack::Factory();

    // Let the SocketManager poll from all sockets, events will be triggered here
    while(true)
        socketManager.listen();

    return 0;
}
