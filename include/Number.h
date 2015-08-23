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
