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

#include "Header.h"

namespace MsgPack {

    //! MsgPack::Header representing only the header of a array
    class ArrayHeader : public Header {
        friend class Serializer;
        friend class Deserializer;
        protected:
        ArrayHeader() { }
        int64_t getHeaderLength() const;
        public:
        //! Initialize with len elements to follow
        ArrayHeader(uint32_t len);
        std::unique_ptr<Element> copy() const;
        void toJSON(std::ostream& stream) const;
        uint32_t getSizeInBytes() const;
        //! Returns the count of elements in the container
        uint32_t getLength() const;
    };

    //! MsgPack::Header representing only the header of a map
    class MapHeader : public Header {
        friend class Serializer;
        friend class Deserializer;
        protected:
        MapHeader() { }
        int64_t getHeaderLength() const;
        public:
        //! Initialize with len key/value pairs to follow
        MapHeader(uint32_t len);
        std::unique_ptr<Element> copy() const;
        void toJSON(std::ostream& stream) const;
        uint32_t getSizeInBytes() const;
        //! Returns the count of elements in the container
        uint32_t getLength() const;
    };

};
