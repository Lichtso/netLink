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
