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

#pragma once

#include "Core.h"

namespace netLink {

    class SocketManager;

    //! Socket and stream buffer
    class Socket : public std::basic_streambuf<uint8_t> {
        typedef std::basic_streambuf<uint8_t> super;
        friend SocketManager;

        struct AddrinfoDestructor {
            AddrinfoDestructor() { };
            void operator() (struct addrinfo* res) const {
                freeaddrinfo(res);
            }
        };
        typedef std::unique_ptr<struct addrinfo, AddrinfoDestructor> AddrinfoContainer;
        AddrinfoContainer getSocketInfoFor(const char* host, unsigned int port, bool wildcardAddress);

        //Buffer management and positioning
        pos_type seekoff(off_type off, std::ios_base::seekdir way, std::ios_base::openmode which = std::ios_base::in | std::ios_base::out);
        pos_type seekpos(pos_type sp, std::ios_base::openmode which = std::ios_base::in | std::ios_base::out);
		int sync();

        //Input functions (get)
        std::streamsize inputIntermediateSize = 0;
        std::streamsize xsgetn(char_type* buffer, std::streamsize size);
        int_type underflow();

        //Output functions (put)
        std::streamsize xsputn(const char_type* buffer, std::streamsize size);
        int_type overflow(int_type c = -1);
        
        IPVersion ipVersion = ANY;
        SocketType type = NONE;
        unsigned int status = NOT_INITIALIZED; //!< Or listen queue size if socket is TCP_SERVER
        int handle = -1; //!< Handle used for the system interface

        void initSocket(bool blocking);
        void setMulticastGroup(const struct sockaddr* addr, bool join);
        Socket(int handle, const std::string& hostLocal, unsigned portLocal,
               struct sockaddr* remoteAddr, IPVersion ipVersion);
        
        public:
        std::string hostLocal, //!< Host string of local
                    hostRemote; //!< Host string of remote
        unsigned int portLocal = 0, //!< Port of local
                     portRemote = 0; //!< Port of remote
        
        /*! Setup socket as TCP client
         @param hostRemote The remote host to connect to
         @param portRemote The remote port to connect to
         @param waitUntilConnected Set blocking mode until connected
         */
        void initAsTcpClient(const std::string& hostRemote, unsigned portRemote, bool waitUntilConnected = false);
        
        /*! Setup socket as TCP server
         @param hostLocal The host to be listening to:
         
         "" or "*" to listen to any incoming data (IPVersion will be choosen by system)
         "0.0.0.0" to listen to any incoming data (IPVersion will be IPv4)
         "::0" to listen to any incoming data (IPVersion will be IPv6)
         @param portLocal The local port
         @param listenQueue Queue size for outstanding sockets to accept
         */
        void initAsTcpServer(const std::string& hostLocal, unsigned portLocal, unsigned listenQueue = 16);
        
        /*! Setup socket as UDP server
         @param hostLocal The host to be listening to:
         
         "" or "*" to listen to any incoming data (IPVersion will be choosen by system)
         "0.0.0.0" to listen to any incoming data (IPVersion will be IPv4)
         "::0" to listen to any incoming data (IPVersion will be IPv6)
         A multicast address of the multicast group to listen to
         @param portLocal The local port
         */
        void initAsUdpPeer(const std::string& hostLocal, unsigned portLocal);
        
        Socket() { };
        virtual ~Socket();

        //! Returns the IPVersion of the socket
        IPVersion getIPVersion() const;

        //! Returns the SocketType of the socket
        SocketType getType() const;

        //! Returns the SocketStatus of the socket
        SocketStatus getStatus() const;

        //! Returns the outstanding bytes in system cache to read
        std::streamsize showmanyc();
        /*! Shifts the remaining data to the beginning of the input intermediate buffer
            and fills up the input intermediate buffer by receiving data if TCP
            or writes the next received packet at the beginning of the input intermediate buffer if UDP
         @return Size of the read packet or 0 on error
         @pre The input intermediate buffer must be used
         */
        std::streamsize advanceInputBuffer();
        //! Receives size bytes into buffer
        std::streamsize receive(char_type* buffer, std::streamsize size);
        //! Sends size bytes from buffer
        std::streamsize send(const char_type* buffer, std::streamsize size);
        
        //! Get the size of the input intermediate buffer in bytes
        std::streamsize getInputBufferSize();
        //! Get the size of the output intermediate buffer in bytes
        std::streamsize getOutputBufferSize();
        //! Set the size of the input intermediate buffer in bytes (unread contents are lost)
        void setInputBufferSize(std::streamsize size);
        //! Set the size of the output intermediate buffer in bytes (unwritten contents are lost)
        void setOutputBufferSize(std::streamsize size);
        
        /*! Updates the blocking mode of the socket.
         A socket will be non blocking by default.
         Try to avoid using blocking mode. Use a SocketManager with listen(sec > 0.0) instead.
         */
        void setBlockingMode(bool blocking);
        /*! Enables or disables the broadcasting (only relevant for sending)
         @param active If true enables broadcasting else disables broadcasting
         @pre Type needs to be UDP_PEER and IPv4
         */
        void setBroadcast(bool active);
        /*! Joines or leaves a muticast group (only relevant for receiving)
         @param address Address of the group
         @param join If true it joines the group else it leaves it
         @pre Type needs to be UDP_PEER
         */
        void setMulticastGroup(const std::string& address, bool join);
        /*! Accepts a TCP connection and returns it
         @return The new accepted socket (type will be TCP_SERVERS_CLIENT)
         @pre Type needs to be TCP_SERVER
         */
        std::unique_ptr<Socket> accept();
        //! Disconnects the socket, deletes the intermediate buffers and sets the handle to -1
        void disconnect();
    };

};