# netLink

C++ 11 KISS principle networking library.

[![Build Status](https://travis-ci.org/Lichtso/netLink.svg)](https://travis-ci.org/Lichtso/netLink)
[![License](https://img.shields.io/badge/License-ZLIB-brightgreen.svg)](http://en.wikipedia.org/wiki/Zlib_License)

## Features:
* C++ 11
* IPv4, IPv6
* Protocols: TCP, UDP
* Enable/Disable blocking mode
* Join/Leave UDP-Multicast groups
* UDP-IPv4-Broadcast
* Operating Systems: Mac OS, Linux, Windows
* MsgPack v5 support: http://msgpack.org so it can communicate with programs running in other programming languages
* Optional: Upgrade std::string with UTF8 support
* Socket can be used as std::streambuf
* SocketManager calls various events for (dis)connecting, receiving data, connection requests and status changes
* Event callbacks: onConnectRequest, onStatusChange, onReceiveRaw, onReceiveMsgPack

## Example Code:
[UDP](https://github.com/Lichtso/netLink/blob/master/src/examples/udp.cpp),
[TCP](https://github.com/Lichtso/netLink/blob/master/src/examples/tcp.cpp)

## Wiki:
[Doxygen online documentation](http://lichtso.github.io/netLink/doc/annotated.html)

[MsgPack tutorial](https://github.com/Lichtso/netLink/wiki/MsgPack)
