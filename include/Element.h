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

#include <map>
#include <queue>
#include <memory>
#include <sstream>
#include <functional>
#include <climits>
#include <cstring>
#include <cstdint>

//Store numbers in network endian (big endian)
void storeUint8(char* target, uint8_t source);
void storeInt8(char* target, int8_t source);
void storeUint16(char* target, uint16_t source);
void storeInt16(char* target, int16_t source);
void storeFloat32(char* target, float source);
void storeUint32(char* target, uint32_t source);
void storeInt32(char* target, int32_t source);
void storeFloat64(char* target, double source);
void storeUint64(char* target, uint64_t source);
void storeInt64(char* target, int64_t source);

//Read numbers from network endian (big endian)
uint8_t loadUint8(const char* source);
int8_t loadInt8(const char* source);
uint16_t loadUint16(const char* source);
int16_t loadInt16(const char* source);
float loadFloat32(const char* source);
uint32_t loadUint32(const char* source);
int32_t loadInt32(const char* source);
double loadFloat64(const char* source);
uint64_t loadUint64(const char* source);
int64_t loadInt64(const char* source);

namespace MsgPack {

    class Serializer;
    class Deserializer;

    enum Type {
        FIXUINT = 0x00,
        FIXMAP = 0x80,
        FIXARRAY = 0x90,
        FIXSTR = 0xA0,
        NIL = 0xC0,
        UNDEFINED = 0xC1,
        BOOL_FALSE = 0xC2,
        BOOL_TRUE = 0xC3,
        BIN_8 = 0xC4,
        BIN_16 = 0xC5,
        BIN_32 = 0xC6,
        EXT_8 = 0xC7,
        EXT_16 = 0xC8,
        EXT_32 = 0xC9,
        FLOAT_32 = 0xCA,
        FLOAT_64 = 0xCB,
        UINT_8 = 0xCC,
        UINT_16 = 0xCD,
        UINT_32 = 0xCE,
        UINT_64 = 0xCF,
        INT_8 = 0xD0,
        INT_16 = 0xD1,
        INT_32 = 0xD2,
        INT_64 = 0xD3,
        FIXEXT_8 = 0xD4,
        FIXEXT_16 = 0xD5,
        FIXEXT_32 = 0xD6,
        FIXEXT_64 = 0xD7,
        FIXEXT_128 = 0xD8,
        STR_8 = 0xD9,
        STR_16 = 0xDA,
        STR_32 = 0xDB,
        ARRAY_16 = 0xDC,
        ARRAY_32 = 0xDD,
        MAP_16 = 0xDE,
        MAP_32 = 0xDF,
        FIXINT = 0xE0
    };

    //! Abstract class to represent one element in a MsgPack stream
    class Element {
        friend class Serializer;
        friend class Deserializer;
        protected:
        Element() { }
        //! Returns the initial serializer position in bytes (negative value if there is a header)
        virtual int64_t startSerialize() { return 0; };
        //! Reads in one byte and returns the initial deserializer position in bytes (negative value if there is a header)
        virtual int64_t startDeserialize(uint8_t firstByte) = 0;
        //! Serializes bytes at the given serializer position pos into streamBuffer
        virtual std::streamsize serialize(int64_t& pos, std::basic_streambuf<char>* streamBuffer, std::streamsize bytes) = 0;
        //! Deserializes bytes at the given deserializer position pos from streamBuffer
        virtual std::streamsize deserialize(int64_t& pos, std::basic_streambuf<char>* streamBuffer, std::streamsize bytes) { return 0; };
        //! Returns true if the header of a container is deserialized and reserves the necessary space for its element vector
        virtual bool containerDeserialized() { return false; };
        //! Returns a raw pointer to the element vector of a container
        virtual std::vector<std::unique_ptr<Element>>* getElementsVector() { return NULL; };
        //! Returns the first invalid (de)serializer position
        virtual int64_t getEndPos() const = 0;
        public:
        virtual ~Element() { }
        //! Creates a deep copy of this element
        virtual std::unique_ptr<Element> copy() const = 0;
        //! Writes a human readable JSON-like string into the given stream
        virtual void toJSON(std::ostream& stream) const = 0;
        //! Returns the MsgPack::Type
        virtual Type getType() const = 0;
        //! Returns the size in bytes this MsgPack::Element takes if completely serialized
        virtual uint32_t getSizeInBytes() const { return getEndPos(); };
    };

};
