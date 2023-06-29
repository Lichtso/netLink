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

    //! MsgPack::Element for all types of numbers
    class Number : public Element {
        friend class Serializer;
        friend class Deserializer;
        protected:
        char data[9]; //!< Internal raw represenation of the number
        Number() { }
        int64_t startDeserialize(uint8_t firstByte);
        std::streamsize serialize(int64_t& pos, std::basic_streambuf<char>* streamBuffer, std::streamsize bytes);
        std::streamsize deserialize(int64_t& pos, std::basic_streambuf<char>* streamBuffer, std::streamsize bytes);
        int64_t getEndPos() const;
        public:
        //! Initialize from unsigned 64 bit integer
        Number(uint64_t value);
        //! Initialize from signed 64 bit integer
        Number(int64_t value);
        //! Initialize from 32 bit float
        Number(float value);
        //! Initialize from 64 bit float
        Number(double value);
        std::unique_ptr<Element> copy() const;
        void toJSON(std::ostream& stream) const;
        Type getType() const;
        //! Returns true if getType() is one of FIXUINT, UINT_8, UINT_16, UINT_32, UINT_64
        bool isUnsignedInteger() const;
        //! Returns true if getType() is one of FIXINT, INT_8, INT_16, INT_32, INT_64
        bool isSignedInteger() const;
        //! Returns true if getType() is one of FLOAT_32, FLOAT_64
        bool isFloatingPoint() const;
        //! Returns the value as given data type T
        template<class T> T getValue() const {
            uint8_t type = static_cast<const uint8_t>(data[0]);
            if(type < FIXMAP)
                return (T)type;
            else if(type >= FIXINT)
                return (T)static_cast<const int8_t>(type);
            else
                switch(type) {
                    case Type::FLOAT_32:
                        return (T)loadFloat32(data+1);
                    case Type::FLOAT_64:
                        return (T)loadFloat64(data+1);
                    case Type::UINT_8:
                        return (T)loadUint8(data+1);
                    case Type::UINT_16:
                        return (T)loadUint16(data+1);
                    case Type::UINT_32:
                        return (T)loadUint32(data+1);
                    case Type::UINT_64:
                        return (T)loadUint64(data+1);
                    case Type::INT_8:
                        return (T)loadInt8(data+1);
                    case Type::INT_16:
                        return (T)loadInt16(data+1);
                    case Type::INT_32:
                        return (T)loadInt32(data+1);
                    case Type::INT_64:
                        return (T)loadInt64(data+1);
                }
            return (T)0;
        }
    };

};
