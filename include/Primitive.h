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

#include "Element.h"

namespace MsgPack {

    //! MsgPack::Element to represent one byte primitives like booleans and null
    class Primitive : public Element {
        friend class Serializer;
        friend class Deserializer;
        protected:
        uint8_t type; //!< Internal type field
        //! Internal initialize from type
        Primitive(Type type);
        int64_t startDeserialize(uint8_t firstByte);
        std::streamsize serialize(int64_t& pos, std::basic_streambuf<char>* streamBuffer, std::streamsize bytes);
        int64_t getEndPos() const;
        public:
        //! Initialize from boolean
        Primitive(bool value);
        Primitive();
        std::unique_ptr<Element> copy() const;
        void toJSON(std::ostream& stream) const;
        Type getType() const;
        //! Returns if type is NIL
        bool isNull() const {
            return (type == Type::NIL);
        }
        //! Returns the boolean value
        bool getValue() const {
            return (type == Type::BOOL_TRUE);
        }
    };

};
