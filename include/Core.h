/*
    netLink: c++ 11 networking library
    Copyright (C) 2013-2023 Alexander Meißner

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "MsgPack.h"

#ifdef WINVER
#include <SDKDDKVer.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#undef min
#undef max
#else
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif
#include <set>
#include <cmath>

#define NETLINK_DEFAULT_INPUT_BUFFER_SIZE 8192
#define NETLINK_DEFAULT_OUTPUT_BUFFER_SIZE 8192

namespace netLink {

    //! netLink Exceptions
    class Exception {
        public:
        //! Types of errors
        enum Code {
            //System calls
            ERROR_IOCTL = 1,
            ERROR_SELECT = 2,
            ERROR_SET_SOCK_OPT = 3,
            ERROR_GET_SOCK_NAME = 4,
            ERROR_RESOLVING_ADDRESS = 5,

            //Functionality
            ERROR_READ = 6,
            ERROR_SEND = 7,
            ERROR_INIT = 8,

            //Wrong parameters
            BAD_PROTOCOL = 9,
            BAD_TYPE = 10
        } code; //!< Exceptions error code

        //! Initialize from error code
        Exception(Code _code): code(_code) { }
    };

	//! Initialize netLink (only neccessary if windows)
	void init();

};
