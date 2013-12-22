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

//Read unsigned integer

MsgPack::Stream& MsgPack::Stream::operator>>(uint8_t& value) {
    uint8_t data = getNextByte();
    
    if((data & 0x80) == 0) {
        streamBuffer->sbumpc();
        value = data;
        return *this;
    }

    if(data == 0xCC) {
        readByte1(&value);
        return *this;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

MsgPack::Stream& MsgPack::Stream::operator>>(uint16_t& value) {
    uint8_t data = getNextByte();
    
    if((data & 0x80) == 0) {
        streamBuffer->sbumpc();
        value = data;
        return *this;
    }

    switch(data) {
        case 0xCC: {
            uint8_t _value;
            readByte1(&_value);
            value = _value;
        } return *this;
        case 0xCD:
            readByte2(&value);
        return *this;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

MsgPack::Stream& MsgPack::Stream::operator>>(uint32_t& value) {
    uint8_t data = getNextByte();
    
    if((data & 0x80) == 0) {
        streamBuffer->sbumpc();
        value = data;
        return *this;
    }

    switch(data) {
        case 0xCC: {
            uint8_t _value;
            readByte1(&_value);
            value = _value;
        } return *this;
        case 0xCD: {
            uint16_t _value;
            readByte2(&_value);
            value = _value;
        } return *this;
        case 0xCE:
            readByte4(&value);
        return *this;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

MsgPack::Stream& MsgPack::Stream::operator>>(uint64_t& value) {
    uint8_t data = getNextByte();
    
    if((data & 0x80) == 0) {
        streamBuffer->sbumpc();
        value = data;
        return *this;
    }

    switch(data) {
        case 0xCC: {
            uint8_t _value;
            readByte1(&_value);
            value = _value;
        } return *this;
        case 0xCD: {
            uint16_t _value;
            readByte2(&_value);
            value = _value;
        } return *this;
        case 0xCE: {
            uint32_t _value;
            readByte4(&_value);
            value = _value;
        } return *this;
        case 0xCF:
            readByte8(&value);
        return *this;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

//Read signed integer

MsgPack::Stream& MsgPack::Stream::operator>>(int8_t& value) {
    uint8_t data = getNextByte();
    
    if((data & 0x80) == 0) {
        streamBuffer->sbumpc();
        value = data;
        return *this;
    }
    
    if((data & 0xE0) == 0xE0) {
        streamBuffer->sbumpc();
        value = -(data & 0x1F)-1;
        return *this;
    }

    if(data == 0xCC) {
        readByte1(&value);
        return *this;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

MsgPack::Stream& MsgPack::Stream::operator>>(int16_t& value) {
    uint8_t data = getNextByte();
    
    if((data & 0x80) == 0) {
        streamBuffer->sbumpc();
        value = data;
        return *this;
    }
    
    if((data & 0xE0) == 0xE0) {
        streamBuffer->sbumpc();
        value = -(data & 0x1F)-1;
        return *this;
    }

    switch(data) {
        case 0xCC: {
            uint8_t _value;
            readByte1(&_value);
            value = _value;
        } return *this;
        case 0xD0: {
            int8_t _value;
            readByte1(&_value);
            value = _value;
        } return *this;
        case 0xD1:
            readByte2(&value);
        return *this;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

MsgPack::Stream& MsgPack::Stream::operator>>(int32_t& value) {
    uint8_t data = getNextByte();
    
    if((data & 0x80) == 0) {
        streamBuffer->sbumpc();
        value = data;
        return *this;
    }
    
    if((data & 0xE0) == 0xE0) {
        streamBuffer->sbumpc();
        value = -(data & 0x1F)-1;
        return *this;
    }

    switch(data) {
        case 0xCC: {
            uint8_t _value;
            readByte1(&_value);
            value = _value;
        } return *this;
        case 0xCD: {
            uint16_t _value;
            readByte2(&_value);
            value = _value;
        } return *this;
        case 0xD0: {
            int8_t _value;
            readByte1(&_value);
            value = _value;
        } return *this;
        case 0xD1: {
            int16_t _value;
            readByte2(&_value);
            value = _value;
        } return *this;
        case 0xD2:
            readByte4(&value);
        return *this;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

MsgPack::Stream& MsgPack::Stream::operator>>(int64_t& value) {
    uint8_t data = getNextByte();
    
    if((data & 0x80) == 0) {
        streamBuffer->sbumpc();
        value = data;
        return *this;
    }
    
    if((data & 0xE0) == 0xE0) {
        streamBuffer->sbumpc();
        value = -(data & 0x1F)-1;
        return *this;
    }

    switch(data) {
        case 0xCC: {
            uint8_t _value;
            readByte1(&_value);
            value = _value;
        } return *this;
        case 0xCD: {
            uint16_t _value;
            readByte2(&_value);
            value = _value;
        } return *this;
        case 0xCE: {
            uint32_t _value;
            readByte4(&_value);
            value = _value;
        } return *this;
        case 0xD0: {
            int8_t _value;
            readByte1(&_value);
            value = _value;
        } return *this;
        case 0xD1: {
            int16_t _value;
            readByte2(&_value);
            value = _value;
        } return *this;
        case 0xD2: {
            int32_t _value;
            readByte4(&_value);
            value = _value;
        } return *this;
        case 0xD3: {
            readByte8(&value);
        } return *this;
    }
    
    throw Exception(Exception::BAD_TYPE);
}

//Write unsigned integer

MsgPack::Stream& MsgPack::Stream::operator<<(uint8_t value) {
    if(value < 0x80)
        streamBuffer->sputc(value);
    else{
        streamBuffer->sputc(0xCC);
        streamBuffer->sputc(value);
    }
    
    return *this;
}

MsgPack::Stream& MsgPack::Stream::operator<<(uint16_t value) {
    if(value < 0x80)
        streamBuffer->sputc((uint8_t)value);
    else if(value <= 0xFF) {
        streamBuffer->sputc(0xCC);
        streamBuffer->sputc((uint8_t)value);
    }else{
        streamBuffer->sputc(0xCD);
        writeByte2(&value);
    }
    
    return *this;
}

MsgPack::Stream& MsgPack::Stream::operator<<(uint32_t value) {
    if(value < 0x80)
        streamBuffer->sputc((uint8_t)value);
    else if(value <= 0xFF) {
        streamBuffer->sputc(0xCC);
        streamBuffer->sputc((uint8_t)value);
    }else if(value <= 0xFFFF) {
        uint16_t _value = value;
        streamBuffer->sputc(0xCD);
        writeByte2(&_value);
    }else{
        streamBuffer->sputc(0xCE);
        writeByte4(&value);
    }
    
    return *this;
}

MsgPack::Stream& MsgPack::Stream::operator<<(uint64_t value) {
    if(value < 0x80)
        streamBuffer->sputc((uint8_t)value);
    else if(value <= 0xFF) {
        streamBuffer->sputc(0xCC);
        streamBuffer->sputc((uint8_t)value);
    }else if(value <= 0xFFFF) {
        uint16_t _value = value;
        streamBuffer->sputc(0xCD);
        writeByte2(&_value);
    }else if(value <= 0xFFFFFFFF) {
        uint32_t _value = value;
        streamBuffer->sputc(0xCE);
        writeByte4(&_value);
    }else{
        streamBuffer->sputc(0xCF);
        writeByte8(&value);
    }
    
    return *this;
}

//Write signed integer

MsgPack::Stream& MsgPack::Stream::operator<<(int8_t value) {
    if(value >= -0x20 && value < 0x00)
        streamBuffer->sputc(0xE0 | (-1-(int8_t)value));
    else if(value >= 0x00 && value < 0x80)
        streamBuffer->sputc(value);
    else{
        streamBuffer->sputc(0xD0);
        streamBuffer->sputc(value);
    }
    
    return *this;
}

MsgPack::Stream& MsgPack::Stream::operator<<(int16_t value) {
    if(value >= -0x20 && value < 0x00)
        streamBuffer->sputc(0xE0 | (-1-(int8_t)value));
    else if(value >= 0x00 && value < 0x80)
        streamBuffer->sputc(value);
    else if(value >= -0x80 && value < 0x80) {
        streamBuffer->sputc(0xD0);
        streamBuffer->sputc(value);
    }else{
        streamBuffer->sputc(0xD1);
        writeByte2(&value);
    }
    
    return *this;
}

MsgPack::Stream& MsgPack::Stream::operator<<(int32_t value) {
    if(value >= -0x20 && value < 0x00)
        streamBuffer->sputc(0xE0 | (-1-(int8_t)value));
    else if(value >= 0x00 && value < 0x80)
        streamBuffer->sputc(value);
    else if(value >= -0x80 && value < 0x80) {
        streamBuffer->sputc(0xD0);
        streamBuffer->sputc(value);
    }else if(value >= -0x8000 && value < 0x8000) {
        int16_t _value = value;
        streamBuffer->sputc(0xD1);
        writeByte2(&_value);
    }else{
        streamBuffer->sputc(0xD2);
        writeByte4(&value);
    }
    
    return *this;
}

MsgPack::Stream& MsgPack::Stream::operator<<(int64_t value) {
    if(value >= -0x20 && value < 0x00)
        streamBuffer->sputc(0xE0 | (-1-(int8_t)value));
    else if(value >= 0x00 && value < 0x80)
        streamBuffer->sputc(value);
    else if(value >= -0x80 && value < 0x80) {
        streamBuffer->sputc(0xD0);
        streamBuffer->sputc(value);
    }else if(value >= -0x8000 && value < 0x8000) {
        int16_t _value = value;
        streamBuffer->sputc(0xD1);
        writeByte2(&_value);
    }else if(value >= -0x80000000 && value < 0x80000000) {
        int32_t _value = value;
        streamBuffer->sputc(0xD2);
        writeByte4(&_value);
    }else{
        streamBuffer->sputc(0xD3);
        writeByte8(&value);
    }
    
    return *this;
}

};