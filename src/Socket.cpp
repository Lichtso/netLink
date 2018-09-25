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

#include "Socket.h"

namespace netLink {

#ifdef WINVER
void init() {
    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
        throw Exception(Exception::ERROR_INIT);
}
#define snprintf _snprintf_s
#else
#define closesocket close
#endif

static void readSockaddr(const struct sockaddr_storage* addr, std::string& host, unsigned int& port) {
    char buffer[INET6_ADDRSTRLEN];
    if(addr->ss_family == AF_INET) {
        auto sin = reinterpret_cast<const struct sockaddr_in*>(addr);
        port = ntohs(sin->sin_port);
        inet_ntop(addr->ss_family, (void*)&(sin->sin_addr), buffer, sizeof(buffer));
    } else {
        auto sin = reinterpret_cast<const struct sockaddr_in6*>(addr);
        port = ntohs(sin->sin6_port);
        inet_ntop(addr->ss_family, (void*)&(sin->sin6_addr), buffer, sizeof(buffer));
    }
    host = buffer;
}

Socket::AddrinfoContainer Socket::getSocketInfoFor(const char* host, unsigned int port, bool wildcardAddress) {
    struct addrinfo conf, *res;
    memset(&conf, 0, sizeof(conf));
    conf.ai_flags = AI_V4MAPPED;
    switch(ipVersion) {
        case IPv4:
            conf.ai_family = AF_INET;
            break;
        case IPv6:
            conf.ai_family = AF_INET6;
            break;
        default:
            conf.ai_family = AF_UNSPEC;
            break;
    }
    if(wildcardAddress)
        conf.ai_flags |= AI_PASSIVE;
    switch(type) {
        case TCP_CLIENT:
        case TCP_SERVER:
            conf.ai_socktype = SOCK_STREAM;
            break;
        case UDP_PEER:
            conf.ai_socktype = SOCK_DGRAM;
            break;
        default:
            disconnect();
            throw Exception(Exception::BAD_PROTOCOL);
    }
    char portStr[10];
    snprintf(portStr, 10, "%u", port);
    int result = getaddrinfo(host, portStr, &conf, &res);
    if(result != 0)
        throw Exception(Exception::ERROR_RESOLVING_ADDRESS);
    return AddrinfoContainer(res);
}

Socket::pos_type Socket::seekoff(Socket::off_type off, std::ios_base::seekdir way, std::ios_base::openmode which) {
    switch(way) {
        case std::ios_base::beg:
            if(off < 0)
                break;
            if(which & std::ios_base::in) {
                if(eback() + off >= egptr())
                    break;
                setg(eback(), eback()+off, egptr());
            }
            if(which & std::ios_base::out) {
                if(pbase() + off >= epptr())
                    break;
                setp(pbase(), epptr());
                pbump(off);
            }
        return off;
        case std::ios_base::cur:
            if(which == std::ios_base::in) {
                if(eback() + off >= egptr() || egptr() + off < eback())
                    break;
                setg(eback(), gptr()+off, egptr());
                return gptr()-eback();
            } else if(which == std::ios_base::out) {
                if(pbase() + off >= epptr() || epptr() + off < pbase())
                    break;
                pbump(off);
                return pptr()-pbase();
            } else
                break;
        case std::ios_base::end:
            if(off > 0)
                break;
            if(which == std::ios_base::in) {
                if(egptr() + off < eback())
                    break;
                setg(eback(), egptr()+off, egptr());
                return gptr()-eback();
            } else if(which == std::ios_base::out) {
                if(epptr() + off < pbase())
                    break;
                setp(pbase(), epptr());
                pbump(off);
                return pptr()-pbase();
            } else
                break;
    }
    return EOF;
}

Socket::pos_type Socket::seekpos(Socket::pos_type sp, std::ios_base::openmode which) {
    return Socket::seekoff(sp, std::ios_base::beg, which);
}

int Socket::sync() {
    if(getOutputBufferSize() == 0) // No output buffer
        return EOF;
    if(pptr() == pbase()) // Allready in sync
        return 0;
    try {
        std::streamsize rest = pptr()-pbase(),
                        sentBytes = send(pbase(), rest);
        rest -= sentBytes;
        memmove(pbase(), pbase()+sentBytes, rest);
        setp(pbase(), epptr());
        pbump(rest);
    } catch(Exception err) {
        return EOF;
    }
    return 0;
}

std::streamsize Socket::xsgetn(char_type* buffer, std::streamsize size) {
    if(getInputBufferSize()) // Read from input buffer
        return super::xsgetn(buffer, size);
    try {
        return receive(buffer, size);
    } catch(Exception err) {
        return 0;
    }
}

Socket::int_type Socket::underflow() {
    if(type == UDP_PEER || advanceInputBuffer() <= 0)
        return EOF;
    return *eback();
}

std::streamsize Socket::xsputn(const char_type* buffer, std::streamsize size) {
    if(getOutputBufferSize()) // Write into buffer
        return super::xsputn(buffer, size);
    try {
        return send(buffer, size);
    } catch(Exception err) {
        return 0;
    }
}

Socket::int_type Socket::overflow(int_type c) {
    if(sync() == EOF)
        return EOF;
    *pbase() = c;
    pbump(1);
    return c;
}



void Socket::initSocket(bool blockingConnect) {
    AddrinfoContainer info;
    if(type == TCP_CLIENT)
        info = getSocketInfoFor(hostRemote.c_str(), portRemote, false);
    else {
        const char* host;
        if(!hostLocal.compare("") || !hostLocal.compare("*"))
            host = NULL;
        else
            host = hostLocal.c_str();
        info = getSocketInfoFor(host, portLocal, true);
    }
    struct addrinfo* nextAddr = info.get();
    while(nextAddr) {
        handle = socket(nextAddr->ai_family, nextAddr->ai_socktype, nextAddr->ai_protocol);
        if(handle == -1) {
            nextAddr = nextAddr->ai_next;
            continue;
        }
        switch(nextAddr->ai_family) {
            case AF_INET:
                ipVersion = IPv4;
            break;
            case AF_INET6:
                ipVersion = IPv6;
            break;
        }
        setBlockingMode(blockingConnect);
        #ifdef WINVER
        char flag = 1;
        #else
        int flag = 1;
        if(setsockopt(handle, SOL_SOCKET, SO_REUSEPORT, reinterpret_cast<const char*>(&flag), sizeof(flag)) == -1) {
            disconnect();
            throw Exception(Exception::ERROR_SET_SOCK_OPT);
        }
        #endif
        if(setsockopt(handle, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&flag), sizeof(flag)) == -1) {
            disconnect();
            throw Exception(Exception::ERROR_SET_SOCK_OPT);
        }
        if(ipVersion == IPv6) {
            int flag = 0;
            if(setsockopt(handle, IPPROTO_IPV6, IPV6_V6ONLY, reinterpret_cast<const char*>(&flag), sizeof(flag)) == -1) {
                disconnect();
                throw Exception(Exception::ERROR_SET_SOCK_OPT);
            }
        }
        switch(type) {
            case NONE:
            case TCP_SERVERS_CLIENT:
                disconnect();
                throw Exception(Exception::BAD_TYPE);
            case TCP_CLIENT:
                if(connect(handle, nextAddr->ai_addr, nextAddr->ai_addrlen) == -1 && blockingConnect) {
                    closesocket(handle);
                    handle = -1;
                } else if(blockingConnect)
                    status = READY;
                else
                    status = CONNECTING;
            break;
            case TCP_SERVER: {
                if(bind(handle, nextAddr->ai_addr, nextAddr->ai_addrlen) == -1) {
                    closesocket(handle);
                    handle = -1;
                }
                if(listen(handle, status) == -1) {
                    disconnect();
                    throw Exception(Exception::ERROR_INIT);
                }
            } break;
            case UDP_PEER: {
                if(bind(handle, nextAddr->ai_addr, nextAddr->ai_addrlen) == -1) {
                    closesocket(handle);
                    handle = -1;
                }
                status = READY;
            } break;
        }
        if(handle == -1) {
            nextAddr = nextAddr->ai_next;
            continue;
        }
        if(blockingConnect)
            setBlockingMode(false);
        break;
    }
    if(handle == -1) {
        disconnect();
        throw Exception(Exception::ERROR_INIT);
    }
    struct sockaddr_storage localAddr;
    #ifdef WINVER
    int size = sizeof(localAddr);
    #else
    unsigned int size = sizeof(localAddr);
    #endif
    if(getsockname(handle, reinterpret_cast<struct sockaddr*>(&localAddr), &size) != 0) {
        disconnect();
        throw Exception(Exception::ERROR_GET_SOCK_NAME);
    }
    readSockaddr(&localAddr, hostLocal, portLocal);
    setInputBufferSize(NETLINK_DEFAULT_INPUT_BUFFER_SIZE);
    setOutputBufferSize(NETLINK_DEFAULT_OUTPUT_BUFFER_SIZE);
}

void Socket::initAsTcpClient(const std::string& _hostRemote, unsigned _portRemote, bool waitUntilConnected) {
    type = TCP_CLIENT;
    hostRemote = _hostRemote;
    portRemote = _portRemote;
    initSocket(waitUntilConnected);
}

void Socket::initAsTcpServer(const std::string& _hostLocal, unsigned _portLocal, unsigned _listenQueue) {
    type = TCP_SERVER;
    hostLocal = _hostLocal;
    portLocal = _portLocal;
    status = _listenQueue;
    initSocket(false);
}

void Socket::initAsUdpPeer(const std::string& _hostLocal, unsigned _portLocal) {
    type = UDP_PEER;
    hostLocal = _hostLocal;
    portLocal = _portLocal;
    initSocket(false);
}

Socket::Socket() :ipVersion(ANY), type(NONE), status(NOT_CONNECTED),
    handle(-1), portLocal(0), portRemote(0) { }

Socket::~Socket() {
    disconnect();
}

Socket::IPVersion Socket::getIPVersion() const {
    return ipVersion;
}

Socket::Type Socket::getType() const {
    return type;
}

Socket::Status Socket::getStatus() const {
    if(type == TCP_SERVER)
        return (status == NOT_CONNECTED) ? NOT_CONNECTED : LISTENING;
    else
        return (Socket::Status)status;
}

std::streamsize Socket::showmanyc() {
    #ifdef WINVER
    unsigned long result = 0;
    if(ioctlsocket(handle, FIONREAD, &result)) {
    #else
    int result = 0;
    if(ioctl(handle, FIONREAD, &result)) {
    #endif
        disconnect();
        throw Exception(Exception::ERROR_IOCTL);
    } else
        return result;
}

std::streamsize Socket::advanceInputBuffer() {
    if(getInputBufferSize() == 0) // No input buffer
        return 0;
    std::streamsize inAvail;
    if(type == UDP_PEER)
        inAvail = 0;
    else {
        inAvail = egptr()-gptr();
        memmove(eback(), gptr(), inAvail);
    }
    try {
        inAvail += receive(eback()+inAvail, getInputBufferSize()-inAvail);
    } catch(Exception err) {

    }
    setg(eback(), eback(), eback()+inAvail);
    return inAvail;
}

std::streamsize Socket::receive(char_type* buffer, std::streamsize size) {
    if(type == TCP_SERVER)
        throw Exception(Exception::BAD_TYPE);
    if(status != Socket::Status::READY && status != Socket::Status::BUSY)
        return 0;
    size = std::min(size, showmanyc());
    if(size == 0)
        return 0;
    switch(type) {
        case UDP_PEER: {
            struct sockaddr_storage remoteAddr;
            #ifdef WINVER
            int addrSize = sizeof(remoteAddr);
            #else
            unsigned int addrSize = sizeof(remoteAddr);
            #endif
            int result = recvfrom(handle, (char*)buffer, size, 0, reinterpret_cast<struct sockaddr*>(&remoteAddr), &addrSize);
            if(result <= 0) {
                portRemote = 0;
                hostRemote = "";
                throw Exception(Exception::ERROR_READ);
            } else
                readSockaddr(&remoteAddr, hostRemote, portRemote);
            return result;
        }
        case TCP_CLIENT:
        case TCP_SERVERS_CLIENT: {
            int result = recv(handle, (char*)buffer, size, 0);
            if(result <= 0)
                throw Exception(Exception::ERROR_READ);
            return result;
        }
        default:
        case NONE:
        case TCP_SERVER:
            throw Exception(Exception::BAD_TYPE);
    }
}

std::streamsize Socket::send(const char_type* buffer, std::streamsize size) {
    if(type == TCP_SERVER)
        throw Exception(Exception::BAD_TYPE);
    if(status != Socket::Status::READY || size == 0)
        return 0;
    switch(type) {
        case UDP_PEER: {
            AddrinfoContainer info = getSocketInfoFor(hostRemote.c_str(), portRemote, false);
            size_t sentBytes = 0;
            while(sentBytes < (size_t)size) {
                int result = ::sendto(handle, (const char*)buffer + sentBytes, size - sentBytes, 0, info->ai_addr, info->ai_addrlen);
                if(result <= 0) {
                    status = BUSY;
                    throw Exception(Exception::ERROR_SEND);
                }
                sentBytes += result;
            }
            return sentBytes;
        }
        case TCP_CLIENT:
        case TCP_SERVERS_CLIENT: {
            size_t sentBytes = 0;
            while(sentBytes < (size_t)size) {
                int result = ::send(handle, (const char*)buffer + sentBytes, size - sentBytes, 0);
                if(result <= 0) {
                    status = BUSY;
                    throw Exception(Exception::ERROR_SEND);
                }
                sentBytes += result;
            }
            return sentBytes;
        }
        default:
        case NONE:
        case TCP_SERVER:
            throw Exception(Exception::BAD_TYPE);
    }
}

std::streamsize Socket::redirect(const std::vector<std::shared_ptr<Socket>>& destinations) {
    if(type == TCP_SERVER)
        throw Exception(Exception::BAD_TYPE);
    std::streamsize size = 0;
    while(in_avail()) {
        auto length = egptr()-gptr();
        for(const auto& destination : destinations)
            if(destination->sputn(gptr(), length) < length)
                throw Exception(Exception::ERROR_SEND);
        gbump(length);
        size += length;
        advanceInputBuffer();
    }
    return size;
}

std::streamsize Socket::getInputBufferSize() {
    return inputIntermediateSize;
}

std::streamsize Socket::getOutputBufferSize() {
    return epptr()-pbase();
}

void Socket::setInputBufferSize(std::streamsize n) {
    if(eback()) delete[] eback();
    if(type != TCP_SERVER && n > 0) {
        char_type* readBuffer = new char_type[n];
        setg(readBuffer, readBuffer, readBuffer);
        inputIntermediateSize = n;
    } else {
        setg(NULL, NULL, NULL);
        inputIntermediateSize = 0;
    }
}

void Socket::setOutputBufferSize(std::streamsize n) {
    if(pbase()) delete[] pbase();
    if(type != TCP_SERVER && n > 0) {
        char_type* writeBuffer = new char_type[n];
        setp(writeBuffer, writeBuffer+n);
    } else
        setp(NULL, NULL);
}

void Socket::setBlockingMode(bool blocking) {
    #ifdef WINVER
    unsigned long flag = !blocking;
    if(ioctlsocket(handle, FIONBIO, &flag) != 0)
    #else
    int flags = fcntl(handle, F_GETFL);
    if(blocking)
        flags &= ~O_NONBLOCK;
    else
        flags |= O_NONBLOCK;
    if(fcntl(handle, F_SETFL, flags) == -1)
    #endif
        throw Exception(Exception::ERROR_IOCTL);
}

void Socket::setBroadcast(bool active) {
    if(type != UDP_PEER || ipVersion != IPv4)
        throw Exception(Exception::BAD_PROTOCOL);
    #ifdef WINVER
    char flag = 1;
    #else
    int flag = 1;
    #endif
    if(setsockopt(handle, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<const char*>(&flag), sizeof(flag)) == -1)
        throw Exception(Exception::ERROR_SET_SOCK_OPT);
}

void Socket::setMulticastGroup(const std::string& address, bool join) {
    if(type != UDP_PEER)
        throw Exception(Exception::BAD_PROTOCOL);
    if(ipVersion == IPv4) {
        struct in_addr sin;
        if(inet_pton(AF_INET, address.c_str(), &sin) == -1)
            throw Exception(Exception::ERROR_RESOLVING_ADDRESS);
        if((ntohl(sin.s_addr) & 0xF0000000) == 0xE0000000) {
            struct ip_mreq mreq;
            mreq.imr_multiaddr = sin;
            mreq.imr_interface.s_addr = 0;
            if(setsockopt(handle, IPPROTO_IP, (join) ? IP_ADD_MEMBERSHIP : IP_DROP_MEMBERSHIP, reinterpret_cast<const char*>(&mreq), sizeof(mreq)) == -1)
                throw Exception(Exception::ERROR_SET_SOCK_OPT);
        }
    } else {
        struct in6_addr sin;
        if(inet_pton(AF_INET6, address.c_str(), &sin) == -1)
            throw Exception(Exception::ERROR_RESOLVING_ADDRESS);
        if(sin.s6_addr[0] == 0xFF) {
            struct ipv6_mreq mreq;
            mreq.ipv6mr_multiaddr = sin;
            mreq.ipv6mr_interface = 0;
            if(setsockopt(handle, IPPROTO_IPV6, (join) ? IPV6_JOIN_GROUP : IPV6_LEAVE_GROUP, reinterpret_cast<const char*>(&mreq), sizeof(ipv6_mreq)) == -1)
                throw Exception(Exception::ERROR_SET_SOCK_OPT);
        }
    }
}

std::shared_ptr<Socket> Socket::accept() {
    if(type != TCP_SERVER)
        throw Exception(Exception::BAD_TYPE);
    struct sockaddr_storage remoteAddr;
    #ifdef WINVER
    int addrSize = sizeof(remoteAddr);
    #else
    unsigned int addrSize = sizeof(remoteAddr);
    #endif
    int clientHandle = ::accept(handle, reinterpret_cast<struct sockaddr*>(&remoteAddr), &addrSize);
    if(clientHandle == -1) return nullptr;
    std::shared_ptr<Socket> client = SocketFactory();
    client->ipVersion = ipVersion;
    client->type = TCP_SERVERS_CLIENT;
    client->status = READY;
    client->handle = clientHandle;
    client->hostLocal = hostLocal;
    client->portLocal = portLocal;
    readSockaddr(&remoteAddr, client->hostRemote, client->portRemote);
    client->setInputBufferSize(NETLINK_DEFAULT_INPUT_BUFFER_SIZE);
    client->setOutputBufferSize(NETLINK_DEFAULT_OUTPUT_BUFFER_SIZE);
    client->setBlockingMode(false);
    clients.insert(client);
    return client;
}

void Socket::disconnect() {
    if(handle == -1)
        return;
    ipVersion = ANY;
    type = NONE;
    status = NOT_CONNECTED;
    setInputBufferSize(0);
    setOutputBufferSize(0);
    clients.clear();
    closesocket(handle);
    handle = -1;
}

void Socket::disconnectOnError() {
    if(status == NOT_CONNECTED)
        return;
    int error;
    #ifdef WINVER
    int length = sizeof(error);
    getsockopt(handle, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&error), &length);
    #else
    socklen_t length = sizeof(error);
    getsockopt(handle, SOL_SOCKET, SO_ERROR, &error, &length);
    #endif
    if(error != 0)
        disconnect();
}

};
