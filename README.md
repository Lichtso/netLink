# netLink

C++ 11 KISS principle networking library.

[![License](https://img.shields.io/badge/License-GPLv3-brightgreen.svg)](https://www.gnu.org/licenses/gpl-3.0.en.html)
[![CI](https://github.com/lichtso/netLink/actions/workflows/actions.yml/badge.svg)](https://github.com/Lichtso/netLink/actions/workflows/actions.yml)

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

## History:
This library was inspired by the ["NetLink Sockets C++ Library"](https://sourceforge.net/projects/netlinksockets/files/release/1.0.0-pre-3/) from [Pedro Francisco Pareja Ruiz](https://github.com/PedroPareja), from which it got its general topic and name.
