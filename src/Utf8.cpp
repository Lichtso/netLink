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

#include "Utf8.h"

namespace utf8 {
    const char bom[] = { static_cast<char>(0xEF), static_cast<char>(0xBB), static_cast<char>(0xBF), 0 };

    void setBOM(std::string& str) {
        if(checkBOM(str.begin())) return;
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
			str.push_back(c);
			break;
			case 2:
			str.push_back(0xC0 | (c >> 6));
			str.push_back(0x80 | (c & 0x3F));
			break;
			case 3:
			str.push_back(0xE0 | (c >> 12));
			str.push_back(0x80 | ((c >> 6) & 0x3F));
			str.push_back(0x80 | (c & 0x3F));
			break;
			case 4:
			str.push_back(0xF0 | (c >> 18));
			str.push_back(0x80 | ((c >> 12) & 0x3F));
			str.push_back(0x80 | ((c >> 6) & 0x3F));
			str.push_back(0x80 | (c & 0x3F));
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
		for(size_t i = 0; i < str.length(); ++ i)
			append(result, str[i]);
		return result;
	}
}
