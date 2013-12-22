/*
    netLink: C++11 networking library
    Copyright 2013 Alexander Meißner (lichtso@gamefortec.net)
*/

#ifndef netLink_SocketManager
#define netLink_SocketManager

#include "Socket.h"

namespace netLink {

    //! Manages a group of Sockets
    class SocketManager {
        public:
        std::set<Socket*> sockets; //!< Sockets which are managed
        
        //! Event which is called if a TCP_SERVER accepts a new connection (if false is returned the connection will be closed immediately)
        std::function<bool(SocketManager* manager, Socket* serverSocket, Socket* newSocket)> onAcceptRequest;
        //! Event which is called if a socket can or can not send more data (also called if nonblocking connect succeeded)
        std::function<void(SocketManager* manager, Socket* socket, SocketSendStatus prev)> onStateChanged;
        //! Event which is called if a socket is disconnected (or if nonblocking connect failed)
        std::function<void(SocketManager* manager, Socket* socket)> onDisconnect;
        //! Event which is called if a socket receives new data
        std::function<void(SocketManager* manager, Socket* socket)> onReceive;
        
        virtual ~SocketManager();
        
        /*! Listens a periode time
         @param sec Time to wait in seconds
         */
        void listen(double sec = 0.0);
    };

};

#endif
