/*
    netLink: c++ 11 networking library
    Copyright (C) 2013-2023 Alexander Meißner

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
