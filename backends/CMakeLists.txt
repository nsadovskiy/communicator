CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(SOURCES ${SOURCES}
    backends/store.cpp
    backends/factory.cpp
)

IF (MongoDB_FOUND)
    SET(SOURCES ${SOURCES}
        backends/mongodb.cpp
    )
ENDIF(MongoDB_FOUND)

IF (RABBITMQ_FOUND)
    SET(SOURCES ${SOURCES}
        backends/rabbitmq.cpp
    )
ENDIF(RABBITMQ_FOUND)
