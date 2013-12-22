/*
    netLink: C++11 networking library
    Copyright 2013 Alexander Meißner (lichtso@gamefortec.net)
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
            FLOAT_32,
            FLOAT_64,
            UINT_8,
            UINT_16,
            UINT_32,
            UINT_64,
            INT_8,
            INT_16,
            INT_32,
            INT_64
        };
        
        class Stream {
            unsigned char getNextByte();
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
            Stream& operator>>(unsigned char& value);
            Stream& operator>>(unsigned short& value);
            Stream& operator>>(unsigned long& value);
            Stream& operator>>(unsigned long long& value);
            Stream& operator>>(char& value);
            Stream& operator>>(short& value);
            Stream& operator>>(long& value);
            Stream& operator>>(long long& value);
            Stream& operator>>(const void* null);
            Stream& operator>>(bool& value);
            Stream& operator>>(float& value);
            Stream& operator>>(double& value);
            Stream& operator>>(std::string& str);
            unsigned long readArray();
            unsigned long readMap();
            
            Stream& operator<<(unsigned char value);
            Stream& operator<<(unsigned short value);
            Stream& operator<<(unsigned long value);
            Stream& operator<<(unsigned long long value);
            Stream& operator<<(char value);
            Stream& operator<<(short value);
            Stream& operator<<(long value);
            Stream& operator<<(long long value);
            Stream& operator<<(const void* null);
            Stream& operator<<(bool value);
            Stream& operator<<(float value);
            Stream& operator<<(double value);
            Stream& operator<<(const std::string& str);
            Stream& operator<<(const char* str);
            void writeArray(unsigned long size);
            void writeMap(unsigned long size);
        };
    };
};

#endif
