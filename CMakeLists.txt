CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(communicator)

INCLUDE(FindProtobuf)

FIND_PACKAGE(Threads)
FIND_PACKAGE(Protobuf REQUIRED)
FIND_PACKAGE(Boost 1.53 COMPONENTS system REQUIRED)

SET(Boost_USE_STATIC_LIBS ON)
SET(Boost_USE_MULTITHREADED ON)

SET(SOURCES ${SOURCES}
    main.cpp
    utils.cpp
    server.cpp
    client.cpp
    protocol_base.cpp
)

INCLUDE(omnicomm/CMakeLists.txt)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${PROTOBUF_INCLUDE_DIR})

SET(CMAKE_CXX_FLAGS "-std=c++0x -O2 -Wall -fno-strict-aliasing")

ADD_EXECUTABLE(communicator ${SOURCES})

TARGET_LINK_LIBRARIES(communicator 
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES}
    ${PROTOBUF_LIBRARY}
)
