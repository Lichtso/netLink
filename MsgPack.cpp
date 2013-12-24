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


    AbstractObject::AbstractObject(Type _type) : type(_type) {

    }

    AbstractObject::AbstractObject(bool value) : AbstractObject((value) ? Type::BOOL_TRUE : Type::BOOL_FALSE) {

    }

    int64_t AbstractObject::startDeserialize(std::streambuf* streamBuffer) {
        type = (uint8_t)streamBuffer->sbumpc();
        return 1;
    }

    std::streamsize AbstractObject::serialize(int64_t& pos, std::streambuf* streamBuffer, std::streamsize bytes) {
        if(bytes > 0 && pos == 0 && streamBuffer->sputc(type) >= 0)
            return (++ pos);
        else
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



    BinObject::BinObject(uint32_t len, const uint8_t* _data) {
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

    int64_t BinObject::getEndPos() const {
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

    int64_t BinObject::getHeaderLength() const {
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

    void BinObject::stringify(std::ostream& stream) const {
        uint32_t len = getEndPos();
        stream << "< Buffer length=" << len << " data: ";
        for(uint32_t i = 0; i < len; i ++)
            stream << (uint16_t)data[i] << " ";
        stream << ">";
    }

    uint8_t* BinObject::getData() const {
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
        float fValue = (float)value;
        if((double)fValue == value) {
            data[0] = Type::FLOAT_32;
            storeFloat32(data+1, fValue);
        }else{
            data[0] = Type::FLOAT_64;
            storeFloat64(data+1, value);
        }
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



    ArrayObject::ArrayObject(std::vector<std::unique_ptr<Object>>&& _elements)
        : ArrayHeaderObject(_elements.size()), elements(std::move(_elements)) {

    }

    Object* ArrayObject::containerGetObject(int64_t& pos) {
        return (pos >= 0 && pos < getLength()) ? elements[pos].get() : NULL;
    }

    bool ArrayObject::containerInsertObject(std::unique_ptr<Object>&& element) {
        elements.push_back(std::move(element));
        return elements.size() == getLength();
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



    MapObject::MapObject(std::vector<std::unique_ptr<Object>>&& _elements)
        : MapHeaderObject(_elements.size()/2), elements(std::move(_elements)) {
        if(elements.size()%2 == 1)
            elements.erase(elements.end()-1);
    }

    Object* MapObject::containerGetObject(int64_t& pos) {
        return (pos >= 0 && pos < getLength()*2) ? elements[pos].get() : NULL;
    }

    bool MapObject::containerInsertObject(std::unique_ptr<Object>&& element) {
        elements.push_back(std::move(element));
        return elements.size() == getLength()*2;
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



    /*std::streamsize Serializer::serialize(std::streamsize bytesLeft) {
        bool deserializeAll = (bytesLeft == 0);
        std::streamsize bytesDone = 0;

        while(rootObject && (bytesLeft > 0 || deserializeAll)) {
            if(deserializeAll) bytesLeft = LONG_MAX;
            
            uint32_t index = 0;
            Object* object = rootObject.get();
            while(object->isContainer())
                object = object->containerGetObject(stack[index ++]);

            std::streamsize bytesWritten = object->serialize(stack[index], streamBuffer, bytesLeft);
            bytesLeft -= bytesWritten;
            bytesDone += bytesWritten;
    
            if(bytesWritten == 0)
                break;
            else if(stack[index] == object->getEndPos()) {
                
            }
        }

        return bytesDone;
    }*/



    bool Deserializer::checkObject() {
        auto iterator = stack.end()-1;
        if((*iterator).first < (*iterator).second->getEndPos() || (*iterator).second->isContainer()) return false;

        //Push deserialized objects into parent container
        while(iterator > stack.begin() &&
              (*(iterator-1)).second->containerInsertObject(std::move((*iterator).second)))
            iterator --;

        //Trigger event for finished object
        if(iterator == stack.begin())
            objectDeserialized(std::move((*iterator).second));

        //Pop all deserialized objects from stack
        stack.erase(iterator, stack.end());

        return true;
    }

    std::streamsize Deserializer::deserialize(bool onlyOne, std::streamsize bytesLeft) {
        bool deserializeAll = (bytesLeft == 0);
        std::streamsize bytesDone = 0;

        while(bytesLeft > 0 || deserializeAll) {
            if(deserializeAll) bytesLeft = LONG_MAX;

            auto iterator = stack.end()-1;
            if(stack.size() > 0 && (*iterator).first < (*iterator).second->getEndPos()) {
                std::streamsize bytesRead = (*iterator).second->deserialize((*iterator).first, streamBuffer, bytesLeft);
                bytesLeft -= bytesRead;
                bytesDone += bytesRead;

                if(bytesRead == 0)
                    break;
                else if(checkObject() && onlyOne)
                    return bytesDone;
            }else{
                //Deserialize next object
                int read = streamBuffer->sgetc();
                if(read < 0) break;
                uint8_t nextByte = (uint8_t)read;
                bytesLeft --;
    
                Object* nextObject;
                if(nextByte < Type::FIXMAP || nextByte >= Type::FIXINT)
                    nextObject = new NumberObject();
                else if(nextByte < Type::FIXARRAY)
                    nextObject = (tokenStream) ? new MapHeaderObject() : new MapObject();
                else if(nextByte < Type::FIXSTR)
                    nextObject = (tokenStream) ? new ArrayHeaderObject() : new ArrayObject();
                else if(nextByte < Type::NIL)
                    nextObject = new StringObject();
                else
                    switch(nextByte) {
                        case Type::NIL:
                        case Type::ERROR:
                        case Type::BOOL_FALSE:
                        case Type::BOOL_TRUE:
                            nextObject = new AbstractObject();
                        break;
                        case Type::BIN_8:
                        case Type::BIN_16:
                        case Type::BIN_32:
                            nextObject = new BinObject();
                        break;
                        case Type::EXT_8:
                        case Type::EXT_16:
                        case Type::EXT_32:
                        case Type::FIXEXT_8:
                        case Type::FIXEXT_16:
                        case Type::FIXEXT_32:
                        case Type::FIXEXT_64:
                        case Type::FIXEXT_128:
                            nextObject = new ExtendedObject();
                        break;
                        case Type::STR_8:
                        case Type::STR_16:
                        case Type::STR_32:
                            nextObject = new StringObject();
                        break;
                        case Type::ARRAY_16:
                        case Type::ARRAY_32:
                            nextObject = (tokenStream) ? new ArrayHeaderObject() : new ArrayObject();
                        break;
                        case Type::MAP_16:
                        case Type::MAP_32:
                            nextObject = (tokenStream) ? new MapHeaderObject() : new MapObject();
                        break;
                        default:
                            nextObject = new NumberObject();
                        break;
                    }

                stack.push_back(stackElement(nextObject->startDeserialize(streamBuffer),
                                             std::unique_ptr<Object>(nextObject)));
                if(checkObject() && onlyOne)
                    return bytesDone;
            }
        }

        return bytesDone;
    }



    std::ostream& operator<<(std::ostream& ostream, const Object& obj) {
        obj.stringify(ostream);
        return ostream;
    }
};