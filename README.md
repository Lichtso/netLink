netLink
=======

C++ 11 networking library especially made for computer games.

[![Build Status](https://travis-ci.org/Lichtso/netLink.png)](https://travis-ci.org/Lichtso/netLink)

Features:
* C++ 11
* IPv4, IPv6
* Protocols: TCP, UDP
* Enable/Disable blocking mode
* Join/Leave UDP-Multicast groups
* UDP-IPv4-Broadcast
* Operating Systems: Mac OS, Linux, Windows
* Socket can be used as std::streambuf 
* SocketManager calls various events for (dis)connecting, receiving data, connection requests and status changes
* MsgPack v5 support: http://msgpack.org
	* std::streambuf serializer and deserializer
	* hierarchy or token stream
	* push and pull parser