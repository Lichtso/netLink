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

#include "include/Exception.h"

NL_NAMESPACE

unsigned char MsgPackStream::getNextByte() {
    int value = streamBuffer->sgetc();
    if(value < 0) //EOF
        throw Exception(Exception::STREAM_UNDERFLOW);
    return value;
}

void MsgPackStream::checkBytesInAvail(unsigned int size) {
    if(streamBuffer->in_avail() < size)
        throw Exception(Exception::STREAM_UNDERFLOW);
}

void MsgPackStream::checkBytesInAvail(unsigned int size, unsigned int unget) {
    if(streamBuffer->in_avail() >= size) return;
    for(unsigned int i = 0; i < unget; i ++)
        if(streamBuffer->sungetc() < 0)
            break;
    throw Exception(Exception::STREAM_UNDERFLOW);
}

void MsgPackStream::readByte1(void* storage) {
    checkBytesInAvail(1);
    streamBuffer->sbumpc();
    *reinterpret_cast<unsigned char*>(storage) = streamBuffer->sbumpc();
}

void MsgPackStream::readByte2(void* _storage) {
    checkBytesInAvail(2);
    streamBuffer->sbumpc();
    unsigned char* storage = reinterpret_cast<unsigned char*>(_storage);
    #if LITTLE_ENDIAN
    storage[0] = streamBuffer->sbumpc();
    storage[1] = streamBuffer->sbumpc();
    #else
    storage[1] = streamBuffer->sbumpc();
    storage[0] = streamBuffer->sbumpc();
    #endif
}

void MsgPackStream::readByte4(void* _storage) {
    checkBytesInAvail(4);
    streamBuffer->sbumpc();
    unsigned char* storage = reinterpret_cast<unsigned char*>(_storage);
    #if LITTLE_ENDIAN
    storage[0] = streamBuffer->sbumpc();
    storage[1] = streamBuffer->sbumpc();
    storage[2] = streamBuffer->sbumpc();
    storage[3] = streamBuffer->sbumpc();
    #else
    storage[3] = streamBuffer->sbumpc();
    storage[2] = streamBuffer->sbumpc();
    storage[1] = streamBuffer->sbumpc();
    storage[0] = streamBuffer->sbumpc();
    #endif
}

void MsgPackStream::readByte8(void* _storage) {
    checkBytesInAvail(8);
    streamBuffer->sbumpc();
    unsigned char* storage = reinterpret_cast<unsigned char*>(_storage);
    #if LITTLE_ENDIAN
    storage[0] = streamBuffer->sbumpc();
    storage[1] = streamBuffer->sbumpc();
    storage[2] = streamBuffer->sbumpc();
    storage[3] = streamBuffer->sbumpc();
    storage[4] = streamBuffer->sbumpc();
    storage[5] = streamBuffer->sbumpc();
    storage[6] = streamBuffer->sbumpc();
    storage[7] = streamBuffer->sbumpc();
    #else
    storage[7] = streamBuffer->sbumpc();
    storage[6] = streamBuffer->sbumpc();
    storage[5] = streamBuffer->sbumpc();
    storage[4] = streamBuffer->sbumpc();
    storage[3] = streamBuffer->sbumpc();
    storage[2] = streamBuffer->sbumpc();
    storage[1] = streamBuffer->sbumpc();
    storage[0] = streamBuffer->sbumpc();
    #endif
}

void MsgPackStream::writeByte2(void* _storage) {
    unsigned char* storage = reinterpret_cast<unsigned char*>(_storage);
    #if LITTLE_ENDIAN
    streamBuffer->sputc(storage[0]);
    streamBuffer->sputc(storage[1]);
    #else
    streamBuffer->sputc(storage[1]);
    streamBuffer->sputc(storage[0]);
    #endif
}

void MsgPackStream::writeByte4(void* _storage) {
    unsigned char* storage = reinterpret_cast<unsigned char*>(_storage);
    #if LITTLE_ENDIAN
    streamBuffer->sputc(storage[0]);
    streamBuffer->sputc(storage[1]);
    streamBuffer->sputc(storage[2]);
    streamBuffer->sputc(storage[3]);
    #else
    streamBuffer->sputc(storage[3]);
    streamBuffer->sputc(storage[2]);
    streamBuffer->sputc(storage[1]);
    streamBuffer->sputc(storage[0]);
    #endif
}

void MsgPackStream::writeByte8(void* _storage) {
    unsigned char* storage = reinterpret_cast<unsigned char*>(_storage);
    #if LITTLE_ENDIAN
    streamBuffer->sputc(storage[0]);
    streamBuffer->sputc(storage[1]);
    streamBuffer->sputc(storage[2]);
    streamBuffer->sputc(storage[3]);
    streamBuffer->sputc(storage[4]);
    streamBuffer->sputc(storage[5]);
    streamBuffer->sputc(storage[6]);
    streamBuffer->sputc(storage[7]);
    #else
    streamBuffer->sputc(storage[7]);
    streamBuffer->sputc(storage[6]);
    streamBuffer->sputc(storage[5]);
    streamBuffer->sputc(storage[4]);
    streamBuffer->sputc(storage[3]);
    streamBuffer->sputc(storage[2]);
    streamBuffer->sputc(storage[1]);
    streamBuffer->sputc(storage[0]);
    #endif
}



Type MsgPackStream::getNextType() {
    unsigned char data = getNextByte();
    
    if((data & 0x80) == 0)
        return POSITIVE_FIXNUM;
    
    switch(data) {
        case 0xC0:
            return NIL;
        case 0xC2:
        case 0xC3:
            return BOOLEAN;
        case 0xCA:
            return FLOAT;
        case 0xCB:
            return DOUBLE;
        case 0xCC:
            return UINT_8;
        case 0xCD:
            return UINT_16;
        case 0xCE:
            return UINT_32;
        case 0xCF:
            return UINT_64;
        case 0xD0:
            return INT_8;
        case 0xD1:
            return INT_16;
        case 0xD2:
            return INT_32;
        case 0xD3:
            return INT_64;
        case 0xDA:
        case 0xDB:
            return RAW;
        case 0xDC:
        case 0xDD:
            return ARRAY;
        case 0xDE:
        case 0xDF:
            return MAP;
    }
    
    switch(data & 0xE0) {
        case 0xA0:
            return RAW;
        case 0xE0:
            return NEGATIVE_FIXNUM;
    }
    
    switch(data & 0xF0) {
        case 0x80:
            return MAP;
        case 0x90:
            return ARRAY;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

MsgPackStream& MsgPackStream::operator>>(unsigned char& value) {
    unsigned char data = getNextByte();
    
    if(data == 0xCC) {
        readByte1(&value);
        return *this;
    }
    
    if((data & 0x80) == 0) {
        streamBuffer->sbumpc();
        value = data;
        return *this;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

MsgPackStream& MsgPackStream::operator>>(unsigned short& value) {
    unsigned char data = getNextByte();
    
    if(data == 0xCD) {
        readByte2(&value);
        return *this;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

MsgPackStream& MsgPackStream::operator>>(unsigned long& value) {
    unsigned char data = getNextByte();
    
    if(data == 0xCE) {
        readByte4(&value);
        return *this;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

MsgPackStream& MsgPackStream::operator>>(unsigned long long& value) {
    unsigned char data = getNextByte();
    
    if(data == 0xCF) {
        readByte8(&value);
        return *this;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

MsgPackStream& MsgPackStream::operator>>(char& value) {
    unsigned char data = getNextByte();
    
    if(data == 0xD0) {
        readByte1(&value);
        return *this;
    }
    
    if((data & 0x80) == 0) {
        streamBuffer->sbumpc();
        value = (char)data;
        return *this;
    }
    
    if((data & 0xE0) == 0xE0) {
        streamBuffer->sbumpc();
        value = -((char)data & 0x1F)-1;
        return *this;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

MsgPackStream& MsgPackStream::operator>>(short& value) {
    unsigned char data = getNextByte();
    
    if(data == 0xD2) {
        readByte2(&value);
        return *this;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

MsgPackStream& MsgPackStream::operator>>(long& value) {
    unsigned char data = getNextByte();
    
    if(data == 0xD2) {
        readByte4(&value);
        return *this;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

MsgPackStream& MsgPackStream::operator>>(long long& value) {
    unsigned char data = getNextByte();
    
    if(data == 0xD3) {
        readByte8(&value);
        return *this;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

MsgPackStream& MsgPackStream::operator>>(const void* null) {
    if(null != NULL)
        throw Exception(Exception::EXPECTED_NULL);
    
    unsigned char data = getNextByte();
    
    if(data == 0xC0) {
        streamBuffer->sbumpc();
        return *this;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

MsgPackStream& MsgPackStream::operator>>(bool& value) {
    unsigned char data = getNextByte();
    
    switch(data) {
        case 0xC2:
            streamBuffer->sbumpc();
            value = false;
            return *this;
        case 0xC3:
            streamBuffer->sbumpc();
            value = true;
            return *this;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

MsgPackStream& MsgPackStream::operator>>(float& value) {
    unsigned char data = getNextByte();
    
    if(data == 0xCA) {
        float storage;
        readByte4(&storage);
        value = storage;
        return *this;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

MsgPackStream& MsgPackStream::operator>>(double& value) {
    unsigned char data = getNextByte();
    
    if(data == 0xCB) {
        readByte8(&value);
        return *this;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

MsgPackStream& MsgPackStream::operator>>(std::string& str) {
    unsigned char data = getNextByte();
    
    if((data & 0xE0) == 0xA0) {
        unsigned char size = data & 0x1F;
        checkBytesInAvail(size);
        streamBuffer->sbumpc();
        char buffer[size];
        streamBuffer->sgetn(buffer, size);
        str = std::string(buffer, size);
        return *this;
    }
    
    switch(data) {
        case 0xCD: {
            unsigned short size;
            readByte2(&size);
            checkBytesInAvail(size, 2);
            char buffer[size];
            streamBuffer->sgetn(buffer, size);
            str = std::string(buffer, size);
            return *this;
        } case 0xCE: {
            streamBuffer->sbumpc();
            unsigned long size;
            readByte4(&size);
            checkBytesInAvail(size, 4);
            char buffer[size];
            streamBuffer->sgetn(buffer, size);
            str = std::string(buffer, size);
            return *this;
        }
    }
    
    throw Exception(Exception::BAD_TYPE);
}

unsigned long MsgPackStream::readArray() {
    unsigned char data = getNextByte();
    
    if((data & 0xF0) == 0x90) {
        streamBuffer->sbumpc();
        return data & 0x0F;
    }
    
    switch(data) {
        case 0xDC: {
            unsigned short size;
            readByte2(&size);
            return size;
        } case 0xDD: {
            unsigned long size;
            readByte4(&size);
            return size;
        }
    }
    
    throw Exception(Exception::BAD_TYPE);
}

unsigned long MsgPackStream::readMap() {
    unsigned char data = getNextByte();
    
    if((data & 0xF0) == 0x80) {
        streamBuffer->sbumpc();
        return data & 0x0F;
    }
    
    switch(data) {
        case 0xDE: {
            unsigned short size;
            readByte2(&size);
            return size;
        } case 0xDF: {
            unsigned short size;
            readByte4(&size);
            return size;
        }
    }
    
    throw Exception(Exception::BAD_TYPE);
}



MsgPackStream& MsgPackStream::operator<<(unsigned char& value) {
    if(value < 128)
        streamBuffer->sputc(0x80 | value);
    else{
        streamBuffer->sputc(0xCC);
        streamBuffer->sputc(value);
    }
    
    return *this;
}

MsgPackStream& MsgPackStream::operator<<(unsigned short& value) {
    streamBuffer->sputc(0xCD);
    writeByte2(&value);
    return *this;
}

MsgPackStream& MsgPackStream::operator<<(unsigned long& value) {
    streamBuffer->sputc(0xCE);
    writeByte4(&value);
    return *this;
}

MsgPackStream& MsgPackStream::operator<<(unsigned long long& value) {
    streamBuffer->sputc(0xCF);
    writeByte8(&value);
    return *this;
}

MsgPackStream& MsgPackStream::operator<<(char& value) {
    if(value >= -32 && value < 0)
        streamBuffer->sputc(0xE0 | (-1-value));
    else if(value >= 0)
        streamBuffer->sputc(0x80 | value);
    else{
        streamBuffer->sputc(0xD0);
        streamBuffer->sputc(value);
    }
    
    return *this;
}

MsgPackStream& MsgPackStream::operator<<(short& value) {
    streamBuffer->sputc(0xD1);
    writeByte2(&value);
    return *this;
}

MsgPackStream& MsgPackStream::operator<<(long& value) {
    streamBuffer->sputc(0xD2);
    writeByte4(&value);
    return *this;
}

MsgPackStream& MsgPackStream::operator<<(long long& value) {
    streamBuffer->sputc(0xD3);
    writeByte8(&value);
    return *this;
}

MsgPackStream& MsgPackStream::operator<<(const void* null) {
    if(null != NULL)
        throw Exception(Exception::EXPECTED_NULL);
    
    streamBuffer->sputc(0xC0);
    
    return *this;
}

MsgPackStream& MsgPackStream::operator<<(bool& value) {
    streamBuffer->sputc((value) ? 0xC3 : 0xC2);
    return *this;
}

MsgPackStream& MsgPackStream::operator<<(float& value) {
    streamBuffer->sputc(0xCA);
    writeByte4(&value);
    return *this;
}

MsgPackStream& MsgPackStream::operator<<(double& value) {
    streamBuffer->sputc(0xCB);
    writeByte8(&value);
    return *this;
}

MsgPackStream& MsgPackStream::operator<<(const std::string& str) {
    if(str.size() < 32) {
        streamBuffer->sputc((unsigned char)str.size() | 0xA0);
        streamBuffer->sputn(str.c_str(), str.size());
    }else if(str.size() < 65536) {
        streamBuffer->sputc(0xDA);
        unsigned short size = str.size();
        writeByte2(&size);
        streamBuffer->sputn(str.c_str(), str.size());
    }else{
        streamBuffer->sputc(0xDB);
        unsigned long size = str.size();
        writeByte4(&size);
        streamBuffer->sputn(str.c_str(), str.size());
    }
    
    return *this;
}

MsgPackStream& MsgPackStream::operator<<(const char* str) {
    return *this << std::string(str);
}

void MsgPackStream::writeArray(unsigned long size) {
    if(size < 16)
        streamBuffer->sputc((unsigned char)size | 0x90);
    else if(size < 65536) {
        streamBuffer->sputc(0xDC);
        unsigned short _size = size;
        writeByte2(&_size);
    }else{
        streamBuffer->sputc(0xDD);
        writeByte4(&size);
    }
}

void MsgPackStream::writeMap(unsigned long size) {
    if(size < 16)
        streamBuffer->sputc((unsigned char)size | 0x80);
    else if(size < 65536) {
        streamBuffer->sputc(0xDE);
        unsigned short _size = size;
        writeByte2(&_size);
    }else{
        streamBuffer->sputc(0xDF);
        writeByte4(&size);
    }
}

NL_NAMESPACE_END