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

#include "include/MsgPack.h"

void storeUint8(uint8_t* target, uint8_t source) {
    *reinterpret_cast<uint8_t*>(target) = source;
}

void storeInt8(uint8_t* target, int8_t source) {
    *reinterpret_cast<int8_t*>(target) = source;
}

void storeUint16(uint8_t* target, uint16_t source) {
    *reinterpret_cast<uint16_t*>(target) = htons(source);
}

void storeInt16(uint8_t* target, int16_t source) {
    *reinterpret_cast<int16_t*>(target) = htons(source);
}

void storeFloat32(uint8_t* target, float source) {
    *reinterpret_cast<float*>(target) = htonl(source);
}

void storeUint32(uint8_t* target, uint32_t source) {
    *reinterpret_cast<uint32_t*>(target) = htonl(source);
}

void storeInt32(uint8_t* target, int32_t source) {
    *reinterpret_cast<int32_t*>(target) = htonl(source);
}

void storeFloat64(uint8_t* target, double source) {
    #if BIG_ENDIAN
    *reinterpret_cast<double*>(target) = source;
    #else
    *reinterpret_cast<double*>(target) = __builtin_bswap64(source);
    #endif
}

void storeUint64(uint8_t* target, uint64_t source) {
    #if BIG_ENDIAN
    *reinterpret_cast<uint64_t*>(target) = source;
    #else
    *reinterpret_cast<uint64_t*>(target) = __builtin_bswap64(source);
    #endif
}

void storeInt64(uint8_t* target, int64_t source) {
    #if BIG_ENDIAN
    *reinterpret_cast<int64_t*>(target) = source;
    #else
    *reinterpret_cast<int64_t*>(target) = __builtin_bswap64(source);
    #endif
}



uint8_t readUint8(const uint8_t* source) {
    return *reinterpret_cast<const uint8_t*>(source);
}

int8_t readInt8(const uint8_t* source) {
    return *reinterpret_cast<const int8_t*>(source);
}

uint16_t readUint16(const uint8_t* source) {
    return ntohs(*reinterpret_cast<const uint16_t*>(source));
}

int16_t readInt16(const uint8_t* source) {
    return ntohs(*reinterpret_cast<const int16_t*>(source));
}

float readFloat32(const uint8_t* source) {
    return ntohl(*reinterpret_cast<const float*>(source));
}

uint32_t readUint32(const uint8_t* source) {
    return ntohl(*reinterpret_cast<const uint32_t*>(source));
}

int32_t readInt32(const uint8_t* source) {
    return ntohl(*reinterpret_cast<const int32_t*>(source));
}

double readFloat64(const uint8_t* source) {
    #if BIG_ENDIAN
    return *reinterpret_cast<const double*>(source);
    #else
    return __builtin_bswap64(*reinterpret_cast<const double*>(source));
    #endif
}

uint64_t readUint64(const uint8_t* source) {
    #if BIG_ENDIAN
    return *reinterpret_cast<const uint64_t*>(source);
    #else
    return __builtin_bswap64(*reinterpret_cast<const uint64_t*>(source));
    #endif
}

int64_t readInt64(const uint8_t* source) {
    #if BIG_ENDIAN
    return *reinterpret_cast<const int64_t*>(source);
    #else
    return __builtin_bswap64(*reinterpret_cast<const int64_t*>(source));
    #endif
}



namespace MsgPack {

    AbstractObject::AbstractObject(int64_t& pos, std::streambuf* streamBuffer) {
        type = (uint8_t)streamBuffer->sbumpc();
        pos = 1;
    }

    AbstractObject::AbstractObject(int64_t& pos, Type _type) : type(_type) {
        switch(type) {
            case Type::BOOL_FALSE:
            case Type::BOOL_TRUE:
            break;
            default:
                type = Type::NIL;
            break;
        }
        pos = 0;
    }

    AbstractObject::AbstractObject(int64_t& pos, bool value) :
    AbstractObject(pos, (value) ? Type::BOOL_TRUE : Type::BOOL_FALSE) {
        pos = 0;
    }

    std::streamsize AbstractObject::serialize(int64_t& pos, std::streambuf* streamBuffer, std::streamsize bytes) {
        if(bytes > 0 && pos == 0 && (streamBuffer->sputc(type) >= 0)) {
            return (++ pos);
        }else
            return 0;
    }

    std::streamsize AbstractObject::deserialize(int64_t& pos, std::streambuf* streamBuffer, std::streamsize bytes) {
        return 0;
    }

    int64_t AbstractObject::getEndPos() const {
        return 1;
    }

    void AbstractObject::stringify(std::ostream& stream) const {
        switch(type) {
            case Type::NIL:
                stream << "null";
            return;
            case Type::ERROR:
                stream << "error";
            break;
            case Type::BOOL_FALSE:
                stream << "false";
            break;
            case Type::BOOL_TRUE:
                stream << "true";
            break;
        }
    }

    Type AbstractObject::getType() const {
        return (Type)type;
    }

    bool AbstractObject::isNull() const {
        return (type == Type::NIL);
    }

    bool AbstractObject::getValue() const {
        return (type == Type::BOOL_TRUE);
    }



    std::streamsize RawObject::serialize(int64_t& pos, std::streambuf* streamBuffer, std::streamsize bytes) {
        std::streamsize bytesDone = 0;

        if(pos < 0) {
            bytesDone = std::min(bytes, (std::streamsize)(-pos));
            bytesDone = streamBuffer->sputn(reinterpret_cast<char*>(header+getHeaderLength()+pos), bytesDone);
            pos += bytesDone;
        }

        if(pos >= 0 && data) {
            bytes = std::min(bytes, (std::streamsize)(getEndPos()-pos));
            bytes = streamBuffer->sputn(reinterpret_cast<char*>(data.get()+pos), bytes);
            pos += bytes;
            bytesDone += bytes;
        }

        return bytesDone;
    }

    std::streamsize RawObject::deserialize(int64_t& pos, std::streambuf* streamBuffer, std::streamsize bytes) {
        std::streamsize bytesDone = 0;

        if(pos < 0) {
            bytesDone = std::min(bytes, (std::streamsize)(-pos));
            bytesDone = streamBuffer->sgetn(reinterpret_cast<char*>(header+getHeaderLength()+pos), bytesDone);
            pos += bytesDone;
        }

        if(pos >= 0) {
            size_t dataLen = getEndPos();
            if(dataLen > 0) {
                if(!data)
                    data.reset(new uint8_t[dataLen]);

                bytes = std::min(bytes, (std::streamsize)(dataLen-pos));
                bytes = streamBuffer->sgetn(reinterpret_cast<char*>(data.get()+pos), bytes);
                pos += bytes;
                bytesDone += bytes;
            }
        }

        return bytesDone;
    }

    Type RawObject::getType() const {
        return (Type)header[0];
    }



    BinObject::BinObject(int64_t& pos, std::streambuf* streamBuffer) {
        header[0] = (uint8_t)streamBuffer->sbumpc();
        pos = 1-getHeaderLength();
    }

    BinObject::BinObject(int64_t& pos, size_t len, const uint8_t* _data) {
        if(len > 0) {
            data.reset(new uint8_t[len]);
            memcpy(data.get(), _data, len);
        }

        if(len <= 0xFF) {
            pos = -2;
            header[0] = Type::BIN_8;
            storeUint8(&header[1], len);
        }else if(len <= 0xFFFF) {
            pos = -3;
            header[0] = Type::BIN_16;
            storeUint16(&header[1], len);
        }else{
            pos = -5;
            header[0] = Type::BIN_32;
            storeUint32(&header[1], len);
        }
    }

    int64_t BinObject::getEndPos() const {
        switch(header[0]) {
            case Type::BIN_8:
                return readUint8(&header[1]);
            case Type::BIN_16:
                return readUint16(&header[1]);
            case Type::BIN_32:
                return readUint32(&header[1]);
        }
        return 0;
    }

    int64_t BinObject::getHeaderLength() const {
        switch(header[0]) {
            case Type::BIN_8:
                return 2;
            case Type::BIN_16:
                return 3;
            case Type::BIN_32:
                return 5;
        }
        return 0;
    }

    void BinObject::stringify(std::ostream& stream) const {
        stream << "<Buffer ";
        stream << getEndPos();
        stream << ">";
    }

    uint8_t* BinObject::getData() const {
        return data.get();
    }



    ExtensionObject::ExtensionObject(int64_t& pos, std::streambuf* streamBuffer) {
        header[0] = (uint8_t)streamBuffer->sbumpc();
        pos = 1-getHeaderLength();
    }

    ExtensionObject::ExtensionObject(int64_t& pos, size_t len, const uint8_t* _data, uint8_t type) {
        data.reset(new uint8_t[len+1]);
        data[0] = type;
        memcpy(&data[1], _data, len);

        pos = -1;
        switch(len) {
            case 1:
                header[0] = Type::FIXEXT_8;
            break;
            case 2:
                header[0] = Type::FIXEXT_16;
            break;
            case 4:
                header[0] = Type::FIXEXT_32;
            break;
            case 8:
                header[0] = Type::FIXEXT_64;
            break;
            case 16:
                header[0] = Type::FIXEXT_128;
            break;
            default:
                if(len <= 0xFF) {
                    pos = -2;
                    header[0] = Type::EXT_8;
                    storeUint8(&header[1], len);
                }else if(len <= 0xFFFF) {
                    pos = -3;
                    header[0] = Type::EXT_16;
                    storeUint16(&header[1], len);
                }else{
                    pos = -5;
                    header[0] = Type::EXT_32;
                    storeUint32(&header[1], len);
                }
            break;
        }
    }

    int64_t ExtensionObject::getEndPos() const {
        switch(header[0]) {
            case Type::FIXEXT_8:
                return 1;
            case Type::FIXEXT_16:
                return 2;
            case Type::FIXEXT_32:
                return 4;
            case Type::FIXEXT_64:
                return 8;
            case Type::FIXEXT_128:
                return 16;
            case Type::EXT_8:
                return readUint8(&header[1]);
            case Type::EXT_16:
                return readUint16(&header[1]);
            case Type::EXT_32:
                return readUint32(&header[1]);
        }
        return 0;
    }

    int64_t ExtensionObject::getHeaderLength() const {
        switch(header[0]) {
            case Type::FIXEXT_8:
            case Type::FIXEXT_16:
            case Type::FIXEXT_32:
            case Type::FIXEXT_64:
            case Type::FIXEXT_128:
                return 1;
            case Type::EXT_8:
                return 2;
            case Type::EXT_16:
                return 3;
            case Type::EXT_32:
                return 5;
        }
        return 0;
    }

    void ExtensionObject::stringify(std::ostream& stream) const {
        stream << "<Extension ";
        stream << getEndPos();
        stream << ">";
    }

    uint8_t ExtensionObject::getDataType() const {
        return data[0];
    }

    uint8_t* ExtensionObject::getData() const {
        return &data[1];
    }



    StringObject::StringObject(int64_t& pos, std::streambuf* streamBuffer) {
        header[0] = (uint8_t)streamBuffer->sbumpc();
        pos = 1-getHeaderLength();
    }

    StringObject::StringObject(int64_t& pos, const std::string& str) {
        size_t len = str.size();
        if(len > 0) {
            data.reset(new uint8_t[len]);
            memcpy(data.get(), &*str.begin(), len);
        }

        if(len < 0x20) {
            pos = -1;
            header[0] = Type::FIXSTR+len;
        }else if(len <= 0xFF) {
            pos = -2;
            header[0] = Type::STR_8;
            storeUint8(&header[1], len);
        }else if(len <= 0xFFFF) {
            pos = -3;
            header[0] = Type::STR_16;
            storeUint16(&header[1], len);
        }else{
            pos = -5;
            header[0] = Type::STR_32;
            storeUint32(&header[1], len);
        }
    }

    int64_t StringObject::getEndPos() const {
        if(header[0] >= Type::FIXSTR && header[0] < Type::NIL)
            return header[0] - Type::FIXSTR;

        switch(header[0]) {
            case Type::STR_8:
                return readUint8(&header[1]);
            case Type::STR_16:
                return readUint16(&header[1]);
            case Type::STR_32:
                return readUint32(&header[1]);
        }
        return 0;
    }

    int64_t StringObject::getHeaderLength() const {
        if(header[0] >= Type::FIXSTR && header[0] < Type::NIL)
            return 1;
        switch(header[0]) {
            case Type::STR_8:
                return 2;
            case Type::STR_16:
                return 3;
            case Type::STR_32:
                return 5;
        }
        return 0;
    }

    void StringObject::stringify(std::ostream& stream) const {
        stream << "\"";
        stream << getStr();
        stream << "\"";
    }

    std::string StringObject::getStr() const {
        return std::string((const char*)data.get(), getEndPos());
    }



    NumberObject::NumberObject(int64_t& pos, std::streambuf* streamBuffer) {
        data[0] = (uint8_t)streamBuffer->sbumpc();
        pos = 1;
    }

    NumberObject::NumberObject(int64_t& pos, uint64_t value) {
        pos = 0;
        if(value < 0x80ULL)
            data[0] = value;
        else if(value <= 0xFFULL) {
            data[0] = Type::UINT_8;
            storeUint8(data+1, value);
        }else if(value <= 0xFFFFULL) {
            data[0] = Type::UINT_16;
            storeUint16(data+1, value);
        }else if(value <= 0xFFFFFFFFULL) {
            data[0] = Type::UINT_32;
            storeUint32(data+1, value);
        }else{
            data[0] = Type::UINT_64;
            storeUint64(data+1, value);
        }
    }

    NumberObject::NumberObject(int64_t& pos, int64_t value) {
        pos = 0;
        if(value >= -0x20LL && value < 0x80LL)
            storeInt8(data, value);
        else if(value >= -0x80LL && value < 0x80LL) {
            data[0] = Type::INT_8;
            storeInt8(data+1, value);
        }else if(value >= -0x8000LL && value < 0x8000LL) {
            data[0] = Type::INT_16;
            storeInt16(data+1, value);
        }else if(value >= -0x80000000LL && value < 0x80000000LL) {
            data[0] = Type::INT_32;
            storeInt32(data+1, value);
        }else{
            data[0] = Type::INT_64;
            storeInt64(data+1, value);
        }
    }

    NumberObject::NumberObject(int64_t& pos, float value) {
        pos = 0;
        data[0] = Type::FLOAT_32;
        storeFloat32(data+1, value);
    }

    NumberObject::NumberObject(int64_t& pos, double value) {
        pos = 0;
        float fValue = (float)value;
        if((double)fValue == value) {
            data[0] = Type::FLOAT_32;
            storeFloat32(data+1, fValue);
        }else{
            data[0] = Type::FLOAT_64;
            storeFloat64(data+1, value);
        }
    }

    std::streamsize NumberObject::serialize(int64_t& pos, std::streambuf* streamBuffer, std::streamsize bytes) {
        bytes = std::min(bytes, (std::streamsize)(getEndPos()-pos));
        bytes = streamBuffer->sputn(reinterpret_cast<char*>(data+pos), bytes);
        pos += bytes;
        return bytes;
    }

    std::streamsize NumberObject::deserialize(int64_t& pos, std::streambuf* streamBuffer, std::streamsize bytes) {
        bytes = std::min(bytes, (std::streamsize)(getEndPos()-pos));
        bytes = streamBuffer->sgetn(reinterpret_cast<char*>(data+pos), bytes);
        pos += bytes;
        return bytes;
    }

    int64_t NumberObject::getEndPos() const {
        if(data[0] < 0x80 || data[0] >= 0xE0)
            return 1;
        else
            switch(data[0]) {
                case Type::UINT_8:
                case Type::INT_8:
                    return 2;
                case Type::UINT_16:
                case Type::INT_16:
                    return 3;
                case Type::FLOAT_32:
                case Type::UINT_32:
                case Type::INT_32:
                    return 5;
                case Type::FLOAT_64:
                case Type::UINT_64:
                case Type::INT_64:
                    return 9;
            }
        return 0;
    }

    void NumberObject::stringify(std::ostream& stream) const {
            if(data[0] < 0x80 || data[0] >= 0xE0)
                stream << reinterpret_cast<const int8_t&>(data[0]);
            else
                switch(data[0]) {
                    case Type::FLOAT_32:
                        stream << readFloat32(data+1);
                    break;
                    case Type::FLOAT_64:
                        stream << readFloat64(data+1);
                    break;
                    case Type::UINT_8:
                        stream << readUint8(data+1);
                    break;
                    case Type::UINT_16:
                        stream << readUint16(data+1);
                    break;
                    case Type::UINT_32:
                        stream << readUint32(data+1);
                    break;
                    case Type::UINT_64:
                        stream << readUint64(data+1);
                    break;
                    case Type::INT_8:
                        stream << readInt8(data+1);
                    break;
                    case Type::INT_16:
                        stream << readInt16(data+1);
                    break;
                    case Type::INT_32:
                        stream << readInt32(data+1);
                    break;
                    case Type::INT_64:
                        stream << readInt64(data+1);
                    break;
                }
    }

    Type NumberObject::getType() const {
        return (Type)data[0];
    }



    std::ostream& operator<<(std::ostream& ostream, const Object& obj) {
        obj.stringify(ostream);
        return ostream;
    }

    std::streamsize Parser::parse(std::streamsize bytesLeft) {
        //bool parseAll = (bytes == 0);

        std::streamsize prevBytes = bytesLeft;

        while(bytesLeft > 0) {
            if(currentObject) {
                std::streamsize bytesRead = currentObject->deserialize(currentPosition, streamBuffer, bytesLeft);
                bytesLeft -= bytesRead;

                if(currentPosition == currentObject->getEndPos()) {
                    onObjectParsed(std::move(currentObject));
                    currentObject.reset();
                }else if(bytesRead == 0) break;
            }else{
                int read = streamBuffer->sgetc();
                if(read < 0) break;
                uint8_t nextByte = (uint8_t)read;
                bytesLeft --;

                if(nextByte < Type::FIXMAP || nextByte >= Type::FIXINT) {
                    currentObject.reset(new NumberObject(currentPosition, streamBuffer));
                }else if(nextByte < Type::FIXARRAY) {
                    //currentObject.reset(new MapObject(streamBuffer));
                }else if(nextByte < Type::FIXSTR) {
                    //currentObject.reset(new ArrayObject(streamBuffer));
                }else if(nextByte < Type::NIL) {
                    currentObject.reset(new StringObject(currentPosition, streamBuffer));
                }else
                    switch(nextByte) {
                        case Type::NIL:
                        case Type::ERROR:
                        case Type::BOOL_FALSE:
                        case Type::BOOL_TRUE:
                            currentObject.reset(new AbstractObject(currentPosition, streamBuffer));
                        break;
                        case Type::BIN_8:
                        case Type::BIN_16:
                        case Type::BIN_32:
                            currentObject.reset(new BinObject(currentPosition, streamBuffer));
                        break;
                        case Type::EXT_8:
                        case Type::EXT_16:
                        case Type::EXT_32:
                        case Type::FIXEXT_8:
                        case Type::FIXEXT_16:
                        case Type::FIXEXT_32:
                        case Type::FIXEXT_64:
                        case Type::FIXEXT_128:
                            currentObject.reset(new ExtensionObject(currentPosition, streamBuffer));
                        break;
                        case Type::STR_8:
                        case Type::STR_16:
                        case Type::STR_32:
                            currentObject.reset(new StringObject(currentPosition, streamBuffer));
                        break;
                        case Type::ARRAY_16:
                        case Type::ARRAY_32:
                            //currentObject.reset(new ArrayObject(streamBuffer));
                        break;
                        case Type::MAP_16:
                        case Type::MAP_32:
                            //currentObject.reset(new MapObject(streamBuffer));
                        break;
                        /*case Type::FLOAT_32:
                        case Type::FLOAT_64:
                        case Type::UINT_8:
                        case Type::UINT_16:
                        case Type::UINT_32:
                        case Type::UINT_64:
                        case Type::INT_8:
                        case Type::INT_16:
                        case Type::INT_32:
                        case Type::INT_64:*/
                        default:
                            currentObject.reset(new NumberObject(currentPosition, streamBuffer));
                        break;
                    }
            }
        }

        return prevBytes-bytesLeft;
    }
};