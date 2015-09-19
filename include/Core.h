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

#pragma once

#include "MsgPack.h"

#ifdef WIN32
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
