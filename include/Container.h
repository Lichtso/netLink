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
