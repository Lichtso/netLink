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

#include "ContainerHeader.h"

namespace MsgPack {

    //! MsgPack::ArrayHeader representing the header and the content of a array
    class Array : public ArrayHeader {
        friend class Serializer;
        friend class Deserializer;
        protected:
        std::vector<std::unique_ptr<Element>> elements; //!< Element vector
        Array() { }
        bool containerDeserialized();
        public:
        //! Initialize from element vector (move semantic)
        Array(std::vector<std::unique_ptr<Element>>&& elements);
        std::unique_ptr<Element> copy() const;
        void toJSON(std::ostream& stream) const;
        uint32_t getSizeInBytes() const;
        std::vector<std::unique_ptr<Element>>* getElementsVector();
        //! Returns the entry at the index or nullptr if out of bounds
        Element* getEntry(uint32_t index) const;
    };

    //! MsgPack::MapHeader representing the header and the content of a map
    class Map : public MapHeader {
        friend class Serializer;
        friend class Deserializer;
        protected:
        std::vector<std::unique_ptr<Element>> elements; //!< Element vector is key/value interlaced
        Map() { }
        bool containerDeserialized();
        public:
        //! Initialize from element vector (move semantic)
        Map(std::vector<std::unique_ptr<Element>>&& elements);
        Map(std::map<std::string, std::unique_ptr<Element>>&& elements);
        std::unique_ptr<Element> copy() const;
        void toJSON(std::ostream& stream) const;
        uint32_t getSizeInBytes() const;
        std::vector<std::unique_ptr<Element>>* getElementsVector();
        /*! Generates a map of the element vector
         * @warning Life time of return value depends on this object
         */
        std::map<std::string, Element*> getElementsMap() const;
        //! Returns the entry pair at the index or nullptr if out of bounds
        std::pair<String*, Element*> getEntry(uint32_t index) const;
        //! Returns the key at the index or nullptr if out of bounds
        String* getKey(uint32_t index) const;
        //! Returns the value at the index or nullptr if out of bounds
        Element* getValue(uint32_t index) const;
    };

};
