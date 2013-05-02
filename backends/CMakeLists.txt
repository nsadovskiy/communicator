CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(SOURCES ${SOURCES}
    backends/base.cpp
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

IF (ORACLE_FOUND)
    SET(SOURCES ${SOURCES}
        backends/oracle.cpp
    )
ENDIF(ORACLE_FOUND)
