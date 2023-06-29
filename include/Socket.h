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

#pragma once

#include "Core.h"

namespace netLink {

    class SocketManager;

    //! Socket and stream buffer
    class Socket : public std::streambuf {
        typedef std::streambuf super; //!< Typedef of super class
        friend class SocketManager;

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
        std::streamsize inputIntermediateSize;
        std::streamsize xsgetn(char_type* buffer, std::streamsize size);
        int_type underflow();

        //Output functions (put)
        std::streamsize xsputn(const char_type* buffer, std::streamsize size);
        int_type overflow(int_type c = -1);

        /*! Shifts the remaining data to the beginning of the input intermediate buffer
            and fills up the input intermediate buffer by receiving data (TCP)
            or writes the next received packet at the beginning of the input intermediate buffer (UDP)
         @return Size of the read packet or 0 on error
         @pre The input intermediate buffer must be used
         */
        std::streamsize advanceInputBuffer();

        public:
        //! Defines the version of IP.
        enum IPVersion {
            IPv4, //!< IPv4 only
            IPv6, //!< IPv6 only
            ANY //!< IPv4 or IPv6 (choosen by the system)
        };

        //! Defines the nature of a socket.
        enum Type {
            NONE, //!< No type defined
            TCP_CLIENT, //!< TCP socket connecting to a server
            TCP_SERVER, //!< TCP socket waiting for TCP_CLIENT to connect
            TCP_SERVERS_CLIENT, //!< TCP socket to represent a TCP_CLIENT connection at the TCP_SERVER
            UDP_PEER //!< UDP socket
        };

        //! Defines the send status of a socket.
        enum Status {
            NOT_CONNECTED, //!< Socket is not even initialized or disconnected
            CONNECTING, //!< Socket is initialized but not connected yet and can not send or receive data
            LISTENING, //!< Socket is a server and can neither send nor receive data
            READY, //!< Socket is connected, can send and receive data
            BUSY //!< Socket is connected and can not send but receive data (at the moment)
        };

        protected:
        IPVersion ipVersion; //!< IP version which is in use
        Type type; //!< Type of the socket
        unsigned int status; //!< Or listen queue size if socket is TCP_SERVER
        int handle; //!< Handle used for the system interface
        /*! Initzialize system handle
         @param blocking Waits for connection if true
        */
        void initSocket(bool blocking);
        //! Generates new sockets for client connections of a server
        virtual std::shared_ptr<Socket> SocketFactory() {
            return std::shared_ptr<Socket>(new Socket());
        }

        public:
        std::set<std::shared_ptr<Socket>> clients; //!< Client sockets of a server
        std::string hostLocal, //!< Host string of local
                    hostRemote; //!< Host string of remote
        unsigned int portLocal, //!< Port of local
                     portRemote; //!< Port of remote

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

        Socket();
        virtual ~Socket();

        //! Returns the IPVersion of the socket
        IPVersion getIPVersion() const;

        //! Returns the SocketType of the socket
        Type getType() const;

        //! Returns the SocketStatus of the socket
        Status getStatus() const;

        /*! Returns only the number of outstanding bytes to be received from the system cache
         @return Number of bytes in the system cache, not including iostream buffers
         @warning Use in_avail() instead if you are interested in the total number of bytes which can be read
        */
        std::streamsize showmanyc();
        /*! Receives size bytes into buffer
         @return The actual number of bytes which were received
         @pre Type must not be TCP_SERVER and status must be READY or BUSY
         @warning In most cases you don't want to call this directly but use the iostream API instead (see wiki)
        */
        std::streamsize receive(char_type* buffer, std::streamsize size);
        /*! Sends size bytes from buffer
         @return The actual number of bytes which were sent
         @pre Type must not be TCP_SERVER and status must be READY
         @warning In most cases you don't want to call this directly but use the iostream API instead (see wiki)
        */
        std::streamsize send(const char_type* buffer, std::streamsize size);
        /*! Redirects received data to all Sockets in destinations
         @param destinations A list of destinations to sent the data to
         @return The actual number of bytes which were redirected
         @warning If one destination can't handle the outgoing load, a Expection is thrown and all other destinations get out of sync (data loss)
        */
        std::streamsize redirect(const std::vector<std::shared_ptr<Socket>>& destinations);

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
        std::shared_ptr<Socket> accept();
        //! Disconnects the socket, deletes the intermediate buffers and sets the handle to -1
        void disconnect();
        //! Check if there is a problem with this socket and disconnect in case there is one
        void disconnectOnError();
    };

};
