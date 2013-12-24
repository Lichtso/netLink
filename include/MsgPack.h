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

#include <set>
#include <vector>
#include <string>
#include <sstream>
#include <functional>

#ifndef netLink_MsgPack
#define netLink_MsgPack

void storeUint8(uint8_t* target, uint8_t source);
void storeInt8(uint8_t* target, int8_t source);
void storeUint16(uint8_t* target, uint16_t source);
void storeInt16(uint8_t* target, int16_t source);
void storeFloat32(uint8_t* target, float source);
void storeUint32(uint8_t* target, uint32_t source);
void storeInt32(uint8_t* target, int32_t source);
void storeFloat64(uint8_t* target, double source);
void storeUint64(uint8_t* target, uint64_t source);
void storeInt64(uint8_t* target, int64_t source);

uint8_t readUint8(const uint8_t* source);
int8_t readInt8(const uint8_t* source);
uint16_t readUint16(const uint8_t* source);
int16_t readInt16(const uint8_t* source);
float readFloat32(const uint8_t* source);
uint32_t readUint32(const uint8_t* source);
int32_t readInt32(const uint8_t* source);
double readFloat64(const uint8_t* source);
uint64_t readUint64(const uint8_t* source);
int64_t readInt64(const uint8_t* source);

namespace MsgPack {

    enum Type {
        FIXUINT = 0x00,
        FIXMAP = 0x80,
        FIXARRAY = 0x90,
        FIXSTR = 0xA0,
        NIL = 0xC0,
        ERROR = 0xC1,
        BOOL_FALSE = 0xC2,
        BOOL_TRUE = 0xC3,
        BIN_8 = 0xC4,
        BIN_16 = 0xC5,
        BIN_32 = 0xC6,
        EXT_8 = 0xC7,
        EXT_16 = 0xC8,
        EXT_32 = 0xC9,
        FLOAT_32 = 0xCA,
        FLOAT_64 = 0xCB,
        UINT_8 = 0xCC,
        UINT_16 = 0xCD,
        UINT_32 = 0xCE,
        UINT_64 = 0xCF,
        INT_8 = 0xD0,
        INT_16 = 0xD1,
        INT_32 = 0xD2,
        INT_64 = 0xD3,
        FIXEXT_8 = 0xD4,
        FIXEXT_16 = 0xD5,
        FIXEXT_32 = 0xD6,
        FIXEXT_64 = 0xD7,
        FIXEXT_128 = 0xD8,
        STR_8 = 0xD9,
        STR_16 = 0xDA,
        STR_32 = 0xDB,
        ARRAY_16 = 0xDC,
        ARRAY_32 = 0xDD,
        MAP_16 = 0xDE,
        MAP_32 = 0xDF,
        FIXINT = 0xE0
    };

    class Serializer;
    class Deserializer;

    class Object {
        friend Serializer;
        friend Deserializer;
        protected:
        Object() { }
        virtual int64_t startDeserialize(std::streambuf* streamBuffer) = 0;
        virtual std::streamsize deserialize(int64_t& pos, std::streambuf* streamBuffer, std::streamsize bytes) { return 0; };
        virtual Object* containerGetObject(int64_t& pos) { return NULL; };
        virtual bool containerInsertObject(std::unique_ptr<Object>&& element) { return true; };
        virtual bool isContainer() { return false; };
        virtual int64_t getEndPos() const = 0;
        public:
        virtual ~Object() { }
        virtual int64_t startSerialize() { return 0; };
        virtual std::streamsize serialize(int64_t& pos, std::streambuf* streamBuffer, std::streamsize bytes) = 0;
        virtual void stringify(std::ostream& stream) const = 0;
        virtual Type getType() const = 0;
    };

    class AbstractObject : public Object {
        friend Serializer;
        friend Deserializer;
        protected:
        uint8_t type;
        AbstractObject(Type type);
        int64_t startDeserialize(std::streambuf* streamBuffer);
        int64_t getEndPos() const;
        public:
        AbstractObject(bool value);
        AbstractObject() :AbstractObject(Type::NIL) { }
        std::streamsize serialize(int64_t& pos, std::streambuf* streamBuffer, std::streamsize bytes);
        void stringify(std::ostream& stream) const;
        Type getType() const;
        bool isNull() const;
        bool getValue() const;
    };

    class HeaderObject : public Object {
        friend Serializer;
        friend Deserializer;
        protected:
        uint8_t header[5];
        int64_t startDeserialize(std::streambuf* streamBuffer);
        std::streamsize deserialize(int64_t& pos, std::streambuf* streamBuffer, std::streamsize bytes);
        int64_t getEndPos() const;
        virtual int64_t getHeaderLength() const = 0;
        public:
        int64_t startSerialize();
        std::streamsize serialize(int64_t& pos, std::streambuf* streamBuffer, std::streamsize bytes);
        Type getType() const;
        virtual uint32_t getLength() const;
    };
    
    class DataObject : public HeaderObject {
        friend Serializer;
        friend Deserializer;
        protected:
        std::unique_ptr<uint8_t[]> data;
        std::streamsize deserialize(int64_t& pos, std::streambuf* streamBuffer, std::streamsize bytes);
        public:
        std::streamsize serialize(int64_t& pos, std::streambuf* streamBuffer, std::streamsize bytes);
    };

    class BinObject : public DataObject {
        friend Serializer;
        friend Deserializer;
        protected:
        BinObject() { }
        int64_t getEndPos() const;
        int64_t getHeaderLength() const;
        public:
        BinObject(uint32_t len, const uint8_t* data);
        void stringify(std::ostream& stream) const;
        uint8_t* getData() const;
    };
    
    class ExtendedObject : public DataObject {
        friend Serializer;
        friend Deserializer;
        protected:
        ExtendedObject() { }
        int64_t getEndPos() const;
        int64_t getHeaderLength() const;
        public:
        ExtendedObject(uint8_t type, uint32_t len, const uint8_t* data);
        void stringify(std::ostream& stream) const;
        uint8_t getDataType() const;
        uint8_t* getData() const;
        uint32_t getLength() const;
    };

    class StringObject : public DataObject {
        friend Serializer;
        friend Deserializer;
        protected:
        StringObject() { }
        int64_t getEndPos() const;
        int64_t getHeaderLength() const;
        public:
        StringObject(const std::string& str);
        void stringify(std::ostream& stream) const;
        std::string getStr() const;
    };

    class NumberObject : public Object {
        friend Serializer;
        friend Deserializer;
        protected:
        uint8_t data[9];
        NumberObject() { }
        int64_t startDeserialize(std::streambuf* streamBuffer);
        std::streamsize deserialize(int64_t& pos, std::streambuf* streamBuffer, std::streamsize bytes);
        int64_t getEndPos() const;
        public:
        NumberObject(uint64_t value);
        NumberObject(int64_t value);
        NumberObject(float value);
        NumberObject(double value);
        std::streamsize serialize(int64_t& pos, std::streambuf* streamBuffer, std::streamsize bytes);
        void stringify(std::ostream& stream) const;
        Type getType() const;
        bool isDone(int64_t pos) const;
        template<class T> T getValue() const {
            if(data[0] < 0x80 || data[0] >= 0xE0)
                return (T)reinterpret_cast<const int8_t&>(data[0]);
            else
                switch(data[0]) {
                    case Type::FLOAT_32:
                        return (T)readFloat32(data+1);
                    case Type::FLOAT_64:
                        return (T)readFloat64(data+1);
                    case Type::UINT_8:
                        return (T)readUint8(data+1);
                    case Type::UINT_16:
                        return (T)readUint16(data+1);
                    case Type::UINT_32:
                        return (T)readUint32(data+1);
                    case Type::UINT_64:
                        return (T)readUint64(data+1);
                    case Type::INT_8:
                        return (T)readInt8(data+1);
                    case Type::INT_16:
                        return (T)readInt16(data+1);
                    case Type::INT_32:
                        return (T)readInt32(data+1);
                    case Type::INT_64:
                        return (T)readInt64(data+1);
                }
        }
    };

    class ArrayHeaderObject : public HeaderObject {
        friend Serializer;
        friend Deserializer;
        protected:
        ArrayHeaderObject() { }
        uint32_t getLength() const;
        int64_t getHeaderLength() const;
        public:
        ArrayHeaderObject(uint32_t len);
        void stringify(std::ostream& stream) const;
    };

    class MapHeaderObject : public HeaderObject {
        friend Serializer;
        friend Deserializer;
        protected:
        MapHeaderObject() { }
        uint32_t getLength() const;
        int64_t getHeaderLength() const;
        public:
        MapHeaderObject(uint32_t len);
        void stringify(std::ostream& stream) const;
    };

    class ArrayObject : public ArrayHeaderObject {
        friend Serializer;
        friend Deserializer;
        protected:
        std::vector<std::unique_ptr<Object>> elements;
        ArrayObject() { }
        Object* containerGetObject(int64_t& pos);
        bool containerInsertObject(std::unique_ptr<Object>&& element);
        bool isContainer() { return true; };
        public:
        ArrayObject(std::vector<std::unique_ptr<Object>>&& elements);
        void stringify(std::ostream& stream) const;
    };

    class MapObject : public MapHeaderObject {
        friend Serializer;
        friend Deserializer;
        protected:
        std::vector<std::unique_ptr<Object>> elements;
        MapObject() { }
        Object* containerGetObject(int64_t& pos);
        bool containerInsertObject(std::unique_ptr<Object>&& element);
        bool isContainer() { return true; };
        public:
        MapObject(std::vector<std::unique_ptr<Object>>&& elements);
        void stringify(std::ostream& stream) const;
    };

    class StreamManager {
        public:
        bool tokenStream = true;
        std::streambuf* streamBuffer;
    };

    class Serializer : public StreamManager {
        std::unique_ptr<Object> rootObject;
        std::vector<int64_t> stack;
        public:
        std::streamsize serialize(std::streamsize bytes = 0);
    };

    class Deserializer : public StreamManager {
        typedef std::pair<int64_t, std::unique_ptr<Object>> stackElement;
        std::vector<stackElement> stack;
        bool checkObject();
        public:
        std::function<void(std::unique_ptr<Object> parsedObject)> objectDeserialized;
        std::streamsize deserialize(bool onlyOne = false, std::streamsize bytes = 0);
    };

    std::ostream& operator<<(std::ostream& ostream, const Object& obj);
};

#endif
