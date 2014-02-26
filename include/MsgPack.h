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

#pragma once

#include <vector>
#include <queue>
#include <string>
#include <sstream>
#include <functional>
#include <memory>
#include <inttypes.h>

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
        UNDEFINED = 0xC1,
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
    class Element {
        friend Serializer;
        friend Deserializer;
        protected:
        Element() { }
        virtual int64_t startSerialize() { return 0; };
        virtual int64_t startDeserialize(std::basic_streambuf<uint8_t>* streamBuffer) = 0;
        virtual std::streamsize serialize(int64_t& pos, std::basic_streambuf<uint8_t>* streamBuffer, std::streamsize bytes) = 0;
        virtual std::streamsize deserialize(int64_t& pos, std::basic_streambuf<uint8_t>* streamBuffer, std::streamsize bytes) { return 0; };
        virtual bool containerDeserialized() { return false; };
        virtual std::vector<std::unique_ptr<Element>>* getContainer() { return NULL; };
        virtual int64_t getEndPos() const = 0;
        public:
        virtual ~Element() { }
        //! Writes a human readable JSON-like string into the given stream
        virtual void stringify(std::ostream& stream) const = 0;
        //! Returns the MsgPack::Type
        virtual Type getType() const = 0;
    };

    //! MsgPack::Element to represent one byte primitives like booleans and null
    class Primitive : public Element {
        friend Serializer;
        friend Deserializer;
        protected:
        uint8_t type;
        Primitive(Type type);
        int64_t startDeserialize(std::basic_streambuf<uint8_t>* streamBuffer);
        std::streamsize serialize(int64_t& pos, std::basic_streambuf<uint8_t>* streamBuffer, std::streamsize bytes);
        int64_t getEndPos() const;
        public:
        Primitive(bool value);
        Primitive() :Primitive(Type::NIL) { }
        void stringify(std::ostream& stream) const;
        Type getType() const;
        //! Returns true if getType() == MsgPack::Type::NIL
        bool isNull() const;
        //! Returns true if getType() == MsgPack::Type::BOOL_TRUE
        bool getValue() const;
    };

    //! MsgPack::Element to represent the header of a dynamical length element
    class Header : public Element {
        friend Serializer;
        friend Deserializer;
        protected:
        uint8_t header[5];
        int64_t startSerialize();
        int64_t startDeserialize(std::basic_streambuf<uint8_t>* streamBuffer);
        std::streamsize serialize(int64_t& pos, std::basic_streambuf<uint8_t>* streamBuffer, std::streamsize bytes);
        std::streamsize deserialize(int64_t& pos, std::basic_streambuf<uint8_t>* streamBuffer, std::streamsize bytes);
        int64_t getEndPos() const;
        virtual int64_t getHeaderLength() const = 0;
        public:
        Type getType() const;
        //! Returns the content length in bytes
        virtual uint32_t getLength() const;
    };
    
    //! MsgPack::Header with a additional buffer used as body
    class Data : public Header {
        friend Serializer;
        friend Deserializer;
        protected:
        std::unique_ptr<uint8_t[]> data;
        std::streamsize serialize(int64_t& pos, std::basic_streambuf<uint8_t>* streamBuffer, std::streamsize bytes);
        std::streamsize deserialize(int64_t& pos, std::basic_streambuf<uint8_t>* streamBuffer, std::streamsize bytes);
    };

    //! MsgPack::Data to represent binary/raw data elements
    class Binary : public Data {
        friend Serializer;
        friend Deserializer;
        protected:
        Binary() { }
        int64_t getEndPos() const;
        int64_t getHeaderLength() const;
        public:
        Binary(uint32_t len, const uint8_t* data);
        void stringify(std::ostream& stream) const;
        //! Returns a pointer to the binary data
        uint8_t* getData() const;
    };
    
    //! MsgPack::Data to represent extended elements
    class Extended : public Data {
        friend Serializer;
        friend Deserializer;
        protected:
        Extended() { }
        int64_t getEndPos() const;
        int64_t getHeaderLength() const;
        public:
        Extended(uint8_t type, uint32_t len, const uint8_t* data);
        void stringify(std::ostream& stream) const;
        //! Returns the user defined data type
        uint8_t getDataType() const;
        //! Returns a pointer to the binary data
        uint8_t* getData() const;
        uint32_t getLength() const;
    };

    //! MsgPack::Data to represent strings
    class String : public Data {
        friend Serializer;
        friend Deserializer;
        protected:
        String() { }
        int64_t getEndPos() const;
        int64_t getHeaderLength() const;
        public:
        String(const std::string& str);
        void stringify(std::ostream& stream) const;
        //! Returns a std::string represenation of the content
        std::string getStr() const;
    };

    //! MsgPack::Element for all types of numbers
    class Number : public Element {
        friend Serializer;
        friend Deserializer;
        protected:
        uint8_t data[9];
        Number() { }
        int64_t startDeserialize(std::basic_streambuf<uint8_t>* streamBuffer);
        std::streamsize serialize(int64_t& pos, std::basic_streambuf<uint8_t>* streamBuffer, std::streamsize bytes);
        std::streamsize deserialize(int64_t& pos, std::basic_streambuf<uint8_t>* streamBuffer, std::streamsize bytes);
        int64_t getEndPos() const;
        public:
        Number(uint64_t value);
        Number(int64_t value);
        Number(float value);
        Number(double value);
        void stringify(std::ostream& stream) const;
        Type getType() const;
        //! Returns the value as given data type T
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

    //! MsgPack::Header representing only the header of a array
    class ArrayHeader : public Header {
        friend Serializer;
        friend Deserializer;
        protected:
        ArrayHeader() { }
        uint32_t getLength() const;
        int64_t getHeaderLength() const;
        public:
        ArrayHeader(uint32_t len);
        void stringify(std::ostream& stream) const;
    };

    //! MsgPack::Header representing only the header of a map
    class MapHeader : public Header {
        friend Serializer;
        friend Deserializer;
        protected:
        MapHeader() { }
        uint32_t getLength() const;
        int64_t getHeaderLength() const;
        public:
        MapHeader(uint32_t len);
        void stringify(std::ostream& stream) const;
    };

    //! MsgPack::ArrayHeader representing the header and the content of a array
    class Array : public ArrayHeader {
        friend Serializer;
        friend Deserializer;
        protected:
        std::vector<std::unique_ptr<Element>> elements;
        Array() { }
        bool containerDeserialized();
        std::vector<std::unique_ptr<Element>>* getContainer();
        public:
        Array(std::vector<std::unique_ptr<Element>> elements);
        void stringify(std::ostream& stream) const;
    };

    //! MsgPack::MapHeader representing the header and the content of a map
    class Map : public MapHeader {
        friend Serializer;
        friend Deserializer;
        protected:
        std::vector<std::unique_ptr<Element>> elements;
        Map() { }
        bool containerDeserialized();
        std::vector<std::unique_ptr<Element>>* getContainer();
        public:
        Map(std::vector<std::unique_ptr<Element>> elements);
        void stringify(std::ostream& stream) const;
    };

    //! Abstract parent class of Serializer and Deserializer
    class StreamManager {
        protected:
        typedef std::pair<Element*, int64_t> StackElement;
        std::unique_ptr<Element> rootElement;
        std::vector<StackElement> stack;
        std::basic_streambuf<uint8_t>* streamBuffer;
        StreamManager(std::basic_streambuf<uint8_t>* _streamBuffer) : streamBuffer(_streamBuffer) { }
    };

    //! Used to serialize elements into a std::streambuf
    class Serializer : public StreamManager {
        std::queue<std::unique_ptr<Element>> queue;
        typedef std::function<std::unique_ptr<Element>()> PullCallback;
        public:
        /*! Constructs the Serializer
         @param _streamBuffer A std::basic_streambuf<uint8_t> to be used as target for read operations
         */
        Serializer(std::basic_streambuf<uint8_t>* _streamBuffer)
            : StreamManager(_streamBuffer) { }
        /*! Constructs the Serializer
         @param _streamBuffer A std::streambuf to be used as target for read operations
         */
        Serializer(std::streambuf* _streamBuffer)
            : Serializer(reinterpret_cast<std::basic_streambuf<uint8_t>*>(_streamBuffer)) { }
        /*! Serializes the elements in the queue and writes them into the streamBuffer
         @param pullElement Optional callback which will be called to get the next element if the queue is empty
         @param bytes Limit of bytes to write or 0 to write as much as possible
         */
        std::streamsize serialize(PullCallback pullElement = nullptr, std::streamsize bytes = 0);
        //! Returns the number of elements not serialized yet or 0 if awaiting the next element to be serialized
        uint32_t getQueueLength();
        /*! Pushes one MsgPack::Element in the queue.
            Call serialize() after you pushed some elements.
         @param element std::unique_ptr containing the element
         */
        Serializer& operator<<(std::unique_ptr<Element>& element);
        /*! Pushes one MsgPack::Element in the queue.
            Call serialize() after you pushed some elements.
         @param element pointer to the element
         @waring element will be deleted, don't push stack associated references
         */
        Serializer& operator<<(Element* element);
        Serializer& operator<<(const char* str) {
            return *this << new String(str);
        }
        Serializer& operator<<(const std::string& str) {
            return *this << new String(str);
        }
        Serializer& operator<<(bool value) {
            return *this << new Primitive(value);
        }
        Serializer& operator<<(uint64_t value) {
            return *this << new Number(value);
        }
        Serializer& operator<<(int64_t value) {
            return *this << new Number(value);
        }
        Serializer& operator<<(float value) {
            return *this << new Number(value);
        }
        Serializer& operator<<(double value) {
            return *this << new Number(value);
        }
    };

    //! Used to deserialize elements from a std::streambuf
    class Deserializer : public StreamManager {
        typedef std::function<bool(std::unique_ptr<Element> parsedElement)> PushCallback;
        bool hierarchy;
        public:
        /*! Constructs the Deserializer
         @param _streamBuffer A std::basic_streambuf<uint8_t> to be used as target for write operations
         @param _hierarchy If false arrays and maps will be deserialized as a flat stream of elements
         */
        Deserializer(std::basic_streambuf<uint8_t>* _streamBuffer, bool _hierarchy = true)
            : StreamManager(_streamBuffer), hierarchy(_hierarchy) { }
        /*! Constructs the Deserializer
         @param _streamBuffer A std::streambuf to be used as target for write operations
         @param _hierarchy If false arrays and maps will be deserialized as a flat stream of elements
         */
        Deserializer(std::streambuf* _streamBuffer, bool _hierarchy = true)
            : Deserializer(reinterpret_cast<std::basic_streambuf<uint8_t>*>(_streamBuffer), _hierarchy) { }
        /*! Deserializes elements from the streamBuffer
         @param pullElement Callback which will be called when the next element has
                           been deserialized and can return true to stop the deserializing
         @param bytes Limit of bytes to read or 0 to read as much as possible
         */
        std::streamsize deserialize(PushCallback pushElement, std::streamsize bytes = 0);
        /*! Tries to deserialize one MsgPack::Element from the streamBuffer
         @param element std::unique_ptr in which the element will be stored
         */
        Deserializer& operator>>(std::unique_ptr<Element>& element);
    };

    std::ostream& operator<<(std::ostream& ostream, const Element& obj);
};