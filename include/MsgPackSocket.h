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
