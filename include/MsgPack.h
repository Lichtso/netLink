/*
    netLink: c++ 11 networking library
    Copyright 2013 Alexander Meißner (lichtso@gamefortec.net)

    This software is provided 'as-is', without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the use of this software.
    Permission is granted to anyone to use this software for any purpose, 
    including commercial applications, and to alter it and redistribute it freely, 
    subject to the following restrictions:
    
    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
*/

#include <set>
#include <vector>
#include <string>
#include <sstream>
#include <functional>

#ifndef netLink_MsgPack
#define netLink_MsgPack

namespace netLink {
    namespace MsgPack {
        enum Type {
            MAP,
            ARRAY,
            RAW,
            STRING,
            EXTENDED,
            NIL,
            BOOLEAN,
            FLOAT,
            DOUBLE,
            UINT,
            INT
        };
        
        class Stream {
            uint8_t getNextByte();
            void checkBytesInAvail(unsigned int size);
            void checkBytesInAvail(unsigned int size, unsigned int unget);
            void readByte1(void* storage);
            void readByte2(void* storage);
            void readByte4(void* storage);
            void readByte8(void* storage);
            void writeByte2(void* storage);
            void writeByte4(void* storage);
            void writeByte8(void* storage);
            
            public:
            std::streambuf* streamBuffer;
            
            Stream(std::streambuf* _streamBuffer) :streamBuffer(_streamBuffer) { };
            
            Type getNextType();
            Stream& operator>>(uint8_t& value);
            Stream& operator>>(uint16_t& value);
            Stream& operator>>(uint32_t& value);
            Stream& operator>>(uint64_t& value);
            Stream& operator>>(int8_t& value);
            Stream& operator>>(int16_t& value);
            Stream& operator>>(int32_t& value);
            Stream& operator>>(int64_t& value);
            Stream& operator>>(const void* null);
            Stream& operator>>(bool& value);
            Stream& operator>>(float& value);
            Stream& operator>>(double& value);
            Stream& operator>>(std::string& str);
            std::unique_ptr<char[]> readRaw(uint32_t& size);
            std::unique_ptr<char[]> readExtended(int8_t& type, uint32_t& size);
            uint32_t readArray();
            uint32_t readMap();
            
            Stream& operator<<(uint8_t value);
            Stream& operator<<(uint16_t value);
            Stream& operator<<(uint32_t value);
            Stream& operator<<(uint64_t value);
            Stream& operator<<(int8_t value);
            Stream& operator<<(int16_t value);
            Stream& operator<<(int32_t value);
            Stream& operator<<(int64_t value);
            Stream& operator<<(const void* null);
            Stream& operator<<(bool value);
            Stream& operator<<(float value);
            Stream& operator<<(double value);
            Stream& operator<<(const std::string& str);
            Stream& operator<<(const char* str);
            void writeRaw(const char* data, uint32_t size);
            void writeExtended(int8_t type, const char* data, uint32_t size);
            void writeArray(uint32_t size);
            void writeMap(uint32_t size);
        };
    };
};

#endif
