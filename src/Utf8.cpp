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

#include "Utf8.h"

namespace utf8 {
    const char bom[] = "\xEF\xBB\xBF";

    void setBOM(std::string& str) {
        if(checkBOM(str.begin()))
            return;
        str.insert(0, bom);
    }

	size_t length(const char* str) {
        return difference(str+strlen(str), str);
	}

	size_t length(const std::string& str) {
        return difference(str.end(), str.begin());
	}

	void insert(std::string& dst, size_t pos, const char* src) {
        auto iterator = dst.begin();
		forwardUntil(iterator, dst.end(), pos);
        dst.insert(iterator - dst.begin(), src);
	}

	void erase(std::string& str, size_t pos, size_t len) {
		auto begin = str.begin(), from = begin;
		forwardUntil(from, str.end(), pos);
		auto to = from;
		forwardUntil(to, str.end(), len);
		str.erase(from, to);
	}

	void replace(std::string& dst, size_t pos, size_t len, const char* src) {
		auto begin(dst.begin()), from = begin;
		forwardUntil(from, dst.end(), pos);
		auto to = from;
		forwardUntil(to, dst.end(), len);
		dst.replace(from-begin, to-from, src);
	}

	size_t byteSize(char32_t c) {
		return (c < 0x800) ? ((c < 0x80) ? 1 : 2) : ((c < 0x10000) ? 3 : 4);
	}

	void append(std::string& str, char32_t c) {
		switch(byteSize(c)) {
			case 1:
			    str.push_back(static_cast<char>(c));
			    break;
			case 2:
    			str.push_back(static_cast<char>(0xC0 | (c >> 6)));
    			str.push_back(static_cast<char>(0x80 | (c & 0x3F)));
    			break;
			case 3:
    			str.push_back(static_cast<char>(0xE0 | (c >> 12)));
    			str.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
    			str.push_back(static_cast<char>(0x80 | (c & 0x3F)));
    			break;
			case 4:
    			str.push_back(static_cast<char>(0xF0 | (c >> 18)));
    			str.push_back(static_cast<char>(0x80 | ((c >> 12) & 0x3F)));
    			str.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
    			str.push_back(static_cast<char>(0x80 | (c & 0x3F)));
    			break;
		}
	}

	std::u32string convertToUtf32(const std::string& str) {
		std::u32string result;
		auto begin = str.begin(), end = str.end();
		for(auto i = begin; i != end; increment(i))
			result.push_back(toUtf32(i));
		return result;
	}

	std::string convertFromUtf32(const std::u32string& str) {
		std::string result;
		for(size_t i = 0; i < str.length(); ++i)
			append(result, str[i]);
		return result;
	}
}
