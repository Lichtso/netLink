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

#include "MsgPack.h"

#ifdef _MSC_VER
#define __builtin_bswap16 _byteswap_ushort
#define __builtin_bswap32 _byteswap_ulong
#define __builtin_bswap64 _byteswap_uint64
#endif

#if BYTE_ORDER == BIG_ENDIAN
#define storeBSwap(bits) \
    memcpy(target, &source, sizeof(source));
#define loadBSwap(bits) \
    memcpy(&target, source, sizeof(target)); \
    return target;
#else
#define storeBSwap(bits) \
    source = __builtin_bswap##bits(source); \
    memcpy(target, &source, sizeof(source));
#define loadBSwap(bits) \
    memcpy(&target, source, sizeof(target)); \
    target = __builtin_bswap##bits(target); \
    return target;
#endif

void storeUint8(char* target, uint8_t source) {
    *reinterpret_cast<uint8_t*>(target) = source;
}

void storeInt8(char* target, int8_t source) {
    *reinterpret_cast<int8_t*>(target) = source;
}

void storeUint16(char* target, uint16_t source) {
    storeBSwap(16);
}

void storeInt16(char* target, int16_t source) {
    storeBSwap(16);
}

void storeFloat32(char* target, float source) {
    storeUint32(target, reinterpret_cast<const uint32_t&>(source));
}

void storeUint32(char* target, uint32_t source) {
    storeBSwap(32);
}

void storeInt32(char* target, int32_t source) {
    storeBSwap(32);
}

void storeFloat64(char* target, double source) {
    storeUint64(target, reinterpret_cast<const uint64_t&>(source));
}

void storeUint64(char* target, uint64_t source) {
    storeBSwap(64);
}

void storeInt64(char* target, int64_t source) {
    storeBSwap(64);
}



uint8_t loadUint8(const char* source) {
    return *reinterpret_cast<const uint8_t*>(source);
}

int8_t loadInt8(const char* source) {
    return *reinterpret_cast<const int8_t*>(source);
}

uint16_t loadUint16(const char* source) {
    uint16_t target;
    loadBSwap(16);
}

int16_t loadInt16(const char* source) {
    int16_t target;
    loadBSwap(16);
}

float loadFloat32(const char* source) {
    uint32_t target = loadUint32(source);
    return reinterpret_cast<const float&>(target);
}

uint32_t loadUint32(const char* source) {
    uint32_t target;
    loadBSwap(32);
}

int32_t loadInt32(const char* source) {
    int32_t target;
    loadBSwap(32);
}

double loadFloat64(const char* source) {
    uint64_t target = loadUint64(source);
    return reinterpret_cast<const double&>(target);
}

uint64_t loadUint64(const char* source) {
    uint64_t target;
    loadBSwap(64);
}

int64_t loadInt64(const char* source) {
    int64_t target;
    loadBSwap(64);
}



namespace MsgPack {


    Primitive::Primitive(Type _type) : type(_type) { }

    Primitive::Primitive(bool value) : type((value) ? Type::BOOL_TRUE : Type::BOOL_FALSE) { }

    Primitive::Primitive() : type(Type::NIL) { }

    int64_t Primitive::startDeserialize(uint8_t firstByte) {
        type = firstByte;
        return 1;
    }

    std::streamsize Primitive::serialize(int64_t& pos, std::basic_streambuf<char>* streamBuffer, std::streamsize bytes) {
        if(bytes > 0 && pos == 0 && streamBuffer->sputc(type) >= 0)
            return (++pos);
        else
            return 0;
    }

    int64_t Primitive::getEndPos() const {
        return 1;
    }

    std::unique_ptr<Element> Primitive::copy() const {
        return std::unique_ptr<Element>(new Primitive(type));
    }

    void Primitive::toJSON(std::ostream& stream) const {
        switch(type) {
            case Type::NIL:
                stream << "null";
            return;
            case Type::UNDEFINED:
                stream << "undefined";
            break;
            case Type::BOOL_FALSE:
                stream << "false";
            break;
            case Type::BOOL_TRUE:
                stream << "true";
            break;
        }
    }

    Type Primitive::getType() const {
        return (Type)type;
    }

    std::unique_ptr<Element> Factory() {
        return std::unique_ptr<Element>(new Primitive());
    }

    std::unique_ptr<Element> Factory(bool value) {
        return std::unique_ptr<Element>(new Primitive(value));
    }



    int64_t Header::startSerialize() {
        return -getHeaderLength();
    }

    int64_t Header::startDeserialize(uint8_t firstByte) {
        header[0] = firstByte;
        return 1-getHeaderLength();
    }

    std::streamsize Header::serialize(int64_t& pos, std::basic_streambuf<char>* streamBuffer, std::streamsize bytes) {
        if(pos < 0) {
            bytes = std::min(bytes, (std::streamsize)(-pos));
            bytes = streamBuffer->sputn(header+getHeaderLength()+pos, bytes);
            pos += bytes;
            return bytes;
        }else
            return 0;
    }

    std::streamsize Header::deserialize(int64_t& pos, std::basic_streambuf<char>* streamBuffer, std::streamsize bytes) {
        if(pos < 0) {
            bytes = std::min(bytes, (std::streamsize)(-pos));
            bytes = streamBuffer->sgetn(header+getHeaderLength()+pos, bytes);
            pos += bytes;
            return bytes;
        }else
            return 0;
    }

    int64_t Header::getEndPos() const {
        return 0;
    }

    Type Header::getType() const {
        uint8_t type = static_cast<const uint8_t>(header[0]);
        if(type < FIXARRAY) return FIXMAP;
        if(type < FIXSTR) return FIXARRAY;
        if(type < NIL) return FIXSTR;
        return static_cast<Type>(type);
    }

    uint32_t Header::getSizeInBytes() const {
        return getHeaderLength()+getEndPos();
    }

    uint32_t Header::getLength() const {
        return getEndPos();
    }



    std::streamsize Data::serialize(int64_t& pos, std::basic_streambuf<char>* streamBuffer, std::streamsize bytes) {
        std::streamsize bytesDone = Header::serialize(pos, streamBuffer, bytes);

        if(pos >= 0 && data) {
            bytes = std::min(bytes, (std::streamsize)(getEndPos()-pos));
            bytes = streamBuffer->sputn(data.get()+pos, bytes);
            pos += bytes;
            bytesDone += bytes;
        }

        return bytesDone;
    }

    std::streamsize Data::deserialize(int64_t& pos, std::basic_streambuf<char>* streamBuffer, std::streamsize bytes) {
        std::streamsize bytesDone = Header::deserialize(pos, streamBuffer, bytes);

        if(pos >= 0) {
            int64_t dataLen = getEndPos();
            if(dataLen > 0) {
                if(!data)
                    data.reset(new char[dataLen]);

                bytes = std::min(bytes, (std::streamsize)(dataLen-pos));
                bytes = streamBuffer->sgetn(data.get()+pos, bytes);
                pos += bytes;
                bytesDone += bytes;
            }
        }

        return bytesDone;
    }



    Binary::Binary(uint32_t len, const void* _data) {
        if(len > 0) {
            data.reset(new char[len]);
            memcpy(data.get(), _data, len);
        }

        if(len <= 0xFF) {
            header[0] = Type::BIN_8;
            storeUint8(&header[1], len);
        }else if(len <= 0xFFFF) {
            header[0] = Type::BIN_16;
            storeUint16(&header[1], len);
        }else{
            header[0] = Type::BIN_32;
            storeUint32(&header[1], len);
        }
    }

    int64_t Binary::getEndPos() const {
        uint8_t type = static_cast<const uint8_t>(header[0]);
        switch(type) {
            case Type::BIN_8:
                return loadUint8(&header[1]);
            case Type::BIN_16:
                return loadUint16(&header[1]);
            case Type::BIN_32:
                return loadUint32(&header[1]);
            default:
                return 0;
        }
    }

    int64_t Binary::getHeaderLength() const {
        uint8_t type = static_cast<const uint8_t>(header[0]);
        switch(type) {
            case Type::BIN_8:
                return 2;
            case Type::BIN_16:
                return 3;
            case Type::BIN_32:
                return 5;
            default:
                return 0;
        }
    }

    std::unique_ptr<Element> Binary::copy() const {
        auto len = getLength();
        auto _data = new char[len];
        memcpy(_data, getData(), len);
        return std::unique_ptr<Element>(new Binary(len, _data));
    }

    void Binary::toJSON(std::ostream& stream) const {
        auto len = getLength();
        stream << "< Buffer length=" << len << " data: ";
        for(uint32_t i = 0; i < len; i ++)
            stream << (uint16_t)data[i] << " ";
        stream << ">";
    }

    char* Binary::getData() const {
        return data.get();
    }



    Extended::Extended(uint8_t type, uint32_t len, const void* _data) {
        data.reset(new char[len+1]);
        data[0] = type;
        memcpy(&data[1], _data, len);

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
                    header[0] = Type::EXT_8;
                    storeUint8(&header[1], len);
                }else if(len <= 0xFFFF) {
                    header[0] = Type::EXT_16;
                    storeUint16(&header[1], len);
                }else{
                    header[0] = Type::EXT_32;
                    storeUint32(&header[1], len);
                }
            break;
        }
    }

    int64_t Extended::getEndPos() const {
        uint8_t type = static_cast<const uint8_t>(header[0]);
        switch(type) {
            case Type::FIXEXT_8:
                return 2;
            case Type::FIXEXT_16:
                return 3;
            case Type::FIXEXT_32:
                return 5;
            case Type::FIXEXT_64:
                return 9;
            case Type::FIXEXT_128:
                return 17;
            case Type::EXT_8:
                return loadUint8(&header[1])+1;
            case Type::EXT_16:
                return loadUint16(&header[1])+1;
            case Type::EXT_32:
                return loadUint32(&header[1])+1;
            default:
                return 0;
        }
    }

    int64_t Extended::getHeaderLength() const {
        uint8_t type = static_cast<const uint8_t>(header[0]);
        switch(type) {
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
            default:
                return 0;
        }
    }

    std::unique_ptr<Element> Extended::copy() const {
        auto len = getLength();
        auto _data = new char[len];
        memcpy(_data, getData(), len);
        return std::unique_ptr<Element>(new Extended(getDataType(), len, _data));
    }

    void Extended::toJSON(std::ostream& stream) const {
        auto len = getLength();
        stream << "< Extended type=" << (uint16_t)data[0];
        stream << " length=" << (len-1) << " data: ";
        for(uint32_t i = 1; i < len; i ++)
            stream << (uint16_t)data[i] << " ";
        stream << ">";
    }

    uint8_t Extended::getDataType() const {
        return data[0];
    }

    char* Extended::getData() const {
        return &data[1];
    }

    uint32_t Extended::getLength() const {
        return getEndPos()-1;
    }



    void String::init(uint32_t len, const void* str) {
        if(len > 0) {
            data.reset(new char[len]);
            memcpy(data.get(), str, len);
        }

        if(len < 0x20)
            header[0] = Type::FIXSTR+len;
        else if(len <= 0xFF) {
            header[0] = Type::STR_8;
            storeUint8(&header[1], len);
        }else if(len <= 0xFFFF) {
            header[0] = Type::STR_16;
            storeUint16(&header[1], len);
        }else{
            header[0] = Type::STR_32;
            storeUint32(&header[1], len);
        }
    }

    String::String(uint32_t len, const void* str) {
        init(len, str);
    }

    String::String(const char* str) {
        init(strlen(str), str);
    }

    String::String(const std::string& str) {
        init(str.size(), str.c_str());
    }

    int64_t String::getEndPos() const {
        uint8_t type = static_cast<const uint8_t>(header[0]);
        if(type >= Type::FIXSTR && type < Type::NIL)
            return type - Type::FIXSTR;

        switch(type) {
            case Type::STR_8:
                return loadUint8(&header[1]);
            case Type::STR_16:
                return loadUint16(&header[1]);
            case Type::STR_32:
                return loadUint32(&header[1]);
            default:
                return 0;
        }
    }

    int64_t String::getHeaderLength() const {
        uint8_t type = static_cast<const uint8_t>(header[0]);
        if(type >= Type::FIXSTR && type < Type::NIL)
            return 1;

        switch(type) {
            case Type::STR_8:
                return 2;
            case Type::STR_16:
                return 3;
            case Type::STR_32:
                return 5;
            default:
                return 0;
        }
    }

    std::unique_ptr<Element> String::copy() const {
        auto len = getLength();
        auto _data = new char[len];
        memcpy(_data, getData(), len);
        return std::unique_ptr<Element>(new String(len, _data));
    }

    void String::toJSON(std::ostream& stream) const {
        stream << "\"";
		stream.write(reinterpret_cast<const char*>(data.get()), getEndPos());
        stream << "\"";
    }

    char* String::getData() const {
        return data.get();
    }

    std::string String::stdString() const {
        return std::string(reinterpret_cast<const char*>(data.get()), getEndPos());
    }

    std::unique_ptr<Element> Factory(const char* str) {
        return std::unique_ptr<Element>(new String(str));
    }

    std::unique_ptr<Element> Factory(const std::string& str) {
        return std::unique_ptr<Element>(new String(str));
    }



    Number::Number(uint64_t value) {
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

    Number::Number(int64_t value) {
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

    Number::Number(float value) {
        data[0] = Type::FLOAT_32;
        storeFloat32(data+1, value);
    }

    Number::Number(double value) {
        data[0] = Type::FLOAT_64;
        storeFloat64(data+1, value);
    }

    int64_t Number::startDeserialize(uint8_t firstByte) {
        data[0] = firstByte;
        return 1;
    }

    std::streamsize Number::serialize(int64_t& pos, std::basic_streambuf<char>* streamBuffer, std::streamsize bytes) {
        bytes = std::min(bytes, (std::streamsize)(getEndPos()-pos));
        bytes = streamBuffer->sputn(data+pos, bytes);
        pos += bytes;
        return bytes;
    }

    std::streamsize Number::deserialize(int64_t& pos, std::basic_streambuf<char>* streamBuffer, std::streamsize bytes) {
        bytes = std::min(bytes, (std::streamsize)(getEndPos()-pos));
        bytes = streamBuffer->sgetn(data+pos, bytes);
        pos += bytes;
        return bytes;
    }

    int64_t Number::getEndPos() const {
        uint8_t type = static_cast<const uint8_t>(data[0]);
        if(type < 0x80 || type >= 0xE0)
            return 1;

        switch(type) {
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
            default:
                return 0;
        }
    }

    std::unique_ptr<Element> Number::copy() const {
        auto result = new Number();
        memcpy(result->data, data, sizeof(data));
        return std::unique_ptr<Element>(result);
    }

    void Number::toJSON(std::ostream& stream) const {
        uint8_t type = static_cast<const uint8_t>(data[0]);
        if(type < FIXMAP)
            stream << (int16_t)type;
        else if(type >= FIXINT)
            stream << (int16_t)static_cast<const int8_t>(type);
        else
            switch(type) {
                case Type::FLOAT_32:
                    stream << loadFloat32(data+1);
                break;
                case Type::FLOAT_64:
                    stream << loadFloat64(data+1);
                break;
                case Type::UINT_8:
                    stream << (uint16_t)loadUint8(data+1);
                break;
                case Type::UINT_16:
                    stream << loadUint16(data+1);
                break;
                case Type::UINT_32:
                    stream << loadUint32(data+1);
                break;
                case Type::UINT_64:
                    stream << loadUint64(data+1);
                break;
                case Type::INT_8:
                    stream << (int16_t)loadInt8(data+1);
                break;
                case Type::INT_16:
                    stream << loadInt16(data+1);
                break;
                case Type::INT_32:
                    stream << loadInt32(data+1);
                break;
                case Type::INT_64:
                    stream << loadInt64(data+1);
                break;
            }
    }

    Type Number::getType() const {
        uint8_t type = static_cast<const uint8_t>(data[0]);
        if(type < FIXMAP) return FIXUINT;
        if(type >= FIXINT) return FIXINT;
        return static_cast<Type>(type);
    }

    bool Number::isUnsignedInteger() const {
        uint8_t type = static_cast<const uint8_t>(data[0]);
        return (type < FIXMAP || (type >= UINT_8 && type <= UINT_64));
    }

    bool Number::isSignedInteger() const {
        uint8_t type = static_cast<const uint8_t>(data[0]);
        return (type >= FIXINT || (type >= INT_8 && type <= INT_64));
    }

    bool Number::isFloatingPoint() const {
        uint8_t type = static_cast<const uint8_t>(data[0]);
        return (type == FLOAT_32 || type == FLOAT_64);
    }

    std::unique_ptr<Element> Factory(uint64_t value) {
        return std::unique_ptr<Element>(new Number(value));
    }

    std::unique_ptr<Element> Factory(int64_t value) {
        return std::unique_ptr<Element>(new Number(value));
    }

    std::unique_ptr<Element> Factory(float value) {
        return std::unique_ptr<Element>(new Number(value));
    }

    std::unique_ptr<Element> Factory(double value) {
        return std::unique_ptr<Element>(new Number(value));
    }



    ArrayHeader::ArrayHeader(uint32_t len) {
        if(len < 0x10)
            header[0] = Type::FIXARRAY+len;
        else if(len <= 0xFFFF) {
            header[0] = Type::ARRAY_16;
            storeUint16(&header[1], len);
        }else{
            header[0] = Type::ARRAY_32;
            storeUint32(&header[1], len);
        }
    }

    uint32_t ArrayHeader::getLength() const {
        uint8_t type = static_cast<const uint8_t>(header[0]);
        if(type >= Type::FIXARRAY && type < Type::FIXSTR)
            return type - Type::FIXARRAY;

        switch(type) {
            case Type::ARRAY_16:
                return loadUint16(&header[1]);
            case Type::ARRAY_32:
                return loadUint32(&header[1]);
            default:
                return 0;
        }
    }

    std::unique_ptr<Element> ArrayHeader::copy() const {
        return std::unique_ptr<Element>(new ArrayHeader(getLength()));
    }

    void ArrayHeader::toJSON(std::ostream& stream) const {
        stream << "< Array length=" << getLength() << " >";
    }

    uint32_t ArrayHeader::getSizeInBytes() const {
        return getHeaderLength();
    }

    int64_t ArrayHeader::getHeaderLength() const {
        uint8_t type = static_cast<const uint8_t>(header[0]);
        if(type >= Type::FIXARRAY && type < Type::FIXSTR)
            return 1;

        switch(type) {
            case Type::ARRAY_16:
                return 3;
            case Type::ARRAY_32:
                return 5;
            default:
                return 0;
        }
    }



    MapHeader::MapHeader(uint32_t len) {
        if(len < 0x10)
            header[0] = Type::FIXMAP+len;
        else if(len <= 0xFFFF) {
            header[0] = Type::MAP_16;
            storeUint16(&header[1], len);
        }else{
            header[0] = Type::MAP_32;
            storeUint32(&header[1], len);
        }
    }

    int64_t MapHeader::getHeaderLength() const {
        uint8_t type = static_cast<const uint8_t>(header[0]);
        if(type >= Type::FIXMAP && type < Type::FIXARRAY)
            return 1;

        switch(type) {
            case Type::MAP_16:
                return 3;
            case Type::MAP_32:
                return 5;
            default:
                return 0;
        }
    }

    std::unique_ptr<Element> MapHeader::copy() const {
        return std::unique_ptr<Element>(new MapHeader(getLength()));
    }

    void MapHeader::toJSON(std::ostream& stream) const {
        stream << "< Map length=" << getLength() << " >";
    }

    uint32_t MapHeader::getSizeInBytes() const {
        return getHeaderLength();
    }

    uint32_t MapHeader::getLength() const {
        uint8_t type = static_cast<const uint8_t>(header[0]);
        if(type >= Type::FIXMAP && type < Type::FIXARRAY)
            return type - Type::FIXMAP;

        switch(type) {
            case Type::MAP_16:
                return loadUint16(&header[1]);
            case Type::MAP_32:
                return loadUint32(&header[1]);
            default:
                return 0;
        }
    }



	Array::Array(std::vector<std::unique_ptr<Element>>&& _elements)
        : ArrayHeader(_elements.size()), elements(std::move(_elements)) {

    }

    bool Array::containerDeserialized() {
        uint32_t len = getLength();
        if(len > 0) {
            elements = std::vector<std::unique_ptr<Element>>(len);
            return true;
        }else
            return false;
    }

    std::unique_ptr<Element> Array::copy() const {
        auto len = getLength();
        std::vector<std::unique_ptr<Element>> _elements;
        _elements.reserve(len);
        for(const auto& i : elements)
            _elements.push_back(std::unique_ptr<Element>(i->copy()));
        return std::unique_ptr<Element>(new Array(std::move(_elements)));
    }

    void Array::toJSON(std::ostream& stream) const {
        uint32_t len = elements.size();
        stream << "[";
        if(len > 0) {
            elements[0]->toJSON(stream);
            for(uint32_t i = 1; i < len; i ++) {
                stream << ", ";
                elements[i]->toJSON(stream);
            }
        }
        stream << "]";
    }

    uint32_t Array::getSizeInBytes() const {
        int64_t size = getHeaderLength();
        uint32_t len = elements.size();
        for(uint32_t i = 1; i < len; i ++)
            size += elements[i]->getSizeInBytes();
        return size;
    }

    std::vector<std::unique_ptr<Element>>* Array::getElementsVector() {
        return &elements;
    }

    Element* Array::getEntry(uint32_t index) const {
        return (index < elements.size()) ? elements[index].get() : nullptr;
    }



    Map::Map(std::vector<std::unique_ptr<Element>>&& _elements)
        : MapHeader(_elements.size()/2), elements(std::move(_elements)) {
        if(elements.size()%2 == 1)
            elements.erase(elements.end()-1);
    }

    Map::Map(std::map<std::string, std::unique_ptr<Element>>&& _elements)
        : MapHeader(_elements.size()) {
        for(auto& pair : _elements) {
            elements.push_back(std::unique_ptr<Element>(new MsgPack::String(pair.first)));
            elements.push_back(std::move(pair.second));
        }
    }

    bool Map::containerDeserialized() {
        uint32_t len = getLength();
        if(len > 0) {
            elements = std::vector<std::unique_ptr<Element>>(len*2);
            return true;
        }else
            return false;
    }

    std::unique_ptr<Element> Map::copy() const {
        auto len = getLength();
        std::vector<std::unique_ptr<Element>> _elements;
        _elements.reserve(len);
        for(const auto& i : elements)
            _elements.push_back(std::unique_ptr<Element>(i->copy()));
        return std::unique_ptr<Element>(new Map(std::move(_elements)));
    }

    void Map::toJSON(std::ostream& stream) const {
        uint64_t len = elements.size();
        stream << "{";
        if(len > 0) {
            elements[0]->toJSON(stream);
            stream << ": ";
            elements[1]->toJSON(stream);
            for(uint64_t i = 2; i < len; i += 2) {
                stream << ", ";
                elements[i]->toJSON(stream);
                stream << ": ";
                elements[i+1]->toJSON(stream);
            }
        }
        stream << "}";
    }

    uint32_t Map::getSizeInBytes() const {
        int64_t size = getHeaderLength();
        uint32_t len = elements.size();
        for(uint32_t i = 1; i < len; i ++)
            size += elements[i]->getSizeInBytes();
        return size;
    }

    std::vector<std::unique_ptr<Element>>* Map::getElementsVector() {
        return &elements;
    }

    std::map<std::string, Element*> Map::getElementsMap() const {
        std::map<std::string, Element*> map;
        for(size_t index = 0; index < elements.size(); index += 2) {
            String* key = dynamic_cast<String*>(elements[index].get());
            if(index+1 == elements.size()) break;
            Element* value = elements[index+1].get();
            map.insert(std::pair<std::string, Element*>(key->stdString(), value));
        }
        return map;
    }

    std::pair<String*, Element*> Map::getEntry(uint32_t index) const {
        index = index*2;
        if(index+1 < elements.size()) {
            auto key = dynamic_cast<String*>(elements[index].get());
            if(key)
                return std::pair<String*, Element*>(key, elements[index+1].get());
        }
        return std::pair<String*, Element*>(nullptr, nullptr);
    }

    String* Map::getKey(uint32_t index) const {
        index = index*2;
        return (index < elements.size()) ? dynamic_cast<String*>(elements[index].get()) : nullptr;
    }

    Element* Map::getValue(uint32_t index) const {
        index = index*2+1;
        return (index < elements.size()) ? elements[index].get() : nullptr;
    }



    std::streamsize Serializer::serialize(PullCallback pullElement, std::streamsize bytesLeft) {
        bool deserializeAll = (bytesLeft == 0);
        std::streamsize bytesDone = 0;

        while(bytesLeft > 0 || deserializeAll) {
            if(deserializeAll) bytesLeft = LONG_MAX;

            //Try to pull next element if necessary
            if(stack.size() == 0) {
                rootElement = pullElement();

                if(!rootElement)
                    break; //Got no element: quit

                stack.push_back(StackElement(rootElement.get(), rootElement->startSerialize()));
            }

            //Find highest element in stack
            StackElement* stackPointer = &stack[stack.size()-1];

            //Serialize element
            std::streamsize bytesWritten = stackPointer->first->serialize(stackPointer->second, streamBuffer, bytesLeft);
            bytesLeft -= bytesWritten;
            bytesDone += bytesWritten;

            if(bytesWritten == 0)
                break; //Stream overflow: quit

            if(stackPointer->second < stackPointer->first->getEndPos())
                continue; //Not done yet

            //Finish element
            std::vector<std::unique_ptr<Element>>* container = stackPointer->first->getElementsVector();
            if(container && container->size() > 0) {
                //Serialized header, begin with first child
                Element* childElement = container->begin()->get();
                stack.push_back(StackElement(childElement, childElement->startSerialize()));
                continue;
            }

            //Find lowest done container in stack
            uint32_t stackIndex = stack.size()-1;
            while(true) {
                stackPointer = &stack[stackIndex];
                container = stackPointer->first->getElementsVector();
                if(container && stackPointer->second+1 < (int64_t)container->size()) {
                    //Container is not done yet. move to next element
                    int64_t pos = ++ stackPointer->second;
                    stackPointer = &stack[++ stackIndex];
                    stackPointer->first = (container->begin()+pos)->get();
                    stackPointer->second = stackPointer->first->startSerialize();
                    stackIndex ++;
                    break;
                }
                //Don't drop below stackIndex = 0
                if(stackIndex == 0) break;
                stackIndex --;
            }

            //Pop all done containers from stack
            stack.erase(stack.begin()+stackIndex, stack.end());

            //Check if root element is done
            if(stackIndex == 0)
                rootElement.reset();
        }

        return bytesDone;
    }

    std::streamsize Serializer::serialize(std::unique_ptr<Element>& element, std::streamsize bytesLeft) {
        return (element) ?
        serialize([&element]() {
            return std::move(element);
        }, bytesLeft)
        : 0;
    }



    std::streamsize Deserializer::deserialize(PushCallback pushElement, bool hierarchy, std::streamsize bytesLeft) {
        bool deserializeAll = (bytesLeft == 0);
        std::streamsize bytesDone = 0;

        while(bytesLeft > 0 || deserializeAll) {
            if(deserializeAll) bytesLeft = LONG_MAX;

            //Find highest element in stack
            StackElement* stackPointer = (stack.size() > 0) ? &stack[stack.size()-1] : NULL;

            if(stackPointer && stackPointer->second < stackPointer->first->getEndPos()) {
                //Deserialize element
                std::streamsize bytesRead = stackPointer->first->deserialize(stackPointer->second, streamBuffer, bytesLeft);
                bytesLeft -= bytesRead;
                bytesDone += bytesRead;

                if(bytesRead == 0)
                    break; //Stream underflow: quit

                if(stackPointer->second < stackPointer->first->getEndPos())
                    continue; //Not done yet

                if(stackPointer->first->containerDeserialized())
                    continue;
            }else{
                //Deserialize next element
                int read = streamBuffer->sbumpc();
                if(read == EOF) break;
                bytesLeft --;
                bytesDone ++;

                Element* element;
                uint8_t nextByte = static_cast<uint8_t>(read);
                if(nextByte < Type::FIXMAP || nextByte >= Type::FIXINT)
                    element = new Number();
                else if(nextByte < Type::FIXARRAY)
                    element = (hierarchy) ? new Map() : new MapHeader();
                else if(nextByte < Type::FIXSTR)
                    element = (hierarchy) ? new Array() : new ArrayHeader();
                else if(nextByte < Type::NIL)
                    element = new String();
                else
                    switch(nextByte) {
                        case Type::NIL:
						case Type::UNDEFINED:
                        case Type::BOOL_FALSE:
                        case Type::BOOL_TRUE:
                            element = new Primitive();
                        break;
                        case Type::BIN_8:
                        case Type::BIN_16:
                        case Type::BIN_32:
                            element = new Binary();
                        break;
                        case Type::EXT_8:
                        case Type::EXT_16:
                        case Type::EXT_32:
                        case Type::FIXEXT_8:
                        case Type::FIXEXT_16:
                        case Type::FIXEXT_32:
                        case Type::FIXEXT_64:
                        case Type::FIXEXT_128:
                            element = new Extended();
                        break;
                        case Type::STR_8:
                        case Type::STR_16:
                        case Type::STR_32:
                            element = new String();
                        break;
                        case Type::ARRAY_16:
                        case Type::ARRAY_32:
                            element = (hierarchy) ? new Array() : new ArrayHeader();
                        break;
                        case Type::MAP_16:
                        case Type::MAP_32:
                            element = (hierarchy) ? new Map() : new MapHeader();
                        break;
                        default:
                            element = new Number();
                        break;
                    }

                //Put element in parent container
                if(stack.size() > 0) {
                    std::vector<std::unique_ptr<Element>>* container = stackPointer->first->getElementsVector();
                    *(container->begin()+stackPointer->second) = std::move(std::unique_ptr<Element>(element));
                }else
                    rootElement.reset(element);

                //Check if element is done
                int64_t pos = element->startDeserialize(nextByte);
                if(pos < element->getEndPos() || element->containerDeserialized()) {
                    stack.push_back(StackElement(element, pos));
                    continue;
                }
            }

            //Element done
            if(stack.size() > 0) {
                //Pop all done containers from stack
                uint32_t stackIndex = stack.size()-1;
                std::vector<std::unique_ptr<Element>>* container;
                while(true) {
                    container = stack[stackIndex].first->getElementsVector();
                    if(container && stack[stackIndex].second+1 < (int64_t)container->size()) {
                        stack[stackIndex ++].second ++;
                        break;
                    }
                    if(stackIndex == 0) break;
                    stackIndex --;
                }

                stack.erase(stack.begin()+stackIndex, stack.end());
            }

            //Trigger event for done element
            if(stack.size() == 0 && pushElement(std::move(rootElement)))
                break; //One element done: quit
        }

        return bytesDone;
    }

    std::streamsize Deserializer::deserialize(std::unique_ptr<Element>& element, bool hierarchy, std::streamsize bytesLeft) {
        return deserialize([&element](std::unique_ptr<Element> parsedElement) {
            element = std::move(parsedElement);
            return true;
        }, hierarchy, bytesLeft);
    }



    std::ostream& operator<<(std::ostream& ostream, const Element& obj) {
        obj.toJSON(ostream);
        return ostream;
    }
};
