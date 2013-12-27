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
    #if BIG_ENDIAN
    *reinterpret_cast<uint16_t*>(target) = source;
    #else
    *reinterpret_cast<uint16_t*>(target) = __builtin_bswap16(source);
    #endif
}

void storeInt16(uint8_t* target, int16_t source) {
    #if BIG_ENDIAN
    *reinterpret_cast<int16_t*>(target) = source;
    #else
    *reinterpret_cast<int16_t*>(target) = __builtin_bswap16(source);
    #endif
}

void storeFloat32(uint8_t* target, float source) {
    #if BIG_ENDIAN
    *reinterpret_cast<float*>(target) = source;
    #else
    *reinterpret_cast<float*>(target) = __builtin_bswap32(source);
    #endif
}

void storeUint32(uint8_t* target, uint32_t source) {
    #if BIG_ENDIAN
    *reinterpret_cast<uint32_t*>(target) = source;
    #else
    *reinterpret_cast<uint32_t*>(target) = __builtin_bswap32(source);
    #endif
}

void storeInt32(uint8_t* target, int32_t source) {
    #if BIG_ENDIAN
    *reinterpret_cast<int32_t*>(target) = source;
    #else
    *reinterpret_cast<int32_t*>(target) = __builtin_bswap32(source);
    #endif
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
    #if BIG_ENDIAN
    return *reinterpret_cast<const uint8_t*>(source);
    #else
    return __builtin_bswap16(*reinterpret_cast<const uint8_t*>(source));
    #endif
}

int16_t readInt16(const uint8_t* source) {
    #if BIG_ENDIAN
    return *reinterpret_cast<const int16_t*>(source);
    #else
    return __builtin_bswap16(*reinterpret_cast<const int16_t*>(source));
    #endif
}

float readFloat32(const uint8_t* source) {
    #if BIG_ENDIAN
    return *reinterpret_cast<const float*>(source);
    #else
    return __builtin_bswap32(*reinterpret_cast<const float*>(source));
    #endif
}

uint32_t readUint32(const uint8_t* source) {
    #if BIG_ENDIAN
    return *reinterpret_cast<const uint32_t*>(source);
    #else
    return __builtin_bswap32(*reinterpret_cast<const uint32_t*>(source));
    #endif
}

int32_t readInt32(const uint8_t* source) {
    #if BIG_ENDIAN
    return *reinterpret_cast<const int32_t*>(source);
    #else
    return __builtin_bswap32(*reinterpret_cast<const int32_t*>(source));
    #endif
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


    PrimitiveObject::PrimitiveObject(Type _type) : type(_type) {

    }

    PrimitiveObject::PrimitiveObject(bool value) : PrimitiveObject((value) ? Type::BOOL_TRUE : Type::BOOL_FALSE) {

    }

    int64_t PrimitiveObject::startDeserialize(std::streambuf* streamBuffer) {
        type = (uint8_t)streamBuffer->sbumpc();
        return 1;
    }

    std::streamsize PrimitiveObject::serialize(int64_t& pos, std::streambuf* streamBuffer, std::streamsize bytes) {
        if(bytes > 0 && pos == 0 && streamBuffer->sputc(type) >= 0)
            return (++ pos);
        else
            return 0;
    }

    int64_t PrimitiveObject::getEndPos() const {
        return 1;
    }

    void PrimitiveObject::stringify(std::ostream& stream) const {
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

    Type PrimitiveObject::getType() const {
        return (Type)type;
    }

    bool PrimitiveObject::isNull() const {
        return (type == Type::NIL);
    }

    bool PrimitiveObject::getValue() const {
        return (type == Type::BOOL_TRUE);
    }



    int64_t HeaderObject::startSerialize() {
        return -getHeaderLength();
    }

    int64_t HeaderObject::startDeserialize(std::streambuf* streamBuffer) {
        header[0] = (uint8_t)streamBuffer->sbumpc();
        return 1-getHeaderLength();
    }

    std::streamsize HeaderObject::serialize(int64_t& pos, std::streambuf* streamBuffer, std::streamsize bytes) {
        if(pos < 0) {
            bytes = std::min(bytes, (std::streamsize)(-pos));
            bytes = streamBuffer->sputn(reinterpret_cast<char*>(header+getHeaderLength()+pos), bytes);
            pos += bytes;
            return bytes;
        }else
            return 0;
    }

    std::streamsize HeaderObject::deserialize(int64_t& pos, std::streambuf* streamBuffer, std::streamsize bytes) {
        if(pos < 0) {
            bytes = std::min(bytes, (std::streamsize)(-pos));
            bytes = streamBuffer->sgetn(reinterpret_cast<char*>(header+getHeaderLength()+pos), bytes);
            pos += bytes;
            return bytes;
        }else
            return 0;
    }

    int64_t HeaderObject::getEndPos() const {
        return 0;
    }

    Type HeaderObject::getType() const {
        return (Type)header[0];
    }

    uint32_t HeaderObject::getLength() const {
        return getEndPos();
    }



    std::streamsize DataObject::serialize(int64_t& pos, std::streambuf* streamBuffer, std::streamsize bytes) {
        std::streamsize bytesDone = HeaderObject::serialize(pos, streamBuffer, bytes);

        if(pos >= 0 && data) {
            bytes = std::min(bytes, (std::streamsize)(getEndPos()-pos));
            bytes = streamBuffer->sputn(reinterpret_cast<char*>(data.get()+pos), bytes);
            pos += bytes;
            bytesDone += bytes;
        }

        return bytesDone;
    }

    std::streamsize DataObject::deserialize(int64_t& pos, std::streambuf* streamBuffer, std::streamsize bytes) {
        std::streamsize bytesDone = HeaderObject::deserialize(pos, streamBuffer, bytes);

        if(pos >= 0) {
            int64_t dataLen = getEndPos();
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



    BinaryObject::BinaryObject(uint32_t len, const uint8_t* _data) {
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

    int64_t BinaryObject::getEndPos() const {
        switch(header[0]) {
            case Type::BIN_8:
                return readUint8(&header[1]);
            case Type::BIN_16:
                return readUint16(&header[1]);
            case Type::BIN_32:
                return readUint32(&header[1]);
            default:
                return 0;
        }
    }

    int64_t BinaryObject::getHeaderLength() const {
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

    void BinaryObject::stringify(std::ostream& stream) const {
        uint32_t len = getEndPos();
        stream << "< Buffer length=" << len << " data: ";
        for(uint32_t i = 0; i < len; i ++)
            stream << (uint16_t)data[i] << " ";
        stream << ">";
    }

    uint8_t* BinaryObject::getData() const {
        return data.get();
    }



    ExtendedObject::ExtendedObject(uint8_t type, uint32_t len, const uint8_t* _data) {
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

    int64_t ExtendedObject::getEndPos() const {
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
                return readUint8(&header[1])+1;
            case Type::EXT_16:
                return readUint16(&header[1])+1;
            case Type::EXT_32:
                return readUint32(&header[1])+1;
            default:
                return 0;
        }
    }

    int64_t ExtendedObject::getHeaderLength() const {
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

    void ExtendedObject::stringify(std::ostream& stream) const {
        uint32_t len = getEndPos();
        stream << "< Extended type=" << (uint16_t)data[0];
        stream << " length=" << (len-1) << " data: ";
        for(uint32_t i = 1; i < len; i ++)
            stream << (uint16_t)data[i] << " ";
        stream << ">";
    }

    uint8_t ExtendedObject::getDataType() const {
        return data[0];
    }

    uint8_t* ExtendedObject::getData() const {
        return &data[1];
    }

    uint32_t ExtendedObject::getLength() const {
        return getEndPos()-1;
    }



    StringObject::StringObject(const std::string& str) {
        uint32_t len = str.size();
        if(len > 0) {
            data.reset(new uint8_t[len]);
            memcpy(data.get(), &*str.begin(), len);
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
            default:
                return 0;
        }
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
            default:
                return 0;
        }
    }

    void StringObject::stringify(std::ostream& stream) const {
        stream << "\"";
        stream << getStr();
        stream << "\"";
    }

    std::string StringObject::getStr() const {
        return std::string((const char*)data.get(), getEndPos());
    }



    NumberObject::NumberObject(uint64_t value) {
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

    NumberObject::NumberObject(int64_t value) {
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

    NumberObject::NumberObject(float value) {
        data[0] = Type::FLOAT_32;
        storeFloat32(data+1, value);
    }

    NumberObject::NumberObject(double value) {
        data[0] = Type::FLOAT_64;
        storeFloat64(data+1, value);
    }

    int64_t NumberObject::startDeserialize(std::streambuf* streamBuffer) {
        data[0] = (uint8_t)streamBuffer->sbumpc();
        return 1;
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

    void NumberObject::stringify(std::ostream& stream) const {
            if(data[0] < 0x80 || data[0] >= 0xE0)
                stream << (int16_t)reinterpret_cast<const int8_t&>(data[0]);
            else
                switch(data[0]) {
                    case Type::FLOAT_32:
                        stream << readFloat32(data+1);
                    break;
                    case Type::FLOAT_64:
                        stream << readFloat64(data+1);
                    break;
                    case Type::UINT_8:
                        stream << (uint16_t)readUint8(data+1);
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
                        stream << (int16_t)readInt8(data+1);
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



    ArrayHeaderObject::ArrayHeaderObject(uint32_t len) {
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

    uint32_t ArrayHeaderObject::getLength() const {
        if(header[0] >= Type::FIXARRAY && header[0] < Type::FIXSTR)
            return header[0] - Type::FIXARRAY;

        switch(header[0]) {
            case Type::ARRAY_16:
                return readUint16(&header[1]);
            case Type::ARRAY_32:
                return readUint32(&header[1]);
            default:
                return 0;
        }
    }

    int64_t ArrayHeaderObject::getHeaderLength() const {
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

    void ArrayHeaderObject::stringify(std::ostream& stream) const {
        stream << "< Array length=" << getLength() << " >";
    }



    MapHeaderObject::MapHeaderObject(uint32_t len) {
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

    uint32_t MapHeaderObject::getLength() const {
        if(header[0] >= Type::FIXMAP && header[0] < Type::FIXARRAY)
            return header[0] - Type::FIXMAP;

        switch(header[0]) {
            case Type::MAP_16:
                return readUint16(&header[1]);
            case Type::MAP_32:
                return readUint32(&header[1]);
            default:
                return 0;
        }
    }

    int64_t MapHeaderObject::getHeaderLength() const {
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

    void MapHeaderObject::stringify(std::ostream& stream) const {
        stream << "< Map length=" << getLength() << " >";
    }



    ArrayObject::ArrayObject(std::vector<std::unique_ptr<Object>> _elements)
        : ArrayHeaderObject(_elements.size()), elements(std::move(_elements)) {

    }

    bool ArrayObject::containerDeserialized() {
        uint32_t len = getLength();
        if(len > 0) {
            elements = std::vector<std::unique_ptr<Object>>(len);
            return true;
        }else
            return false;
    }

    std::vector<std::unique_ptr<Object>>* ArrayObject::getContainer() {
        return &elements;
    }

    void ArrayObject::stringify(std::ostream& stream) const {
        uint32_t len = elements.size();
        stream << "[";
        if(len > 0) {
            elements[0]->stringify(stream);
            for(uint32_t i = 1; i < len; i ++) {
                stream << ", ";
                elements[i]->stringify(stream);
            }
        }
        stream << "]";
    }



    MapObject::MapObject(std::vector<std::unique_ptr<Object>> _elements)
        : MapHeaderObject(_elements.size()/2), elements(std::move(_elements)) {
        if(elements.size()%2 == 1)
            elements.erase(elements.end()-1);
    }

    bool MapObject::containerDeserialized() {
        uint32_t len = getLength();
        if(len > 0) {
            elements = std::vector<std::unique_ptr<Object>>(len*2);
            return true;
        }else
            return false;
    }

    std::vector<std::unique_ptr<Object>>* MapObject::getContainer() {
        return &elements;
    }

    void MapObject::stringify(std::ostream& stream) const {
        uint64_t len = elements.size();
        stream << "{";
        if(len > 0) {
            elements[0]->stringify(stream);
            stream << ": ";
            elements[1]->stringify(stream);
            for(uint64_t i = 2; i < len; i += 2) {
                stream << ", ";
                elements[i]->stringify(stream);
                stream << ": ";
                elements[i+1]->stringify(stream);
            }
        }
        stream << "}";
    }



    std::streamsize Serializer::serialize(PullCallback pullObject, std::streamsize bytesLeft) {
        bool deserializeAll = (bytesLeft == 0);
        std::streamsize bytesDone = 0;

        while(bytesLeft > 0 || deserializeAll) {
            if(deserializeAll) bytesLeft = LONG_MAX;

            //Try to pull next object if necessary
            if(stack.size() == 0) {
                if(queue.size() > 0) {
                    rootObject = std::move(queue[0]);
                    queue.erase(queue.begin());
                }else if(pullObject)
                    rootObject = pullObject();

                if(!rootObject)
                    break; //Got no object: quit

                stack.push_back(StackElement(rootObject.get(), rootObject->startSerialize()));
            }

            //Find highest object in stack
            StackElement* stackPointer = &stack[stack.size()-1];
            
            //Serialize object
            std::streamsize bytesWritten = stackPointer->first->serialize(stackPointer->second, streamBuffer, bytesLeft);
            bytesLeft -= bytesWritten;
            bytesDone += bytesWritten;

            if(bytesWritten == 0)
                break; //Stream overflow: quit

            if(stackPointer->second < stackPointer->first->getEndPos())
                continue; //Not done yet

            //Finish object
            std::vector<std::unique_ptr<Object>>* container = stackPointer->first->getContainer();
            if(container && container->size() > 0) {
                //Serialized header, begin with first child
                Object* childObject = container->begin()->get();
                stack.push_back(StackElement(childObject, childObject->startSerialize()));
                continue;
            }

            //Find lowest done container in stack
            uint32_t stackIndex = stack.size()-1;
            while(true) {
                stackPointer = &stack[stackIndex];
                container = stackPointer->first->getContainer();
                if(container && stackPointer->second+1 < container->size()) {
                    //Container is not done yet. move to next object
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

            //Check if root object is done
            if(stackIndex == 0)
                rootObject.reset();
        }

        return bytesDone;
    }

    uint32_t Serializer::getQueueLength() {
        if(rootObject)
            return 1 + queue.size();
        else
            return queue.size();
    }

    Serializer& Serializer::operator<<(std::unique_ptr<Object>& object) {
        if(object)
            queue.push_back(std::move(object));
        return *this;
    }

    Serializer& Serializer::operator<<(Object* object) {
        if(object)
            queue.push_back(std::move(std::unique_ptr<Object>(object)));
        return *this;
    }



    std::streamsize Deserializer::deserialize(PushCallback pushObject, std::streamsize bytesLeft) {
        bool deserializeAll = (bytesLeft == 0);
        std::streamsize bytesDone = 0;

        while(bytesLeft > 0 || deserializeAll) {
            if(deserializeAll) bytesLeft = LONG_MAX;

            //Find highest object in stack
            StackElement* stackPointer = (stack.size() > 0) ? &stack[stack.size()-1] : NULL;

            if(stackPointer && stackPointer->second < stackPointer->first->getEndPos()) {
                //Deserialize object
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
                //Deserialize next object
                int read = streamBuffer->sgetc();
                if(read < 0) break;
                Object* object;
                uint8_t nextByte = (uint8_t)read;
                bytesLeft --;

                if(nextByte < Type::FIXMAP || nextByte >= Type::FIXINT)
                    object = new NumberObject();
                else if(nextByte < Type::FIXARRAY)
                    object = (hierarchy) ? new MapObject() : new MapHeaderObject();
                else if(nextByte < Type::FIXSTR)
                    object = (hierarchy) ? new ArrayObject() : new ArrayHeaderObject();
                else if(nextByte < Type::NIL)
                    object = new StringObject();
                else
                    switch(nextByte) {
                        case Type::NIL:
                        case Type::ERROR:
                        case Type::BOOL_FALSE:
                        case Type::BOOL_TRUE:
                            object = new PrimitiveObject();
                        break;
                        case Type::BIN_8:
                        case Type::BIN_16:
                        case Type::BIN_32:
                            object = new BinaryObject();
                        break;
                        case Type::EXT_8:
                        case Type::EXT_16:
                        case Type::EXT_32:
                        case Type::FIXEXT_8:
                        case Type::FIXEXT_16:
                        case Type::FIXEXT_32:
                        case Type::FIXEXT_64:
                        case Type::FIXEXT_128:
                            object = new ExtendedObject();
                        break;
                        case Type::STR_8:
                        case Type::STR_16:
                        case Type::STR_32:
                            object = new StringObject();
                        break;
                        case Type::ARRAY_16:
                        case Type::ARRAY_32:
                            object = (hierarchy) ? new ArrayObject() : new ArrayHeaderObject();
                        break;
                        case Type::MAP_16:
                        case Type::MAP_32:
                            object = (hierarchy) ? new MapObject() : new MapHeaderObject();
                        break;
                        default:
                            object = new NumberObject();
                        break;
                    }

                //Put object in parent container
                if(stack.size() > 0) {
                    std::vector<std::unique_ptr<Object>>* container = stackPointer->first->getContainer();
                    *(container->begin()+stackPointer->second) = std::move(std::unique_ptr<Object>(object));
                }else
                    rootObject.reset(object);

                //Check if object is done
                int64_t pos = object->startDeserialize(streamBuffer);
                if(pos < object->getEndPos() || object->containerDeserialized()) {
                    stack.push_back(StackElement(object, pos));
                    continue;
                }
            }

            //Object done
            if(stack.size() > 0) {
                //Pop all done containers from stack
                uint32_t stackIndex = stack.size()-1;
                std::vector<std::unique_ptr<Object>>* container;
                while(true) {
                    container = stack[stackIndex].first->getContainer();
                    if(container && stack[stackIndex].second+1 < container->size()) {
                        stack[stackIndex ++].second ++;
                        break;
                    }
                    if(stackIndex == 0) break;
                    stackIndex --;
                }
                
                stack.erase(stack.begin()+stackIndex, stack.end());
            }
    
            //Trigger event for done object
            if(stack.size() == 0 && pushObject(std::move(rootObject)))
                break; //One object done: quit
        }

        return bytesDone;
    }

    Deserializer& Deserializer::operator>>(std::unique_ptr<Object>& object) {
        deserialize([&object](std::unique_ptr<Object> _object) {
            object = std::move(_object);
            return true;
        }, true);
        return *this;
    }



    std::ostream& operator<<(std::ostream& ostream, const Object& obj) {
        obj.stringify(ostream);
        return ostream;
    }
};