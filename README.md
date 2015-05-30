# netLink

C++ 11 KISS principle networking library.

[![Build Status](https://travis-ci.org/Lichtso/netLink.svg)](https://travis-ci.org/Lichtso/netLink)
[![License](https://img.shields.io/badge/License-ZLIB-brightgreen.svg)](http://en.wikipedia.org/wiki/Zlib_License)

## Documentation:
[See wiki pages](https://github.com/Lichtso/netLink/wiki)

## Features:
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
* Optional: Upgrade std::string with UTF8 support
