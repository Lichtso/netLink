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

#include "include/Core.h"

namespace netLink {

uint8_t MsgPack::Stream::getNextByte() {
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
    checkBytesInAvail(2);
    streamBuffer->sbumpc();
    *reinterpret_cast<uint8_t*>(storage) = streamBuffer->sbumpc();
}

void MsgPack::Stream::readByte2(void* _storage) {
    checkBytesInAvail(3);
    streamBuffer->sbumpc();
    uint8_t* storage = reinterpret_cast<uint8_t*>(_storage);
    #if LITTLE_ENDIAN
    storage[0] = streamBuffer->sbumpc();
    storage[1] = streamBuffer->sbumpc();
    #else
    storage[1] = streamBuffer->sbumpc();
    storage[0] = streamBuffer->sbumpc();
    #endif
}

void MsgPack::Stream::readByte4(void* _storage) {
    checkBytesInAvail(5);
    streamBuffer->sbumpc();
    uint8_t* storage = reinterpret_cast<uint8_t*>(_storage);
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
    checkBytesInAvail(9);
    streamBuffer->sbumpc();
    uint8_t* storage = reinterpret_cast<uint8_t*>(_storage);
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
    uint8_t* storage = reinterpret_cast<uint8_t*>(_storage);
    #if LITTLE_ENDIAN
    streamBuffer->sputc(storage[0]);
    streamBuffer->sputc(storage[1]);
    #else
    streamBuffer->sputc(storage[1]);
    streamBuffer->sputc(storage[0]);
    #endif
}

void MsgPack::Stream::writeByte4(void* _storage) {
    uint8_t* storage = reinterpret_cast<uint8_t*>(_storage);
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
    uint8_t* storage = reinterpret_cast<uint8_t*>(_storage);
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
    uint8_t data = getNextByte();
    
    if((data & 0x80) == 0)
        return UINT;

    switch(data & 0xF0) {
        case 0x80:
            return MAP;
        case 0x90:
            return ARRAY;
    }

    switch(data & 0xE0) {
        case 0xA0:
            return STRING;
        case 0xE0:
            return INT;
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
            return FLOAT;
        case 0xCB:
            return DOUBLE;
        case 0xCC:
        case 0xCD:
        case 0xCE:
        case 0xCF:
            return UINT;
        case 0xD0:
        case 0xD1:
        case 0xD2:
        case 0xD3:
            return INT;
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

MsgPack::Stream& MsgPack::Stream::operator>>(const void* null) {
    if(null != NULL)
        throw Exception(Exception::EXPECTED_NULL);
    
    uint8_t data = getNextByte();
    
    if(data == 0xC0) {
        streamBuffer->sbumpc();
        return *this;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

MsgPack::Stream& MsgPack::Stream::operator>>(bool& value) {
    uint8_t data = getNextByte();
    
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

std::unique_ptr<char[]> MsgPack::Stream::readRaw(uint32_t& size) {
    uint8_t data = getNextByte();
    
    switch(data) {
        case 0xC4: {
            uint8_t _size;
            readByte1(&_size);
            size = _size;
            checkBytesInAvail(size, 1);
            char* data = new char[size];
            streamBuffer->sgetn(data, size);
            return std::unique_ptr<char[]>(data);
        } case 0xC5: {
            uint16_t _size;
            readByte2(&_size);
            size = _size;
            checkBytesInAvail(size, 2);
            char* data = new char[size];
            streamBuffer->sgetn(data, size);
            return std::unique_ptr<char[]>(data);
        } case 0xC6: {
            streamBuffer->sbumpc();
            readByte4(&size);
            checkBytesInAvail(size, 4);
            char* data = new char[size];
            streamBuffer->sgetn(data, size);
            return std::unique_ptr<char[]>(data);
        }
    }
    
    throw Exception(Exception::BAD_TYPE);
}

std::unique_ptr<char[]> MsgPack::Stream::readExtended(int8_t& type, uint32_t& size) {
    uint8_t data = getNextByte();
    
    switch(data) {
        case 0xD4: {
            checkBytesInAvail(3);
            readByte1(&type);
            char* data = new char[1];
            streamBuffer->sgetn(data, 1);
            return std::unique_ptr<char[]>(data);
        } case 0xD5: {
            checkBytesInAvail(4);
            readByte1(&type);
            char* data = new char[2];
            streamBuffer->sgetn(data, 2);
            return std::unique_ptr<char[]>(data);
        } case 0xD6: {
            checkBytesInAvail(6);
            readByte1(&type);
            char* data = new char[4];
            streamBuffer->sgetn(data, 4);
            return std::unique_ptr<char[]>(data);
        } case 0xD7: {
            checkBytesInAvail(10);
            readByte1(&type);
            char* data = new char[8];
            streamBuffer->sgetn(data, 8);
            return std::unique_ptr<char[]>(data);
        } case 0xD8: {
            checkBytesInAvail(18);
            readByte1(&type);
            char* data = new char[16];
            streamBuffer->sgetn(data, 16);
            return std::unique_ptr<char[]>(data);
        } case 0xC7: {
            checkBytesInAvail(3);
            uint8_t _size;
            readByte1(&_size);
            size = _size;
            type = streamBuffer->sbumpc();
            checkBytesInAvail(size, 2);
            char* data = new char[size];
            streamBuffer->sgetn(data, size);
            return std::unique_ptr<char[]>(data);
        } case 0xC8: {
            checkBytesInAvail(4);
            uint16_t _size;
            readByte2(&_size);
            size = _size;
            type = streamBuffer->sbumpc();
            checkBytesInAvail(size, 3);
            char* data = new char[size];
            streamBuffer->sgetn(data, size);
            return std::unique_ptr<char[]>(data);
        } case 0xC9: {
            checkBytesInAvail(6);
            streamBuffer->sbumpc();
            readByte4(&size);
            type = streamBuffer->sbumpc();
            checkBytesInAvail(size, 5);
            char* data = new char[size];
            streamBuffer->sgetn(data, size);
            return std::unique_ptr<char[]>(data);
        }
    }
    
    throw Exception(Exception::BAD_TYPE);
}

MsgPack::Stream& MsgPack::Stream::operator>>(float& value) {
    uint8_t data = getNextByte();
    
    if(data == 0xCA) {
        readByte4(&value);
        return *this;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

MsgPack::Stream& MsgPack::Stream::operator>>(double& value) {
    uint8_t data = getNextByte();
    
    if(data == 0xCB) {
        readByte8(&value);
        return *this;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

MsgPack::Stream& MsgPack::Stream::operator>>(std::string& str) {
    uint8_t data = getNextByte();
    
    if((data & 0xE0) == 0xA0) {
        uint8_t size = data & 0x1F;
        checkBytesInAvail(size);
        streamBuffer->sbumpc();
        str = std::string(size, 0);
        streamBuffer->sgetn(&*str.begin(), size);
        return *this;
    }
    
    switch(data) {
        case 0xD9: {
            uint8_t size;
            readByte1(&size);
            checkBytesInAvail(size, 1);
            str = std::string(size, 0);
            streamBuffer->sgetn(&*str.begin(), size);
            return *this;
        } case 0xDA: {
            uint16_t size;
            readByte2(&size);
            checkBytesInAvail(size, 2);
            str = std::string(size, 0);
            streamBuffer->sgetn(&*str.begin(), size);
            return *this;
        } case 0xDB: {
            streamBuffer->sbumpc();
            uint32_t size;
            readByte4(&size);
            checkBytesInAvail(size, 4);
            str = std::string(size, 0);
            streamBuffer->sgetn(&*str.begin(), size);
            return *this;
        }
    }
    
    throw Exception(Exception::BAD_TYPE);
}

uint32_t MsgPack::Stream::readArray() {
    uint8_t data = getNextByte();
    
    if((data & 0xF0) == 0x90) {
        streamBuffer->sbumpc();
        return data & 0x0F;
    }
    
    switch(data) {
        case 0xDC: {
            uint16_t size;
            readByte2(&size);
            return size;
        } case 0xDD: {
            uint32_t size;
            readByte4(&size);
            return size;
        }
    }
    
    throw Exception(Exception::BAD_TYPE);
}

uint32_t MsgPack::Stream::readMap() {
    uint8_t data = getNextByte();
    
    if((data & 0xF0) == 0x80) {
        streamBuffer->sbumpc();
        return data & 0x0F;
    }
    
    switch(data) {
        case 0xDE: {
            uint16_t size;
            readByte2(&size);
            return size;
        } case 0xDF: {
            uint16_t size;
            readByte4(&size);
            return size;
        }
    }
    
    throw Exception(Exception::BAD_TYPE);
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

void MsgPack::Stream::writeRaw(const char* data, uint32_t size) {
    if(size <= 0xFF) {
        streamBuffer->sputc(0xC4);
        streamBuffer->sputc((uint8_t)size);
        streamBuffer->sputn(data, size);
    }else if(size <= 0xFFFF) {
        streamBuffer->sputc(0xC5);
        uint16_t _size = size;
        writeByte2(&_size);
        streamBuffer->sputn(data, size);
    }else{
        streamBuffer->sputc(0xC6);
        writeByte4(&size);
        streamBuffer->sputn(data, size);
    }
}

void MsgPack::Stream::writeExtended(int8_t type, const char* data, uint32_t size) {
    switch(size) {
        case 1:
            streamBuffer->sputc(0xD4);
            streamBuffer->sputc(type);
            streamBuffer->sputc(*data);
        break;
        case 2:
            streamBuffer->sputc(0xD5);
            streamBuffer->sputc(type);
            streamBuffer->sputn(data, 2);
        break;
        case 4:
            streamBuffer->sputc(0xD6);
            streamBuffer->sputc(type);
            streamBuffer->sputn(data, 4);
        break;
        case 8:
            streamBuffer->sputc(0xD7);
            streamBuffer->sputc(type);
            streamBuffer->sputn(data, 8);
        break;
        case 16:
            streamBuffer->sputc(0xD8);
            streamBuffer->sputc(type);
            streamBuffer->sputn(data, 16);
        break;
        default:
            if(size <= 0xFF) {
                streamBuffer->sputc(0xC7);
                streamBuffer->sputc(size);
                streamBuffer->sputc(type);
                streamBuffer->sputn(data, size);
            }else if(size <= 0xFFFF) {
                streamBuffer->sputc(0xC8);
                uint16_t _size = size;
                writeByte2(&_size);
                streamBuffer->sputc(type);
                streamBuffer->sputn(data, size);
            }else{
                streamBuffer->sputc(0xC9);
                writeByte4(&size);
                streamBuffer->sputc(type);
                streamBuffer->sputn(data, size);
            }
        break;
    }
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
    uint32_t size = str.size();
    if(size < 0x20) {
        streamBuffer->sputc((uint8_t)size | 0xA0);
        streamBuffer->sputn(str.c_str(), size);
    }else if(size <= 0xFF) {
        streamBuffer->sputc(0xD9);
        streamBuffer->sputc((uint8_t)size);
        streamBuffer->sputn(str.c_str(), size);
    }else if(size <= 0xFFFF) {
        streamBuffer->sputc(0xDA);
        uint16_t _size = size;
        writeByte2(&_size);
        streamBuffer->sputn(str.c_str(), size);
    }else{
        streamBuffer->sputc(0xDB);
        writeByte4(&size);
        streamBuffer->sputn(str.c_str(), size);
    }
    
    return *this;
}

MsgPack::Stream& MsgPack::Stream::operator<<(const char* str) {
    return *this << std::string(str);
}

void MsgPack::Stream::writeArray(uint32_t size) {
    if(size < 0x20)
        streamBuffer->sputc((uint8_t)size | 0x90);
    else if(size <= 0xFFFF) {
        streamBuffer->sputc(0xDC);
        uint16_t _size = size;
        writeByte2(&_size);
    }else{
        streamBuffer->sputc(0xDD);
        writeByte4(&size);
    }
}

void MsgPack::Stream::writeMap(uint32_t size) {
    if(size < 0x20)
        streamBuffer->sputc((uint8_t)size | 0x80);
    else if(size <= 0xFFFF) {
        streamBuffer->sputc(0xDE);
        uint16_t _size = size;
        writeByte2(&_size);
    }else{
        streamBuffer->sputc(0xDF);
        writeByte4(&size);
    }
}

};