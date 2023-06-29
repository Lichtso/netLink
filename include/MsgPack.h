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

#include "Primitive.h"
#include "Data.h"
#include "Number.h"
#include "Container.h"
#include "StreamManager.h"

namespace MsgPack {
	//Convenience factory methods that let the compiler decide which type to use (implicit)

	//! Make unique MsgPack::Primitive filled with nothing (nil)
    std::unique_ptr<MsgPack::Element> Factory();
	//! Make unique MsgPack::Primitive from boolean
	std::unique_ptr<MsgPack::Element> Factory(bool value);
	//! Make unique MsgPack::String from null terminated c string
	std::unique_ptr<MsgPack::Element> Factory(const char* str);
	//! Make unique MsgPack::String from std::string
	std::unique_ptr<MsgPack::Element> Factory(const std::string& str);
	//! Make unique MsgPack::Number from uint64_t
	std::unique_ptr<MsgPack::Element> Factory(uint64_t value);
	//! Make unique MsgPack::Number from int64_t
	std::unique_ptr<MsgPack::Element> Factory(int64_t value);
	//! Make unique MsgPack::Number from float
	std::unique_ptr<MsgPack::Element> Factory(float value);
	//! Make unique MsgPack::Number from double
	std::unique_ptr<MsgPack::Element> Factory(double value);
	
	//!Convenience factory method for MsgPack::Elements which need explicit types or additional arguments
	#define MsgPack__Factory(x) std::unique_ptr<MsgPack::Element>(new MsgPack::x)
	
};