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

#include "Element.h"

namespace MsgPack {

    //! MsgPack::Element to represent the header of a dynamical length element
    class Header : public Element {
        friend class Serializer;
        friend class Deserializer;
        protected:
        char header[5]; //!< Internal buffer of the header
        int64_t startSerialize();
        int64_t startDeserialize(uint8_t firstByte);
        std::streamsize serialize(int64_t& pos, std::basic_streambuf<char>* streamBuffer, std::streamsize bytes);
        std::streamsize deserialize(int64_t& pos, std::basic_streambuf<char>* streamBuffer, std::streamsize bytes);
        int64_t getEndPos() const;
        //! Returns the size of the header in bytes
        virtual int64_t getHeaderLength() const = 0;
        public:
        Type getType() const;
        uint32_t getSizeInBytes() const;
        //! Returns the content length in bytes
        virtual uint32_t getLength() const;
    };

};
