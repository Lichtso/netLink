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

#include "include/Socket.h"

#ifdef WIN32
#define snprintf _snprintf_s
#else
#define closesocket close
#endif

namespace netLink {

static void readSockaddr(const struct sockaddr* addr, std::string& host, unsigned int& port) {
    char buffer[INET6_ADDRSTRLEN];
    if(addr->sa_family == AF_INET) {
        auto sin = reinterpret_cast<const struct sockaddr_in*>(addr);
        port = ntohs(sin->sin_port);
		inet_ntop(addr->sa_family, (void*)&(sin->sin_addr), buffer, sizeof(buffer));
    }else{
        auto sin = reinterpret_cast<const struct sockaddr_in6*>(addr);
        port = ntohs(sin->sin6_port);
		inet_ntop(addr->sa_family, (void*)&(sin->sin6_addr), buffer, sizeof(buffer));
    }
    host = buffer;
}

Socket::AddrinfoContainer Socket::getSocketInfoFor(const char* host, unsigned int port, bool wildcardAddress) {
    struct addrinfo conf, *res;
    memset(&conf, 0, sizeof(conf));
    conf.ai_flags = AI_V4MAPPED;
    conf.ai_family = AF_UNSPEC;
    
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
			if (off < 0) break;

            if(which & std::ios_base::in) {
				if(eback() + off >= egptr()) break;
                setg(eback(), eback()+off, egptr());
            }

            if(which & std::ios_base::out) {
				if(pbase() + off >= epptr()) break;
                setp(pbase(), epptr());
                pbump(off);
            }
        return off;
        case std::ios_base::cur:
            if(which == std::ios_base::in) {
				if(eback() + off >= egptr() || egptr() + off < eback()) break;
                setg(eback(), gptr()+off, egptr());
                return gptr()-eback();
            }else if(which == std::ios_base::out) {
				if(pbase() + off >= epptr() || epptr() + off < pbase()) break;
                pbump(off);
                return pptr()-pbase();
            }else
				break;
        case std::ios_base::end:
			if(off > 0) break;

            if(which == std::ios_base::in) {
				if(egptr() + off < eback()) break;
                setg(eback(), egptr()+off, egptr());
                return gptr()-eback();
            }else if(which == std::ios_base::out) {
				if(epptr() + off < pbase()) break;
                setp(pbase(), epptr());
                pbump(off);
                return pptr()-pbase();
            }else
                break;
    }
	return EOF;
}

Socket::pos_type Socket::seekpos(Socket::pos_type sp, std::ios_base::openmode which) {
    return Socket::seekoff(sp, std::ios_base::beg, which);
}

int Socket::sync() {
    if(getOutputBufferSize() <= 0) //No output buffer
        return EOF;

    if(pptr() == pbase()) //Allready in sync
        return 0;

    try {
        send(pbase(), pptr()-pbase());
        setp(pbase(), epptr());
        return 0;
    } catch(Exception err) {
        return EOF;
    }
}

std::streamsize Socket::xsgetn(char_type* buffer, std::streamsize size) {
    if(inputIntermediateSize > 0) //Read from input buffer
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
    if(getOutputBufferSize()) //Write into buffer
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
    else{
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
        
        setBlockingMode(blockingConnect);
		#ifdef WIN32
		char flag = 1;
		#else
		int flag = 1;
		#endif
        if(setsockopt(handle, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) == -1)
            throw Exception(Exception::ERROR_SET_SOCK_OPT);
        
        switch(nextAddr->ai_family) {
            case AF_INET:
                ipVersion = IPv4;
            break;
            case AF_INET6:
                ipVersion = IPv6;
            break;
        }
        
        switch(type) {
            case NONE:
            case TCP_SERVERS_CLIENT:
                throw Exception(Exception::BAD_TYPE);
            case TCP_CLIENT:
                if(connect(handle, nextAddr->ai_addr, nextAddr->ai_addrlen) == -1 && blockingConnect) {
					closesocket(handle);
                    handle = -1;
                }else if(blockingConnect)
                    status = READY;
                else
                    status = NOT_CONNECTED;
            break;
            case TCP_SERVER: {
                if(bind(handle, nextAddr->ai_addr, nextAddr->ai_addrlen) == -1) {
					closesocket(handle);
                    handle = -1;
                }
                
                if(listen(handle, status) == -1)
                    throw Exception(Exception::ERROR_INIT);
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
    
    if(handle == -1)
        throw Exception(Exception::ERROR_INIT);

	#ifdef WIN32
    int size = sizeof(nextAddr->ai_addr);
	#else
	unsigned int size = sizeof(nextAddr->ai_addr);
	#endif
    if(getsockname(handle, nextAddr->ai_addr, &size) != 0)
        throw Exception(Exception::ERROR_GET_SOCK_NAME);
    
    readSockaddr(nextAddr->ai_addr, hostLocal, portLocal);
}

void Socket::setMulticastGroup(const struct sockaddr* addr, bool join) {
    if(ipVersion == IPv4) {
        auto sin = reinterpret_cast<const struct sockaddr_in*>(addr)->sin_addr;
        if((ntohl(sin.s_addr) & 0xF0000000) == 0xE0000000) {
            struct ip_mreq mreq;
            mreq.imr_multiaddr = sin;
            mreq.imr_interface.s_addr = 0;

			if(setsockopt(handle, IPPROTO_IP, (join) ? IP_ADD_MEMBERSHIP : IP_DROP_MEMBERSHIP, (const char*)&mreq, sizeof(mreq)) == -1)
				throw Exception(Exception::ERROR_SET_SOCK_OPT);
        }
    }else{
        auto sin = reinterpret_cast<const struct sockaddr_in6*>(addr)->sin6_addr;
        if(sin.s6_addr[0] == 0xFF) {
            struct ipv6_mreq mreq;
            mreq.ipv6mr_multiaddr = sin;
            mreq.ipv6mr_interface = 0;

			if(setsockopt(handle, IPPROTO_IPV6, (join) ? IPV6_JOIN_GROUP : IPV6_LEAVE_GROUP, (const char*)&mreq, sizeof(ipv6_mreq)) == -1)
				throw Exception(Exception::ERROR_SET_SOCK_OPT);
        }
    }
}

Socket::Socket(int _handle, const std::string& _hostLocal, unsigned _portLocal,
               struct sockaddr* remoteAddr, IPVersion _ipVersion)
                :ipVersion(_ipVersion), type(TCP_SERVERS_CLIENT), status(READY),
                handle(_handle), hostLocal(_hostLocal), portLocal(_portLocal) {
    readSockaddr(remoteAddr, hostRemote, portRemote);
    setBlockingMode(false);
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

Socket::~Socket() {
    disconnect();
}

SocketType Socket::getType() const {
    return type;
}

IPVersion Socket::getIPVersion() const {
    return ipVersion;
}

SocketStatus Socket::getStatus() const {
    if(type == TCP_SERVER)
        return (status == NOT_INITIALIZED) ? NOT_INITIALIZED : LISTENING;
    else
        return (SocketStatus)status;
}

std::streamsize Socket::showmanyc() {
	#ifdef WIN32
	unsigned long result = -1;
	if(ioctlsocket(handle, FIONREAD, &result))
	#else
	int result = -1;
	if(ioctl(handle, FIONREAD, &result))
	#endif
		throw Exception(Exception::ERROR_IOCTL);
    else
        return result;
}

std::streamsize Socket::advanceInputBuffer() {
    if(inputIntermediateSize == 0) //No input buffer
        return 0;

    std::streamsize inAvail;
    if(type == UDP_PEER)
        inAvail = 0;
    else{
        inAvail = egptr()-gptr();
        memmove(eback(), gptr(), inAvail);
    }

    try {
        inAvail += receive(eback()+inAvail, inputIntermediateSize-inAvail);
    } catch(Exception err) {

    }

    setg(eback(), eback(), eback()+inAvail);
    return inAvail;
}

std::streamsize Socket::receive(char_type* buffer, std::streamsize size) {
    size = std::min(size, showmanyc());
    if(size == 0) return 0;
    
    switch(type) {
        case UDP_PEER: {
            struct sockaddr remoteAddr;
			#ifdef WIN32
			int addrSize = sizeof(remoteAddr);
			#else
			unsigned int addrSize = sizeof(remoteAddr);
			#endif
            int result = recvfrom(handle, (char*)buffer, size, 0, &remoteAddr, &addrSize);
            
            if(result == -1) {
                portRemote = 0;
                hostRemote = "";
                throw Exception(Exception::ERROR_READ);
            }else
                readSockaddr(&remoteAddr, hostRemote, portRemote);
            
            return result;
        }
        case TCP_CLIENT:
        case TCP_SERVERS_CLIENT: {
            int result = recv(handle, (char*)buffer, size, 0);
            
            if(result == -1)
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
    if(size == 0) return 0;

    switch(type) {
        case UDP_PEER: {
            AddrinfoContainer info = getSocketInfoFor(hostRemote.c_str(), portRemote, false);
            
            size_t sentBytes = 0;
            while(sentBytes < size) {
                int result = ::sendto(handle, (const char*)buffer + sentBytes, size - sentBytes, 0, info->ai_addr, info->ai_addrlen);
                
                if(result == -1)
                    throw Exception(Exception::ERROR_SEND);
                
                sentBytes += result;
            }

            return sentBytes;
        }
        case TCP_CLIENT:
        case TCP_SERVERS_CLIENT: {
            size_t sentBytes = 0;
            while(sentBytes < size) {
                int result = ::send(handle, (const char*)buffer + sentBytes, size - sentBytes, 0);
                
                if(result == -1)
                    throw Exception(Exception::ERROR_SEND);
                
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

std::streamsize Socket::getInputBufferSize() {
    return inputIntermediateSize;
}

std::streamsize Socket::getOutputBufferSize() {
    return epptr()-pbase();
}

void Socket::setInputBufferSize(std::streamsize n) {
    if(eback()) delete [] eback();
    if(n == 0) return;
    if(type == TCP_SERVER)
        throw Exception(Exception::BAD_TYPE);
    
    char_type* readBuffer = new char_type[n];
    setg(readBuffer, readBuffer, readBuffer);
    inputIntermediateSize = n;
}

void Socket::setOutputBufferSize(std::streamsize n) {
    if(pbase()) delete [] pbase();
    if(n == 0) return;
    if(type == TCP_SERVER)
        throw Exception(Exception::BAD_TYPE);
    
    char_type* writeBuffer = new char_type[n];
    setp(writeBuffer, writeBuffer+n);
}

void Socket::setBlockingMode(bool blocking) {
	#ifdef WIN32
	unsigned long flag = !blocking;
	if(ioctlsocket(handle, FIONBIO, &flag) == -1)
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

	#ifdef WIN32
	char flag = 1;
	#else
	int flag = 1;
	#endif
    if(setsockopt(handle, SOL_SOCKET, SO_BROADCAST, &flag, sizeof(flag)) == -1)
        throw Exception(Exception::ERROR_SET_SOCK_OPT);
}

void Socket::setMulticastGroup(const std::string& address, bool join) {
    if(type != UDP_PEER)
        throw Exception(Exception::BAD_PROTOCOL);
    
    struct sockaddr_storage addr;
    if(ipVersion == IPv4) {
        auto sin = reinterpret_cast<struct sockaddr_in*>(&addr);
        if(inet_pton(AF_INET, address.c_str(), &sin->sin_addr) == -1)
            throw Exception(Exception::ERROR_RESOLVING_ADDRESS);
    }else{
        auto sin = reinterpret_cast<struct sockaddr_in6*>(&addr);
        if(inet_pton(AF_INET6, address.c_str(), &sin->sin6_addr) == -1)
            throw Exception(Exception::ERROR_RESOLVING_ADDRESS);
    }

    setMulticastGroup(reinterpret_cast<struct sockaddr*>(&addr), join);
}

std::unique_ptr<Socket> Socket::accept() {
    if(type != TCP_SERVER)
        throw Exception(Exception::BAD_TYPE);
    
    struct sockaddr remoteAddr;
	#ifdef WIN32
	int addrSize = sizeof(remoteAddr);
	#else
	unsigned int addrSize = sizeof(remoteAddr);
	#endif
    
    int newHandler = ::accept(handle, &remoteAddr, &addrSize);
    if(newHandler == -1) return nullptr;
    
    return std::unique_ptr<Socket>(new Socket(newHandler, hostLocal, portLocal, &remoteAddr, ipVersion));
}

void Socket::disconnect() {
    setInputBufferSize(0);
    setOutputBufferSize(0);
    if(handle == -1) return;
	closesocket(handle);
    handle = -1;
    status = NOT_CONNECTED;
}

};