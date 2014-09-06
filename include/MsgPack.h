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