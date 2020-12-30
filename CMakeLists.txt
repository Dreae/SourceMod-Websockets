cmake_minimum_required(VERSION 3.5)
project(websocket)

set(SOURCE_FILES
    src/smsdk_ext.cpp
    src/extension.cpp
    src/event_loop.cpp
    src/websocket_connection_base.cpp
    src/websocket_connection_ssl.cpp
    src/websocket_connection.cpp
    src/smn_json.cpp
    src/smn_websocket.cpp
    CxxUrl/url.cpp)

if(NOT TARGET)
    set(TARGET linux)
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
    add_definitions(-DWIN32)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L$ENV{HOME}/boost_1_75_0/stage/lib -std=c++17 -pipe -DSOURCEMOD_BUILD -fno-strict-aliasing -m32")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3")
endif()

if(${TARGET} STREQUAL linux)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_LINUX")
endif()

set(CMAKE_SKIP_BUILD_RPATH true)
set(CMAKE_VERBOSE_MAKEFILE false)

include_directories(sourcemod/public sourcepawn/include amtl amtl/amtl json/include CxxUrl)

add_library(websocket.ext SHARED ${SOURCE_FILES})

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_link_libraries(websocket.ext)
else()
    include_directories($ENV{HOME}/boost_1_75_0)
    target_link_libraries(websocket.ext -lpthread -lcrypto -lssl -Wl,--no-undefined -Wl,-Bstatic -lboost_system -static-libstdc++ -Wl,-Bdynamic)
endif()

set_target_properties(websocket.ext PROPERTIES PREFIX "")

if(NOT ${TARGET} STREQUAL linux)
    set_target_properties(websocket.ext PROPERTIES SUFFIX ".dll")
endif()