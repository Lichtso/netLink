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

#include "Element.h"

namespace MsgPack {
    
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
         @param pullElement Callback which will be called to get the next element
         @param bytes Limit of bytes to write or 0 to write as much as possible
         */
        std::streamsize serialize(PullCallback pullElement, std::streamsize bytes = 0);
        /*! Tries to serialize one MsgPack::Element into the streamBuffer
         @param element std::unique_ptr containing the element
         */
        Serializer& operator<<(std::unique_ptr<Element>& element);
    };

    //! Used to deserialize elements from a std::streambuf
    class Deserializer : public StreamManager {
        typedef std::function<bool(std::unique_ptr<Element> parsedElement)> PushCallback;
        public:
        /*! Constructs the Deserializer
         @param _streamBuffer A std::basic_streambuf<uint8_t> to be used as target for write operations
         */
        Deserializer(std::basic_streambuf<uint8_t>* _streamBuffer)
            : StreamManager(_streamBuffer) { }
        /*! Constructs the Deserializer
         @param _streamBuffer A std::streambuf to be used as target for write operations
         */
        Deserializer(std::streambuf* _streamBuffer)
            : Deserializer(reinterpret_cast<std::basic_streambuf<uint8_t>*>(_streamBuffer)) { }
        /*! Deserializes elements from the streamBuffer
         @param pullElement Callback which will be called when the next element has
                           been deserialized and can return true to stop the deserialization
         @param hierarchy If false arrays and maps will be deserialized as a flat stream of elements
         @param bytes Limit of bytes to read or 0 to read as much as possible
         */
        std::streamsize deserialize(PushCallback pushElement, bool hierarchy = true, std::streamsize bytes = 0);
        /*! Tries to deserialize one MsgPack::Element from the streamBuffer
         @param element std::unique_ptr in which the element will be stored
         */
        Deserializer& operator>>(std::unique_ptr<Element>& element);
    };

    std::ostream& operator<<(std::ostream& ostream, const Element& obj);
};