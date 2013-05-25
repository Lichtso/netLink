/*
    NetLink Sockets: Networking C++ library
    Copyright 2013 Alexander Meißner (lichtso@gamefortec.net)

    This file is part of NetLink Sockets.

    NetLink Sockets is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    NetLink Sockets is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with NetLink Sockets. If not, see <http://www.gnu.org/licenses/>.

*/

#include <set>
#include <vector>
#include <string>
#include <sstream>
#include <functional>

#ifndef MSG_PACK_HEADER
#define MSG_PACK_HEADER

NL_NAMESPACE

enum Type {
    STREAM,
    NIL,
    BOOLEAN,
    FLOAT,
    DOUBLE,
    POSITIVE_FIXNUM,
    NEGATIVE_FIXNUM,
    UINT_8,
    UINT_16,
    UINT_32,
    UINT_64,
    INT_8,
    INT_16,
    INT_32,
    INT_64,
    RAW,
    ARRAY,
    MAP
};

class MsgPackStream {
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
    
    MsgPackStream(std::streambuf* _streamBuffer) :streamBuffer(_streamBuffer) { };
    
    Type getNextType();
    MsgPackStream& operator>>(unsigned char& value);
    MsgPackStream& operator>>(unsigned short& value);
    MsgPackStream& operator>>(unsigned long& value);
    MsgPackStream& operator>>(unsigned long long& value);
    MsgPackStream& operator>>(char& value);
    MsgPackStream& operator>>(short& value);
    MsgPackStream& operator>>(long& value);
    MsgPackStream& operator>>(long long& value);
    MsgPackStream& operator>>(const void* null);
    MsgPackStream& operator>>(bool& value);
    MsgPackStream& operator>>(float& value);
    MsgPackStream& operator>>(double& value);
    MsgPackStream& operator>>(std::string& str);
    unsigned long readArray();
    unsigned long readMap();
    
    MsgPackStream& operator<<(unsigned char& value);
    MsgPackStream& operator<<(unsigned short& value);
    MsgPackStream& operator<<(unsigned long& value);
    MsgPackStream& operator<<(unsigned long long& value);
    MsgPackStream& operator<<(char& value);
    MsgPackStream& operator<<(short& value);
    MsgPackStream& operator<<(long& value);
    MsgPackStream& operator<<(long long& value);
    MsgPackStream& operator<<(const void* null);
    MsgPackStream& operator<<(bool& value);
    MsgPackStream& operator<<(float& value);
    MsgPackStream& operator<<(double& value);
    MsgPackStream& operator<<(const std::string& str);
    MsgPackStream& operator<<(const char* str);
    void writeArray(unsigned long size);
    void writeMap(unsigned long size);
};

NL_NAMESPACE_END

#endif
