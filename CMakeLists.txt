CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(communicator)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

SET(Boost_USE_STATIC_LIBS ON)
SET(Boost_USE_MULTITHREADED ON)

INCLUDE(FindProtobuf)
INCLUDE(FindLog4cplus)

FIND_PACKAGE(Threads)
FIND_PACKAGE(MongoDB)
FIND_PACKAGE(Protobuf REQUIRED)
FIND_PACKAGE(Log4cplus REQUIRED)
FIND_PACKAGE(Boost 1.49 COMPONENTS system thread filesystem program_options REQUIRED)

SET(SOURCES ${SOURCES}
    main.cpp
    utils.cpp
    server.cpp
    client.cpp
    protocol_base.cpp
    store.cpp
)

IF (MongoDB_FOUND)
    MESSAGE("-- MongoDB backend support enabled")
    SET(SOURCES ${SOURCES}
        mongodb_backend.cpp
    )
ENDIF(MongoDB_FOUND)

INCLUDE(omnicomm/CMakeLists.txt)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${MongoDB_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${PROTOBUF_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${LOG4CPLUS_INCLUDE_DIRS})

# SET(CMAKE_CXX_FLAGS "-std=c++0x -O2 -Wall -fno-strict-aliasing")
SET(CMAKE_CXX_FLAGS "-std=c++0x -O2 -Wall")

ADD_EXECUTABLE(communicator ${SOURCES})

TARGET_LINK_LIBRARIES(communicator 
    ${CMAKE_THREAD_LIBS_INIT}
    ${MongoDB_LIBRARIES}
    ${Boost_LIBRARIES}
    ${PROTOBUF_LIBRARY}
    ${LOG4CPLUS_LIBRARIES}
)

SET(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/bin")
INSTALL(TARGETS communicator DESTINATION .)
INSTALL(FILES log4cplus.config DESTINATION .)
