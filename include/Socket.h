/*
    netLink: C++11 networking library
    Copyright 2013 Alexander Meißner (lichtso@gamefortec.net)
*/

#ifndef netLink_Socket
#define netLink_Socket

#include "Core.h"

namespace netLink {

    //! Socket and stream buffer
    class Socket : public std::streambuf {
        std::streamsize inputIntermediateSize = 0;

        struct AddrinfoDestructor {
            AddrinfoDestructor() { };
            void operator() (struct addrinfo* res) const {
                freeaddrinfo(res);
            }
        };
        typedef std::unique_ptr<struct addrinfo, AddrinfoDestructor> AddrinfoContainer;
        AddrinfoContainer getSocketInfoFor(const char* host, unsigned int port, bool wildcardAddress);

        void setMulticastGroup(const struct sockaddr* addr, bool join);
        std::streamsize xsgetn(char_type* buffer, std::streamsize size);
        std::streamsize xsputn(const char_type* buffer, std::streamsize size);
        //! Receives all available bytes and returns the first byte else -1 on failure
        int_type syncInputBuffer();
        //! Sends all outstanding bytes and returns 0 else -1 on failure
        int_type sync();
        //! Checks if the Socket is not UDP and then returns syncInputBuffer()
        int_type underflow();
        //! sync() and writes c in the output buffer
        int_type overflow(int_type c = -1);
        
        void initSocket(bool blocking);
        Socket(int handle, const std::string& hostLocal, unsigned portLocal,
               struct sockaddr* remoteAddr, IPVer ipVer);
        
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

};

#endif
