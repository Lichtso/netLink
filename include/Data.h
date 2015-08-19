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

#pragma once

#include "Header.h"

namespace MsgPack {

    //! MsgPack::Header with a additional buffer used as body
    class Data : public Header {
        friend class Serializer;
        friend class Deserializer;
        protected:
        std::unique_ptr<char[]> data; //!< The raw data buffer
        std::streamsize serialize(int64_t& pos, std::basic_streambuf<char>* streamBuffer, std::streamsize bytes);
        std::streamsize deserialize(int64_t& pos, std::basic_streambuf<char>* streamBuffer, std::streamsize bytes);
    };

    //! MsgPack::Data to represent binary/raw data elements
    class Binary : public Data {
        friend class Serializer;
        friend class Deserializer;
        protected:
        Binary() { }
        int64_t getEndPos() const;
        int64_t getHeaderLength() const;
        public:
        //! Initialize from len in bytes and raw data (copy semantic)
        Binary(uint32_t len, const void* data);
        std::unique_ptr<Element> copy() const;
        void toJSON(std::ostream& stream) const;
        //! Returns a pointer to the binary data
        char* getData() const;
    };

    //! MsgPack::Data to represent extended elements
    class Extended : public Data {
        friend class Serializer;
        friend class Deserializer;
        protected:
        Extended() { }
        int64_t getEndPos() const;
        int64_t getHeaderLength() const;
        public:
        //! Initialize from type, len in bytes and raw data (copy semantic)
        Extended(uint8_t type, uint32_t len, const void* data);
        std::unique_ptr<Element> copy() const;
        void toJSON(std::ostream& stream) const;
        //! Returns the user defined data type
        uint8_t getDataType() const;
        //! Returns a pointer to the binary data
        char* getData() const;
        uint32_t getLength() const;
    };

    //! MsgPack::Data to represent strings
    class String : public Data {
        friend class Serializer;
        friend class Deserializer;
        void init(uint32_t len, const void* str);
        protected:
        String() { }
        int64_t getEndPos() const;
        int64_t getHeaderLength() const;
        public:
		//! Initialize from raw c string and length in bytes
		String(uint32_t len, const void* str);
		//! Initialize from nullterminated c string
		String(const char* str);
		//! Initialize from std::string
		String(const std::string& str);
        std::unique_ptr<Element> copy() const;
        void toJSON(std::ostream& stream) const;
        //! Returns a pointer to the binary data
        char* getData() const;
        //! Returns a std::string represenation of the content
        std::string stdString() const;
    };

};
