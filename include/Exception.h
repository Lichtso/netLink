/*
    NetLink Sockets: Networking C++ library
    Copyright 2012 Pedro Francisco Pareja Ruiz (PedroPareja@Gmail.com)
    Modified 2013 Alexander Meißner (lichtso@gamefortec.net)

    This file is part of NetLink Sockets.

    NetLink Sockets is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    NetLink Sockets is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with NetLink Sockets. If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef NL_EXCEPTION
#define NL_EXCEPTION

#include "Core.h"

NL_NAMESPACE

//! netLink Exceptions
class Exception {
    public:
    enum CODE {
        //System calls
        ERROR_IOCTL,
        ERROR_SELECT,
        ERROR_SET_SOCK_OPT,
        ERROR_GET_SOCK_NAME,
        ERROR_RESOLVING_ADDRESS,
        
        //Functionality
        ERROR_READ,
        ERROR_SEND,
        ERROR_INIT,
        STREAM_UNDERFLOW,
        
        //Wrong parameters
        BAD_PROTOCOL,
        BAD_TYPE,
        EXPECTED_NULL
    } code; //!< NetLinks error code
    int nativeErrorCode; //!< Systems error code
    
    Exception(CODE _code, int _nativeErrorCode = 0): code(_code), nativeErrorCode(_nativeErrorCode) {}
};

NL_NAMESPACE_END

#endif
