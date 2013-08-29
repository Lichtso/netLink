/*
    NetLink Sockets: Networking C++ library
    Copyright 2012 Pedro Francisco Pareja Ruiz (PedroPareja@Gmail.com)
    Modified 2013 Alexander Meißner (lichtso@gamefortec.net)

    This file is part of NetLink Sockets.

    NetLink Sockets is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    NetLink Sockets is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with NetLink Sockets. If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef NL_SOCKET
#define NL_SOCKET

#include "Exception.h"

NL_NAMESPACE

//! Socket and stream buffer
class Socket : public std::streambuf {
    unsigned int inputIntermediateSize = 0;
    
    struct AddrinfoDestructor {
        AddrinfoDestructor() { };
        void operator() (struct addrinfo* res) const {
            freeaddrinfo(res);
        };
    };
    std::unique_ptr<struct addrinfo, AddrinfoDestructor> getSocketInfoFor(const char* host, unsigned int port, bool wildcardAddress);
    
    void setMulticastGroup(struct sockaddr_storage* addr, bool join);
    int_type refillInputIntermediate();
    std::streamsize xsgetn(char_type* buffer, std::streamsize size);
    std::streamsize xsputn(const char_type* buffer, std::streamsize size);
    int sync();
    int_type underflow();
    int_type overflow(int_type c = traits_type::eof());
    
    void initSocket(bool blocking);
    Socket(int handle, const std::string& hostLocal, unsigned portLocal,
           struct sockaddr_storage* remoteAddr, IPVer ipVer);
    
    public:
    int handle = -1; //!< Handle used for the system interface
    std::string hostLocal, //!< Host string of local
                hostRemote; //!< Host string of remote
    unsigned int portLocal = 0, //!< Port of local
                 portRemote = 0, //!< Port of remote
                 recvStatus = SOCKET_STATUS_NOT_CONNECTED; //!< Or listen queue size if socket is TCP_SERVER
    IPVer ipVer = ANY;
    SocketType type = NONE;
    
    /*! Setup socket as TCP client
     @param hostRemote The remote host to connect to
     @param portRemote The remote port to connect to
     @param waitUntilConnected Set blocking mode until connected
     */
    void initAsTcpClient(const std::string& hostRemote, unsigned portRemote, bool waitUntilConnected = false);
    
    /*! Setup socket as TCP server
     @param hostLocal The host to be listening to:
     
     "" or "*" to listen to any incoming data (ipVer will be choosen by system)
     "0.0.0.0" to listen to any incoming data (ipVer will be IPv4)
     "::0" to listen to any incoming data (ipVer will be IPv6)
     @param portLocal The local port
     @param listenQueue Queue size for outstanding sockets to accept
     */
    void initAsTcpServer(const std::string& hostLocal, unsigned portLocal, unsigned listenQueue = 16);
    
    /*! Setup socket as UDP server
     @param hostLocal The host to be listening to:
     
     "" or "*" to listen to any incoming data (ipVer will be choosen by system)
     "0.0.0.0" to listen to any incoming data (ipVer will be IPv4)
     "::0" to listen to any incoming data (ipVer will be IPv6)
     A multicast address of the multicast group to listen to
     @param portLocal The local port
     */
    void initAsUdpPeer(const std::string& hostLocal, unsigned portLocal);
    
    Socket() { };
    virtual ~Socket();
    
    //! Returns the outstanding bytes in system cache to read
    std::streamsize showmanyc();
    //! Returns showmanyc() plus the bytes in the input intermediate buffer
    std::streamsize in_avail();
    //! Writes the outstanding bytes from the input intermediate buffer (and new data from system cache if possible) into buffer
    std::streamsize sgetn(char_type* buffer, std::streamsize size);
    //! Sends the outstanding bytes from the output intermediate buffer and data from buffer
    std::streamsize sputn(const char_type* buffer, std::streamsize size);
    
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
    /*! Joines or leaves a muticast group
     @param address Address of the group
     @param join If true it joines the group else it leaves it
     @pre Type needs to be UDP_PEER
     */
    void setMulticastGroup(const std::string& address, bool join);
    /*! Clears the input intermediate buffer and fills it with the next packet
     @return Size of the read packet or 0 on error
     @pre Type needs to be UDP_PEER and the input intermediate buffer must be used
     */
    std::streamsize advanceToNextPacket();
    /*! Accepts a TCP connection and returns it
     @return The new accepted socket (type will be TCP_SERVERS_CLIENT)
     @pre Type needs to be TCP_SERVER
     */
    Socket* accept();
    //! Disconnects the socket, deletes the intermediate buffers and sets the handle to -1
    void disconnect();
};

NL_NAMESPACE_END

#endif
