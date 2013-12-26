netLink
=======

C++ 11 Networking Library

Features:
* C++ 11
* IPv4, IPv6
* Operating Systems: Mac OS, Linux
* Enable/Disable blocking mode
* Join/Leave UDP-Multicast groups
* UDP-IPv4-Broadcast
* Protocols: TCP, UDP
* Socket can be used as std::streambuf 
* SocketManager calls events for connecting, disconnecting, receiving data and write buffer overflow
* MsgPack support: http://msgpack.org
	* std::streambuf serializer and deserializer
	* hierarchy or token stream
	* push and pull parser