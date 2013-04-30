# - Try to find rabbitmq-c
# Once done this will define
#  RABBITMQ_FOUND - System has rabbitmq-c
#  RABBITMQ_INCLUDE_DIRS - The rabbitmq-c include directories
#  RABBITMQ_LIBRARIES - The libraries needed to use rabbitmq-c


FIND_PATH(RABBITMQ_INCLUDE_DIRS amqp.h
    /usr/include/
    /usr/local/include/)

FIND_LIBRARY(RABBITMQ_LIBRARIES NAMES rabbitmq
             PATHS /usr)

SET(RABBITMQ_LIBRARIES ${RABBITMQ_LIBRARIES} )
SET(RABBITMQ_INCLUDE_DIRS ${RABBITMQ_INCLUDE_DIRS} )

INCLUDE(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set RabbitMQ_FOUND to TRUE
# if all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(RABBITMQ DEFAULT_MSG
                                  RABBITMQ_LIBRARIES RABBITMQ_INCLUDE_DIRS)

MARK_AS_ADVANCED(RABBITMQ_INCLUDE_DIRS RABBITMQ_LIBRARIES)
