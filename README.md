netLink
=======

C++ 11 Networking Library

[![Build Status](https://travis-ci.org/Lichtso/netLink.png)](https://travis-ci.org/Lichtso/netLink)

Features:
* C++ 11
* IPv4, IPv6
* Protocols: TCP, UDP
* Enable/Disable blocking mode
* Join/Leave UDP-Multicast groups
* UDP-IPv4-Broadcast
* Operating Systems: Mac OS, Linux
* Socket can be used as std::streambuf 
* SocketManager calls events for connecting, disconnecting, receiving data and write buffer overflow
* MsgPack support: http://msgpack.org
	* std::streambuf serializer and deserializer
	* hierarchy or token stream
	* push and pull parser