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
#include <queue>
#include <string>
#include <sstream>
#include <functional>

#ifndef netLink_MsgPack
#define netLink_MsgPack

//Store numbers in network endian (big endian)
inline void storeUint8(uint8_t* target, uint8_t source);
inline void storeInt8(uint8_t* target, int8_t source);
inline void storeUint16(uint8_t* target, uint16_t source);
inline void storeInt16(uint8_t* target, int16_t source);
inline void storeFloat32(uint8_t* target, float source);
inline void storeUint32(uint8_t* target, uint32_t source);
inline void storeInt32(uint8_t* target, int32_t source);
inline void storeFloat64(uint8_t* target, double source);
inline void storeUint64(uint8_t* target, uint64_t source);
inline void storeInt64(uint8_t* target, int64_t source);

//Read numbers from network endian (big endian)
inline uint8_t readUint8(const uint8_t* source);
inline int8_t readInt8(const uint8_t* source);
inline uint16_t readUint16(const uint8_t* source);
inline int16_t readInt16(const uint8_t* source);
inline float readFloat32(const uint8_t* source);
inline uint32_t readUint32(const uint8_t* source);
inline int32_t readInt32(const uint8_t* source);
inline double readFloat64(const uint8_t* source);
inline uint64_t readUint64(const uint8_t* source);
inline int64_t readInt64(const uint8_t* source);

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

    //! Abstract class to represent one element in a MsgPack stream
    class Object {
        friend Serializer;
        friend Deserializer;
        protected:
        Object() { }
        virtual int64_t startSerialize() { return 0; };
        virtual int64_t startDeserialize(std::streambuf* streamBuffer) = 0;
        virtual std::streamsize serialize(int64_t& pos, std::streambuf* streamBuffer, std::streamsize bytes) = 0;
        virtual std::streamsize deserialize(int64_t& pos, std::streambuf* streamBuffer, std::streamsize bytes) { return 0; };
        virtual bool containerDeserialized() { return false; };
        virtual std::vector<std::unique_ptr<Object>>* getContainer() { return NULL; };
        virtual int64_t getEndPos() const = 0;
        public:
        virtual ~Object() { }
        virtual void stringify(std::ostream& stream) const = 0;
        virtual Type getType() const = 0;
    };

    //! MsgPack::Object to represent one byte primitives like booleans and null
    class PrimitiveObject : public Object {
        friend Serializer;
        friend Deserializer;
        protected:
        uint8_t type;
        PrimitiveObject(Type type);
        int64_t startDeserialize(std::streambuf* streamBuffer);
        std::streamsize serialize(int64_t& pos, std::streambuf* streamBuffer, std::streamsize bytes);
        int64_t getEndPos() const;
        public:
        PrimitiveObject(bool value);
        PrimitiveObject() :PrimitiveObject(Type::NIL) { }
        void stringify(std::ostream& stream) const;
        Type getType() const;
        bool isNull() const;
        bool getValue() const;
    };

    //! MsgPack::Object to represent the header of a dynamical length object
    class HeaderObject : public Object {
        friend Serializer;
        friend Deserializer;
        protected:
        uint8_t header[5];
        int64_t startSerialize();
        int64_t startDeserialize(std::streambuf* streamBuffer);
        std::streamsize serialize(int64_t& pos, std::streambuf* streamBuffer, std::streamsize bytes);
        std::streamsize deserialize(int64_t& pos, std::streambuf* streamBuffer, std::streamsize bytes);
        int64_t getEndPos() const;
        virtual int64_t getHeaderLength() const = 0;
        public:
        Type getType() const;
        virtual uint32_t getLength() const;
    };
    
    //! MsgPack::HeaderObject with a additional buffer used as body
    class DataObject : public HeaderObject {
        friend Serializer;
        friend Deserializer;
        protected:
        std::unique_ptr<uint8_t[]> data;
        std::streamsize serialize(int64_t& pos, std::streambuf* streamBuffer, std::streamsize bytes);
        std::streamsize deserialize(int64_t& pos, std::streambuf* streamBuffer, std::streamsize bytes);
    };

    //! MsgPack::DataObject to represent binary/raw data elements
    class BinaryObject : public DataObject {
        friend Serializer;
        friend Deserializer;
        protected:
        BinaryObject() { }
        int64_t getEndPos() const;
        int64_t getHeaderLength() const;
        public:
        BinaryObject(uint32_t len, const uint8_t* data);
        void stringify(std::ostream& stream) const;
        uint8_t* getData() const;
    };
    
    //! MsgPack::DataObject to represent extended elements
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

    //! MsgPack::DataObject to represent strings
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

    //! MsgPack::Object for all types of numbers
    class NumberObject : public Object {
        friend Serializer;
        friend Deserializer;
        protected:
        uint8_t data[9];
        NumberObject() { }
        int64_t startDeserialize(std::streambuf* streamBuffer);
        std::streamsize serialize(int64_t& pos, std::streambuf* streamBuffer, std::streamsize bytes);
        std::streamsize deserialize(int64_t& pos, std::streambuf* streamBuffer, std::streamsize bytes);
        int64_t getEndPos() const;
        public:
        NumberObject(uint64_t value);
        NumberObject(int64_t value);
        NumberObject(float value);
        NumberObject(double value);
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
            return (T)0;
        }
    };

    //! MsgPack::HeaderObject representing only the header of a array
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

    //! MsgPack::HeaderObject representing only the header of a map
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

    //! MsgPack::ArrayHeaderObject representing the header and the content of a array
    class ArrayObject : public ArrayHeaderObject {
        friend Serializer;
        friend Deserializer;
        protected:
        std::vector<std::unique_ptr<Object>> elements;
        ArrayObject() { }
        bool containerDeserialized();
        std::vector<std::unique_ptr<Object>>* getContainer();
        public:
        ArrayObject(std::vector<std::unique_ptr<Object>> elements);
        void stringify(std::ostream& stream) const;
    };

    //! MsgPack::MapHeaderObject representing the header and the content of a map
    class MapObject : public MapHeaderObject {
        friend Serializer;
        friend Deserializer;
        protected:
        std::vector<std::unique_ptr<Object>> elements;
        MapObject() { }
        bool containerDeserialized();
        std::vector<std::unique_ptr<Object>>* getContainer();
        public:
        MapObject(std::vector<std::unique_ptr<Object>> elements);
        void stringify(std::ostream& stream) const;
    };

    //! Abstract parent class of Serializer and Deserializer
    class StreamManager {
        protected:
        typedef std::pair<Object*, int64_t> StackElement;
        std::unique_ptr<Object> rootObject;
        std::vector<StackElement> stack;
        std::streambuf* streamBuffer;
        StreamManager(std::streambuf* _streamBuffer) : streamBuffer(_streamBuffer) { }
    };

    //! Used to serialize elements into a std::streambuf
    class Serializer : public StreamManager {
        std::queue<std::unique_ptr<Object>> queue;
        typedef std::function<std::unique_ptr<Object>()> PullCallback;
        public:
        /*! Constructs the Serializer
         @param _streamBuffer A std::streambuf to be used as target for read operations
         */
        Serializer(std::streambuf* _streamBuffer) : StreamManager(_streamBuffer) { }
        /*! Serializes the objects in the queue and writes them into the streamBuffer
         @param pullObject Optional callback which will be called to get the next object if the queue is empty
         @param bytes Limit of bytes to write or 0 to write as much as possible
         */
        std::streamsize serialize(PullCallback pullObject = nullptr, std::streamsize bytes = 0);
        //! Returns the number of objects not serialized yet or 0 if awaiting the next object to be serialized
        uint32_t getQueueLength();
        /*! Pushes one MsgPack::Object in the queue.
            Call serialize() after you pushed some objects.
         @param object std::unique_ptr containing the object
         */
        Serializer& operator<<(std::unique_ptr<Object>& object);
        /*! Pushes one MsgPack::Object in the queue.
            Call serialize() after you pushed some objects.
         @param object pointer to the object
         @waring object will be deleted, don't push stack associated references
         */
        Serializer& operator<<(Object* object);
        Serializer& operator<<(const char* str) {
            return *this << new StringObject(str);
        }
        Serializer& operator<<(const std::string& str) {
            return *this << new StringObject(str);
        }
        Serializer& operator<<(bool value) {
            return *this << new PrimitiveObject(value);
        }
        Serializer& operator<<(uint64_t value) {
            return *this << new NumberObject(value);
        }
        Serializer& operator<<(int64_t value) {
            return *this << new NumberObject(value);
        }
        Serializer& operator<<(float value) {
            return *this << new NumberObject(value);
        }
        Serializer& operator<<(double value) {
            return *this << new NumberObject(value);
        }
        Serializer& operator<<(std::vector<std::unique_ptr<Object>> elements) {
            return *this << new ArrayObject(std::move(elements));
        }
    };

    //! Used to deserialize elements from a std::streambuf
    class Deserializer : public StreamManager {
        typedef std::function<bool(std::unique_ptr<Object> parsedObject)> PushCallback;
        bool hierarchy;
        public:
        /*! Constructs the Deserializer
         @param _streamBuffer A std::streambuf to be used as target for write operations
         @param _hierarchy If false arrays and maps will be deserialized as a flat stream of elements
         */
        Deserializer(std::streambuf* _streamBuffer, bool _hierarchy = true)
            : StreamManager(_streamBuffer), hierarchy(_hierarchy) { }
        /*! Deserializes objects from the streamBuffer
         @param pullObject Callback which will be called when the next object has
                           been deserialized and can return true to stop the deserializing
         @param bytes Limit of bytes to read or 0 to read as much as possible
         */
        std::streamsize deserialize(PushCallback pushObject, std::streamsize bytes = 0);
        /*! Tries to deserialize one MsgPack::Object from the streamBuffer
         @param object std::unique_ptr in which the object will be stored
         */
        Deserializer& operator>>(std::unique_ptr<Object>& object);
    };

    std::ostream& operator<<(std::ostream& ostream, const Object& obj);
};

#endif
