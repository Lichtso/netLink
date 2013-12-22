/*
    netLink: C++11 networking library
    Copyright 2013 Alexander Meißner (lichtso@gamefortec.net)
*/

#include "include/Core.h"

namespace netLink {

unsigned char MsgPack::Stream::getNextByte() {
    int value = streamBuffer->sgetc();
    if(value < 0) //EOF
        throw Exception(Exception::STREAM_UNDERFLOW);
    return value;
}

void MsgPack::Stream::checkBytesInAvail(unsigned int size) {
    if(streamBuffer->in_avail() < size)
        throw Exception(Exception::STREAM_UNDERFLOW);
}

void MsgPack::Stream::checkBytesInAvail(unsigned int size, unsigned int unget) {
    if(streamBuffer->in_avail() >= size) return;
    for(unsigned int i = 0; i < unget; i ++)
        if(streamBuffer->sungetc() < 0)
            break;
    throw Exception(Exception::STREAM_UNDERFLOW);
}

void MsgPack::Stream::readByte1(void* storage) {
    checkBytesInAvail(1);
    streamBuffer->sbumpc();
    *reinterpret_cast<unsigned char*>(storage) = streamBuffer->sbumpc();
}

void MsgPack::Stream::readByte2(void* _storage) {
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

void MsgPack::Stream::readByte4(void* _storage) {
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

void MsgPack::Stream::readByte8(void* _storage) {
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

void MsgPack::Stream::writeByte2(void* _storage) {
    unsigned char* storage = reinterpret_cast<unsigned char*>(_storage);
    #if LITTLE_ENDIAN
    streamBuffer->sputc(storage[0]);
    streamBuffer->sputc(storage[1]);
    #else
    streamBuffer->sputc(storage[1]);
    streamBuffer->sputc(storage[0]);
    #endif
}

void MsgPack::Stream::writeByte4(void* _storage) {
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

void MsgPack::Stream::writeByte8(void* _storage) {
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



MsgPack::Type MsgPack::Stream::getNextType() {
    unsigned char data = getNextByte();
    
    if((data & 0x80) == 0)
        return UINT_8;

    switch(data & 0xF0) {
        case 0x80:
            return MAP;
        case 0x90:
            return ARRAY;
    }

    switch(data & 0xE0) {
        case 0xA0:
            return RAW;
        case 0xE0:
            return INT_8;
    }
    
    switch(data) {
        case 0xC0:
            return NIL;
        //case 0xC1: BAD_TYPE
        case 0xC2:
        case 0xC3:
            return BOOLEAN;
        case 0xC4:
        case 0xC5:
        case 0xC6:
            return RAW;
        case 0xC7:
        case 0xC8:
        case 0xC9:
            return EXTENDED;
        case 0xCA:
            return FLOAT_32;
        case 0xCB:
            return FLOAT_64;
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
        case 0xD4:
        case 0xD5:
        case 0xD6:
        case 0xD7:
        case 0xD8:
            return EXTENDED;
        case 0xD9:
        case 0xDA:
        case 0xDB:
            return STRING;
        case 0xDC:
        case 0xDD:
            return ARRAY;
        case 0xDE:
        case 0xDF:
            return MAP;
    }

    throw Exception(Exception::BAD_TYPE);
}

MsgPack::Stream& MsgPack::Stream::operator>>(unsigned char& value) {
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

MsgPack::Stream& MsgPack::Stream::operator>>(unsigned short& value) {
    unsigned char data = getNextByte();
    
    if(data == 0xCD) {
        readByte2(&value);
        return *this;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

MsgPack::Stream& MsgPack::Stream::operator>>(unsigned long& value) {
    unsigned char data = getNextByte();
    
    if(data == 0xCE) {
        readByte4(&value);
        return *this;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

MsgPack::Stream& MsgPack::Stream::operator>>(unsigned long long& value) {
    unsigned char data = getNextByte();
    
    if(data == 0xCF) {
        readByte8(&value);
        return *this;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

MsgPack::Stream& MsgPack::Stream::operator>>(char& value) {
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

MsgPack::Stream& MsgPack::Stream::operator>>(short& value) {
    unsigned char data = getNextByte();
    
    if(data == 0xD2) {
        readByte2(&value);
        return *this;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

MsgPack::Stream& MsgPack::Stream::operator>>(long& value) {
    unsigned char data = getNextByte();
    
    if(data == 0xD2) {
        readByte4(&value);
        return *this;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

MsgPack::Stream& MsgPack::Stream::operator>>(long long& value) {
    unsigned char data = getNextByte();
    
    if(data == 0xD3) {
        readByte8(&value);
        return *this;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

MsgPack::Stream& MsgPack::Stream::operator>>(const void* null) {
    if(null != NULL)
        throw Exception(Exception::EXPECTED_NULL);
    
    unsigned char data = getNextByte();
    
    if(data == 0xC0) {
        streamBuffer->sbumpc();
        return *this;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

MsgPack::Stream& MsgPack::Stream::operator>>(bool& value) {
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

MsgPack::Stream& MsgPack::Stream::operator>>(float& value) {
    unsigned char data = getNextByte();
    
    if(data == 0xCA) {
        float storage;
        readByte4(&storage);
        value = storage;
        return *this;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

MsgPack::Stream& MsgPack::Stream::operator>>(double& value) {
    unsigned char data = getNextByte();
    
    if(data == 0xCB) {
        readByte8(&value);
        return *this;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

MsgPack::Stream& MsgPack::Stream::operator>>(std::string& str) {
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

unsigned long MsgPack::Stream::readArray() {
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

unsigned long MsgPack::Stream::readMap() {
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



MsgPack::Stream& MsgPack::Stream::operator<<(unsigned char value) {
    if(value < 128)
        streamBuffer->sputc(0x80 | value);
    else{
        streamBuffer->sputc(0xCC);
        streamBuffer->sputc(value);
    }
    
    return *this;
}

MsgPack::Stream& MsgPack::Stream::operator<<(unsigned short value) {
    streamBuffer->sputc(0xCD);
    writeByte2(&value);
    return *this;
}

MsgPack::Stream& MsgPack::Stream::operator<<(unsigned long value) {
    streamBuffer->sputc(0xCE);
    writeByte4(&value);
    return *this;
}

MsgPack::Stream& MsgPack::Stream::operator<<(unsigned long long value) {
    streamBuffer->sputc(0xCF);
    writeByte8(&value);
    return *this;
}

MsgPack::Stream& MsgPack::Stream::operator<<(char value) {
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

MsgPack::Stream& MsgPack::Stream::operator<<(short value) {
    streamBuffer->sputc(0xD1);
    writeByte2(&value);
    return *this;
}

MsgPack::Stream& MsgPack::Stream::operator<<(long value) {
    streamBuffer->sputc(0xD2);
    writeByte4(&value);
    return *this;
}

MsgPack::Stream& MsgPack::Stream::operator<<(long long value) {
    streamBuffer->sputc(0xD3);
    writeByte8(&value);
    return *this;
}

MsgPack::Stream& MsgPack::Stream::operator<<(const void* null) {
    if(null != NULL)
        throw Exception(Exception::EXPECTED_NULL);
    
    streamBuffer->sputc(0xC0);
    
    return *this;
}

MsgPack::Stream& MsgPack::Stream::operator<<(bool value) {
    streamBuffer->sputc((value) ? 0xC3 : 0xC2);
    return *this;
}

MsgPack::Stream& MsgPack::Stream::operator<<(float value) {
    streamBuffer->sputc(0xCA);
    writeByte4(&value);
    return *this;
}

MsgPack::Stream& MsgPack::Stream::operator<<(double value) {
    streamBuffer->sputc(0xCB);
    writeByte8(&value);
    return *this;
}

MsgPack::Stream& MsgPack::Stream::operator<<(const std::string& str) {
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

MsgPack::Stream& MsgPack::Stream::operator<<(const char* str) {
    return *this << std::string(str);
}

void MsgPack::Stream::writeArray(unsigned long size) {
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

void MsgPack::Stream::writeMap(unsigned long size) {
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

};