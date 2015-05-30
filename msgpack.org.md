# MsgPack v5 implementation for C++ 11

[![Build Status](https://travis-ci.org/Lichtso/netLink.svg)](https://travis-ci.org/Lichtso/netLink)
[![License](https://img.shields.io/badge/License-ZLIB-brightgreen.svg)](http://en.wikipedia.org/wiki/Zlib_License)

## Features
* std::streambuf serializer and deserializer
* hierarchy or token stream
* push and pull parser
* byte wise data flow control

## Small Example

    MsgPack::Serializer serializer(socket);  
    std::vector<std::unique_ptr<MsgPack::Element>> arrayWithoutElements, arrayWith3Elements;
    arrayWith3Elements.push_back(MsgPack::Factory(true));
    arrayWith3Elements.push_back(MsgPack__Factory(Array(std::move(arrayWithoutElements))));
    arrayWith3Elements.push_back(MsgPack::Factory("Hello World!"));  
    serializer << MsgPack__Factory(Array(std::move(arrayWith3Elements)));

    MsgPack::Deserializer deserializer(socket);  
    deserializer.deserialize([](std::unique_ptr<MsgPack::Element> parsed) {
        std::cout << "Parsed: " << *parsed << "\n";
        return false;
    }, true);

## Read More

[Tutorial](https://github.com/Lichtso/netLink/wiki/MsgPack)
