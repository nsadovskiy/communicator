# - Try to find Oracle SDK
# Once done this will define
#  ORACLE_FOUND - System has Oracle
#  ORACLE_INCLUDE_DIRS - The Oracle include directories
#  ORACLE_LIBRARIES - The libraries needed to use Oracle

SET(ORACLE_HOME $ENV{ORACLE_HOME})


FIND_PATH(ORACLE_INCLUDE_DIRS oci.h
        /usr/include/
        /usr/local/include/
        ${ORACLE_HOME}/include
        ${ORACLE_HOME}/sdk/include
        ${ORACLE_HOME}/OCI/include
        ${ORACLE_HOME}/rdbms/public
    )

FIND_LIBRARY(
        ORACLE_CLNTSH_LIBRARY
        NAMES clntsh oci
        PATHS
            /usr
            ${ORACLE_HOME}
            ${ORACLE_HOME}/oci/lib/msvc
    )

FIND_LIBRARY(
        ORACLE_OCCI_LIBRARY
        NAMES occi oraocci11
        PATHS
            /usr
            ${ORACLE_HOME}
            ${ORACLE_HOME}/oci/lib/msvc
    )

SET(ORACLE_LIBRARIES
        ${ORACLE_OCCI_LIBRARY}
        ${ORACLE_CLNTSH_LIBRARY}
    )

SET(ORACLE_LIBRARIES ${ORACLE_LIBRARIES} )
SET(ORACLE_INCLUDE_DIRS ${ORACLE_INCLUDE_DIRS} )

INCLUDE(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set ORACLE_FOUND to TRUE
# if all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ORACLE DEFAULT_MSG
                                  ORACLE_LIBRARIES ORACLE_INCLUDE_DIRS)

MARK_AS_ADVANCED(ORACLE_INCLUDE_DIRS ORACLE_LIBRARIES)
