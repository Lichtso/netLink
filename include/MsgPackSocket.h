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

#include "Socket.h"

namespace netLink {

    //! Socket with MsgPack abstraction layer
    class MsgPackSocket : public Socket {
        typedef Socket super;
        friend class SocketManager;

        protected:
        std::shared_ptr<Socket> SocketFactory() {
            return std::shared_ptr<Socket>(new MsgPackSocket());
        }

        public:
        std::queue<std::unique_ptr<MsgPack::Element>> queue; //!< Internal queue of elements to be serialized and sent
        MsgPack::Serializer serializer; //!< Internal MsgPack serializer
        MsgPack::Deserializer deserializer; //!< Internal MsgPack deserializer

        MsgPackSocket() :Socket(), serializer(this), deserializer(this) { };

        /*! Pushes one MsgPack::Element in the queue.
         @param element pointer containing the element
         */
        MsgPackSocket& operator<<(std::unique_ptr<MsgPack::Element> element) {
            queue.push(std::move(element));
            return *this;
        }
    };

};
