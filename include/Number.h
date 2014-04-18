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
        friend Serializer;
        friend Deserializer;
        protected:
        uint8_t data[9];
        Number() { }
        int64_t startDeserialize(std::basic_streambuf<uint8_t>* streamBuffer);
        std::streamsize serialize(int64_t& pos, std::basic_streambuf<uint8_t>* streamBuffer, std::streamsize bytes);
        std::streamsize deserialize(int64_t& pos, std::basic_streambuf<uint8_t>* streamBuffer, std::streamsize bytes);
        int64_t getEndPos() const;
        public:
        Number(uint64_t value);
        Number(int64_t value);
        Number(float value);
        Number(double value);
        void stringify(std::ostream& stream) const;
        Type getType() const;
        //! Returns the value as given data type T
        template<class T> T getValue() const {
            if(data[0] < 0x80 || data[0] >= 0xE0)
                return (T)reinterpret_cast<const int8_t&>(data[0]);
            else
                switch(data[0]) {
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