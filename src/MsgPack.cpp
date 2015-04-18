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

#ifdef WIN32
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

inline void storeUint8(uint8_t* target, uint8_t source) {
    *reinterpret_cast<uint8_t*>(target) = source;
}

inline void storeInt8(uint8_t* target, int8_t source) {
    *reinterpret_cast<int8_t*>(target) = source;
}

inline void storeUint16(uint8_t* target, uint16_t source) {
    storeBSwap(16);
}

inline void storeInt16(uint8_t* target, int16_t source) {
    storeBSwap(16);
}

inline void storeFloat32(uint8_t* target, float source) {
    storeBSwap(32);
}

inline void storeUint32(uint8_t* target, uint32_t source) {
    storeBSwap(32);
}

inline void storeInt32(uint8_t* target, int32_t source) {
    storeBSwap(32);
}

inline void storeFloat64(uint8_t* target, double source) {
    storeBSwap(64);
}

inline void storeUint64(uint8_t* target, uint64_t source) {
    storeBSwap(64);
}

inline void storeInt64(uint8_t* target, int64_t source) {
    storeBSwap(64);
}



inline uint8_t loadUint8(const uint8_t* source) {
    return *reinterpret_cast<const uint8_t*>(source);
}

inline int8_t loadInt8(const uint8_t* source) {
    return *reinterpret_cast<const int8_t*>(source);
}

inline uint16_t loadUint16(const uint8_t* source) {
    uint16_t target;
    loadBSwap(16);
}

inline int16_t loadInt16(const uint8_t* source) {
    int16_t target;
    loadBSwap(16);
}

inline float loadFloat32(const uint8_t* source) {
    float target;
    loadBSwap(32);
}

inline uint32_t loadUint32(const uint8_t* source) {
    uint32_t target;
    loadBSwap(32);
}

inline int32_t loadInt32(const uint8_t* source) {
    int32_t target;
    loadBSwap(32);
}

inline double loadFloat64(const uint8_t* source) {
    double target;
    loadBSwap(64);
}

inline uint64_t loadUint64(const uint8_t* source) {
    uint64_t target;
    loadBSwap(64);
}

inline int64_t loadInt64(const uint8_t* source) {
    int64_t target;
    loadBSwap(64);
}



namespace MsgPack {


    Primitive::Primitive(Type _type) : type(_type) {

    }

    Primitive::Primitive(bool value) : Primitive((value) ? Type::BOOL_TRUE : Type::BOOL_FALSE) {

    }

    int64_t Primitive::startDeserialize(std::basic_streambuf<uint8_t>* streamBuffer) {
        type = (uint8_t)streamBuffer->sbumpc();
        return 1;
    }

    std::streamsize Primitive::serialize(int64_t& pos, std::basic_streambuf<uint8_t>* streamBuffer, std::streamsize bytes) {
        if(bytes > 0 && pos == 0 && streamBuffer->sputc(type) >= 0)
            return (++ pos);
        else
            return 0;
    }

    int64_t Primitive::getEndPos() const {
        return 1;
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

    int64_t Header::startDeserialize(std::basic_streambuf<uint8_t>* streamBuffer) {
        header[0] = (uint8_t)streamBuffer->sbumpc();
        return 1-getHeaderLength();
    }

    std::streamsize Header::serialize(int64_t& pos, std::basic_streambuf<uint8_t>* streamBuffer, std::streamsize bytes) {
        if(pos < 0) {
            bytes = std::min(bytes, (std::streamsize)(-pos));
            bytes = streamBuffer->sputn(header+getHeaderLength()+pos, bytes);
            pos += bytes;
            return bytes;
        }else
            return 0;
    }

    std::streamsize Header::deserialize(int64_t& pos, std::basic_streambuf<uint8_t>* streamBuffer, std::streamsize bytes) {
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
        return (Type)header[0];
    }

    uint32_t Header::getSizeInBytes() const {
        return getHeaderLength()+getEndPos();
    }

    uint32_t Header::getLength() const {
        return getEndPos();
    }



    std::streamsize Data::serialize(int64_t& pos, std::basic_streambuf<uint8_t>* streamBuffer, std::streamsize bytes) {
        std::streamsize bytesDone = Header::serialize(pos, streamBuffer, bytes);

        if(pos >= 0 && data) {
            bytes = std::min(bytes, (std::streamsize)(getEndPos()-pos));
            bytes = streamBuffer->sputn(data.get()+pos, bytes);
            pos += bytes;
            bytesDone += bytes;
        }

        return bytesDone;
    }

    std::streamsize Data::deserialize(int64_t& pos, std::basic_streambuf<uint8_t>* streamBuffer, std::streamsize bytes) {
        std::streamsize bytesDone = Header::deserialize(pos, streamBuffer, bytes);

        if(pos >= 0) {
            int64_t dataLen = getEndPos();
            if(dataLen > 0) {
                if(!data)
                    data.reset(new uint8_t[dataLen]);

                bytes = std::min(bytes, (std::streamsize)(dataLen-pos));
                bytes = streamBuffer->sgetn(data.get()+pos, bytes);
                pos += bytes;
                bytesDone += bytes;
            }
        }

        return bytesDone;
    }



    Binary::Binary(uint32_t len, const uint8_t* _data) {
        if(len > 0) {
            data.reset(new uint8_t[len]);
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
        switch(header[0]) {
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
        switch(header[0]) {
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

    void Binary::toJSON(std::ostream& stream) const {
        uint32_t len = getEndPos();
        stream << "< Buffer length=" << len << " data: ";
        for(uint32_t i = 0; i < len; i ++)
            stream << (uint16_t)data[i] << " ";
        stream << ">";
    }

    uint8_t* Binary::getData() const {
        return data.get();
    }



    Extended::Extended(uint8_t type, uint32_t len, const uint8_t* _data) {
        data.reset(new uint8_t[len+1]);
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
        switch(header[0]) {
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
            default:
                return 0;
        }
    }

    void Extended::toJSON(std::ostream& stream) const {
        uint32_t len = getEndPos();
        stream << "< Extended type=" << (uint16_t)data[0];
        stream << " length=" << (len-1) << " data: ";
        for(uint32_t i = 1; i < len; i ++)
            stream << (uint16_t)data[i] << " ";
        stream << ">";
    }

    uint8_t Extended::getDataType() const {
        return data[0];
    }

    uint8_t* Extended::getData() const {
        return &data[1];
    }

    uint32_t Extended::getLength() const {
        return getEndPos()-1;
    }



    String::String(const char* str, uint32_t len) {
        if(len > 0) {
            data.reset(new uint8_t[len]);
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

	String::String(const char* str) :String(str, strlen(str)) {

    }

	String::String(const std::string& str) :String(str.c_str(), str.size()) {

    }

    int64_t String::getEndPos() const {
        if(header[0] >= Type::FIXSTR && header[0] < Type::NIL)
            return header[0] - Type::FIXSTR;

        switch(header[0]) {
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
        if(header[0] >= Type::FIXSTR && header[0] < Type::NIL)
            return 1;

        switch(header[0]) {
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

    void String::toJSON(std::ostream& stream) const {
        stream << "\"";
		stream.write(reinterpret_cast<const char*>(data.get()), getEndPos());
        stream << "\"";
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

    int64_t Number::startDeserialize(std::basic_streambuf<uint8_t>* streamBuffer) {
        data[0] = (uint8_t)streamBuffer->sbumpc();
        return 1;
    }

    std::streamsize Number::serialize(int64_t& pos, std::basic_streambuf<uint8_t>* streamBuffer, std::streamsize bytes) {
        bytes = std::min(bytes, (std::streamsize)(getEndPos()-pos));
        bytes = streamBuffer->sputn(data+pos, bytes);
        pos += bytes;
        return bytes;
    }

    std::streamsize Number::deserialize(int64_t& pos, std::basic_streambuf<uint8_t>* streamBuffer, std::streamsize bytes) {
        bytes = std::min(bytes, (std::streamsize)(getEndPos()-pos));
        bytes = streamBuffer->sgetn(data+pos, bytes);
        pos += bytes;
        return bytes;
    }

    int64_t Number::getEndPos() const {
        if(data[0] < 0x80 || data[0] >= 0xE0)
            return 1;

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
            default:
                return 0;
        }
    }

    void Number::toJSON(std::ostream& stream) const {
            if(data[0] < 0x80 || data[0] >= 0xE0)
                stream << (int16_t)reinterpret_cast<const int8_t&>(data[0]);
            else
                switch(data[0]) {
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
        return (Type)data[0];
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
        if(header[0] >= Type::FIXARRAY && header[0] < Type::FIXSTR)
            return header[0] - Type::FIXARRAY;

        switch(header[0]) {
            case Type::ARRAY_16:
                return loadUint16(&header[1]);
            case Type::ARRAY_32:
                return loadUint32(&header[1]);
            default:
                return 0;
        }
    }

    int64_t ArrayHeader::getHeaderLength() const {
        if(header[0] >= Type::FIXARRAY && header[0] < Type::FIXSTR)
            return 1;

        switch(header[0]) {
            case Type::ARRAY_16:
                return 3;
            case Type::ARRAY_32:
                return 5;
            default:
                return 0;
        }
    }

    void ArrayHeader::toJSON(std::ostream& stream) const {
        stream << "< Array length=" << getLength() << " >";
    }

    uint32_t ArrayHeader::getSizeInBytes() const {
        return getHeaderLength();
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

    uint32_t MapHeader::getLength() const {
        if(header[0] >= Type::FIXMAP && header[0] < Type::FIXARRAY)
            return header[0] - Type::FIXMAP;

        switch(header[0]) {
            case Type::MAP_16:
                return loadUint16(&header[1]);
            case Type::MAP_32:
                return loadUint32(&header[1]);
            default:
                return 0;
        }
    }

    int64_t MapHeader::getHeaderLength() const {
        if(header[0] >= Type::FIXMAP && header[0] < Type::FIXARRAY)
            return 1;

        switch(header[0]) {
            case Type::MAP_16:
                return 3;
            case Type::MAP_32:
                return 5;
            default:
                return 0;
        }
    }

    void MapHeader::toJSON(std::ostream& stream) const {
        stream << "< Map length=" << getLength() << " >";
    }

    uint32_t MapHeader::getSizeInBytes() const {
        return getHeaderLength();
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

    std::vector<std::unique_ptr<Element>>* Array::getContainer() {
        return &elements;
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



    Map::Map(std::vector<std::unique_ptr<Element>>&& _elements)
        : MapHeader(_elements.size()/2), elements(std::move(_elements)) {
        if(elements.size()%2 == 1)
            elements.erase(elements.end()-1);
    }

    bool Map::containerDeserialized() {
        uint32_t len = getLength();
        if(len > 0) {
            elements = std::vector<std::unique_ptr<Element>>(len*2);
            return true;
        }else
            return false;
    }

    std::vector<std::unique_ptr<Element>>* Map::getContainer() {
        return &elements;
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
            std::vector<std::unique_ptr<Element>>* container = stackPointer->first->getContainer();
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
                container = stackPointer->first->getContainer();
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
                int read = streamBuffer->sgetc();
                if(read < 0) break;
                Element* element;
                uint8_t nextByte = (uint8_t)read;
                bytesLeft --;
                bytesDone ++;

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
                    std::vector<std::unique_ptr<Element>>* container = stackPointer->first->getContainer();
                    *(container->begin()+stackPointer->second) = std::move(std::unique_ptr<Element>(element));
                }else
                    rootElement.reset(element);

                //Check if element is done
                int64_t pos = element->startDeserialize(streamBuffer);
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
                    container = stack[stackIndex].first->getContainer();
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
