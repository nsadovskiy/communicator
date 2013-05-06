# - Try to find Log4cplus
# Once done this will define
#  LOG4CPLUS_FOUND - System has Log4cplus
#  LOG4CPLUS_INCLUDE_DIRS - The Log4cplus include directories
#  LOG4CPLUS_LIBRARIES - The libraries needed to use Log4cplus

message("Log4cplus - " ${LOG4CLPUS_ROOT_FOLDER})

find_path(LOG4CPLUS_INCLUDE_DIR 
		NAMES log4cplus/config.hxx
		PATHS
          ${LOG4CLPUS_ROOT_FOLDER}
          ${LOG4CLPUS_ROOT_FOLDER}/include
    )

find_library(LOG4CPLUS_LIBRARY
		NAMES log4cplus log4cplusU
        PATHS
        	/usr
        	${LOG4CLPUS_ROOT_FOLDER}
        	${LOG4CLPUS_ROOT_FOLDER}/lib
    )

set(LOG4CPLUS_LIBRARIES ${LOG4CPLUS_LIBRARY} )
set(LOG4CPLUS_INCLUDE_DIRS ${LOG4CPLUS_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Log4cplus  DEFAULT_MSG
                                  LOG4CPLUS_LIBRARY LOG4CPLUS_INCLUDE_DIR)

mark_as_advanced(LOG4CPLUS_INCLUDE_DIR LOG4CPLUS_LIBRARY )
