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

#include <cstring>
#include <string>

#ifdef _MSC_VER
#include <intrin.h>
#define countLeadingOneBits(x) __lzcnt(~(x))
#else
#define countLeadingOneBits(x) __builtin_clz(~(x))
#endif

namespace utf8 {
	//! Decrements a Utf8 string iterator by one character
	template<typename iterator>
	iterator& decrement(iterator& i) {
        while((*(--i)&0xC0) == 0x80);
        return i;
    }

	//! Increments a Utf8 string iterator by one character
	template<typename iterator>
	iterator& increment(iterator& i) {
        while((*(++i)&0xC0) == 0x80);
        return i;
    }

	//! Returns a decremented copy of the given Utf8 string iterator
	template<typename iterator>
	iterator decremented(iterator i) {
        return decrement(i);
    }

	//! Returns a incremented copy of the given Utf8 string iterator
	template<typename iterator>
	iterator incremented(iterator i) {
        return increment(i);
    }

	//! Decrements a Utf8 string iterator by n characters
	template<typename iterator>
    iterator& backward(iterator& i, size_t n) {
        for(size_t c = 0; true; --i) {
            if((*i&0xC0) != 0x80)
                ++c;
            if(c > n)
                break;
        }
        return i;
    }

	//! Increments a Utf8 string iterator by n characters
	template<typename iterator>
    iterator& forward(iterator& i, size_t n) {
        for(size_t c = 0; true; ++i) {
            if((*i&0xC0) != 0x80)
                ++c;
            if(c > n)
                break;
        }
        return i;
    }

	//! Decrements a Utf8 string iterator string by n characters or stops at end iterator
	template<typename iterator>
    iterator& backwardUntil(iterator& i, const iterator& end, size_t n) {
        for(size_t c = 0; i > end; --i) {
            if((*i&0xC0) != 0x80)
                ++c;
            if(c > n)
                break;
        }
        return i;
    }

	//! Increments a Utf8 string iterator by n characters or stops at end iterator
	template<typename iterator>
    iterator& forwardUntil(iterator& i, const iterator& end, size_t n) {
        for(size_t c = 0; i < end; ++i) {
            if((*i&0xC0) != 0x80)
                ++c;
            if(c > n)
                break;
        }
        return i;
    }

	//! Returns the difference in characters between two Utf8 string iterators
	template<typename iterator>
	size_t difference(const iterator& end, const iterator& begin) {
        size_t len = 0;
        for(auto i = begin; i < end; ++i)
            if((*i&0xC0) != 0x80)
                ++len;
        return len;
    }

	//! Byte Order Mark
	extern const char bom[4];

    //! Check the BOM at the given Utf8 string iterator
    template<typename iterator>
    bool checkBOM(iterator i) {
        return (i[0] == bom[0] && i[1] == bom[1]  && i[2] == bom[2]);
    }

    //! Set the BOM in the given Utf8 string
    void setBOM(std::string& str);

	//! Get the length in characters of a Utf8 string
	size_t length(const char* str);

	//! Get the length in characters of a Utf8 string
	size_t length(const std::string& str);

	//! Insert a Utf8 string into another one at the given character position
	void insert(std::string& dst, size_t pos, const char* src);

	//! Erase a interval from a Utf8 string at the given character position and length
	void erase(std::string& str, size_t pos, size_t len);

	//! Erase a interval from a Utf8 string at the given character position and length
	void replace(std::string& dst, size_t pos, size_t len, const char* src);

	//! Returns the Utf8 size in bytes of a given Utf32 character
	size_t byteSize(char32_t c);

	//! Returns the Utf8 size in bytes of a given Utf8 character
	template<typename iterator>
	size_t byteSize(iterator i) {
		size_t s;
        char32_t c;
        start:
        c = i[0];
		if((c&0x80) == 0)
            return 1;
		s = countLeadingOneBits(c<<24);
		if(s == 1) {
			decrement(i);
            goto start;
		} else
			return (s > 4) ? -1 : s;
    }

	//! Returns the Utf32 character at the given Utf8 string iterator
	template<typename iterator>
    char32_t toUtf32(iterator i) {
        char32_t c;
        start:
        c = i[0];
		if((c&0x80) == 0)
            return c;
		switch(countLeadingOneBits(c << 24)) {
			case 1:
                decrement(i);
                goto start;
			case 2:
                return (c&0x1F)<<6 | (i[1]&0x3F);
			case 3:
                return (c&0x0F)<<12 | (i[1]&0x3F)<<6 | (i[2]&0x3F);
			case 4:
                return (c&0x0F)<<18 | (i[1]&0x3F)<<12 | (i[2]&0x3F)<<6 | (i[3]&0x3F);
            default:
                return -1;
		}
    }

	//! Appends a Utf32 character to a Utf8 string
	void append(std::string& str, char32_t c);

	//! Converts a Utf8 string into a Utf32 string
	std::u32string convertToUtf32(const std::string& str);

	//! Converts a Utf32 string into a Utf8 string
	std::string convertFromUtf32(const std::u32string& str);
}
