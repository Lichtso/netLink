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
