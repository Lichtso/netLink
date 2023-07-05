cmake_minimum_required(VERSION 3.1)
cmake_policy(SET CMP0054 NEW)
project(netLink)

set(OUTPUT_DIR out)
set(SOURCE_DIR src)
set(EXAMPLES_DIR ${SOURCE_DIR}/examples)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

include_directories(PUBLIC include)
aux_source_directory(${SOURCE_DIR} SOURCES)
add_library(shared SHARED ${SOURCES})
add_library(static STATIC ${SOURCES})
set_target_properties(shared PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
set_target_properties(static PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
set_target_properties(shared PROPERTIES DEBUG_POSTFIX -d)
set_target_properties(static PROPERTIES DEBUG_POSTFIX -d)

if(WIN32)
    target_link_libraries(shared ws2_32)
    # https://msdn.microsoft.com/en-us/library/6sehtctf.aspx
    if(${CMAKE_SYSTEM_VERSION} EQUAL "6.1") # Windows 7
        add_definitions(-D_WIN32_WINNT=0x0601 -DWINVER=0x0601)
    elseif(${CMAKE_SYSTEM_VERSION} EQUAL "6.2") # Windows 8
        add_definitions(-D_WIN32_WINNT=0x0602 -DWINVER=0x0602)
    elseif(${CMAKE_SYSTEM_VERSION} EQUAL "6.3") # Windows 8.1
        add_definitions(-D_WIN32_WINNT=0x0603 -DWINVER=0x0603)
    elseif(${CMAKE_SYSTEM_VERSION} EQUAL "10.0") # Windows 10
        add_definitions(-D_WIN32_WINNT=0x0A00 -DWINVER=0x0A00)
    else()
        message(SEND_ERROR "Unknown windows version")
    endif()
endif(WIN32)

if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    add_definitions(-O3 -std=c++11)
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_definitions(-stdlib=libc++)
endif()

add_executable(example_tcp ${EXAMPLES_DIR}/tcp.cpp)
add_executable(example_udp ${EXAMPLES_DIR}/udp.cpp)
add_dependencies(example_tcp shared static)
add_dependencies(example_udp shared static)
target_link_libraries(example_tcp shared)
target_link_libraries(example_udp shared)

file(GLOB INCLUDE_HEADERS "include/*.h")
install(FILES ${INCLUDE_HEADERS} DESTINATION include/${CMAKE_PROJECT_NAME})
install(TARGETS shared DESTINATION lib)
