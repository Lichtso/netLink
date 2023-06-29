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

#include "Element.h"

namespace MsgPack {

    //! Abstract parent class of Serializer and Deserializer
    class StreamManager {
        protected:
        typedef std::pair<Element*, int64_t> StackElement; //!< To store the (de)serializer position
        std::unique_ptr<Element> rootElement; //!< The element at the root of the hierarchy
        std::vector<StackElement> stack; //!< The element hierarchy stack
        std::streambuf* streamBuffer; //!< The raw stream buffer
        //! Initalize from stream buffer
        StreamManager(std::streambuf* _streamBuffer) : streamBuffer(_streamBuffer) { }
    };

    //! Used to serialize elements into a std::streambuf
    class Serializer : public StreamManager {
        typedef std::function<std::unique_ptr<Element>()> PullCallback; //!< Typedef of callback to get the next element to be serialized
        public:
        /*! Constructs the Serializer
         @param _streamBuffer A std::streambuf to be used as target for read operations
         */
        Serializer(std::streambuf* _streamBuffer)
            : StreamManager(_streamBuffer) { }
        /*! Pulls elements and writes them into the streamBuffer
         @param pullElement Callback which will be called to get the next element
         @param bytes Limit of bytes to write or 0 to write as much as possible
         */
        std::streamsize serialize(PullCallback pullElement, std::streamsize bytes = 0);
        /*! Tries to serialize one MsgPack::Element into the streamBuffer
         @param element std::unique_ptr containing the element
         @param bytes Limit of bytes to write or 0 to write as much as possible
         */
        std::streamsize serialize(std::unique_ptr<Element>& element, std::streamsize bytes = 0);
        /*! Tries to serialize one MsgPack::Element into the streamBuffer
         @param element std::unique_ptr containing the element
         */
        Serializer& operator<<(std::unique_ptr<Element>& element) {
            Serializer::serialize(element);
            return *this;
        }
        /*! Tries to serialize one MsgPack::Element into the streamBuffer
         @param element std::unique_ptr containing the element
         */
        Serializer& operator<<(std::unique_ptr<Element>&& element) {
            return *this << element;
        }
    };

    //! Used to deserialize elements from a std::streambuf
    class Deserializer : public StreamManager {
        typedef std::function<bool(std::unique_ptr<Element> parsedElement)> PushCallback; //!< Typedef of callback to return deserialized elements
        public:
        /*! Constructs the Deserializer
         @param _streamBuffer A std::streambuf to be used as target for write operations
         */
        Deserializer(std::streambuf* _streamBuffer)
            : StreamManager(_streamBuffer) { }
        /*! Deserializes elements from the streamBuffer
         @param pushElement Callback which will be called when the next element has
                            been deserialized and can return true to stop the deserialization
         @param hierarchy If false arrays and maps will be deserialized as a flat stream of elements
         @param bytes Limit of bytes to read or 0 to read as much as possible
         */
        std::streamsize deserialize(PushCallback pushElement, bool hierarchy = true, std::streamsize bytes = 0);
        /*! Deserializes one element from the streamBuffer
         @param element std::unique_ptr in which the element will be stored
         @param hierarchy If false arrays and maps will be deserialized as a flat stream of elements
         @param bytes Limit of bytes to read or 0 to read as much as possible
         */
        std::streamsize deserialize(std::unique_ptr<Element>& element, bool hierarchy = true, std::streamsize bytes = 0);
        /*! Tries to deserialize one MsgPack::Element from the streamBuffer
         @param element std::unique_ptr in which the element will be stored
         */
        Deserializer& operator>>(std::unique_ptr<Element>& element) {
            deserialize(element);
            return *this;
        }
    };

    std::ostream& operator<<(std::ostream& ostream, const Element& obj);
};
