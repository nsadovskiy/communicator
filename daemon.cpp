/**
 *
 *
 **/
#include <unistd.h>
#include <stdexcept>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>


namespace {
    const log4cplus::Logger logger = log4cplus::Logger::getInstance("main");
}


/**
 *
 *
 **/
void daemonize() {

    LOG4CPLUS_INFO(logger, "Entering daemon mode");

    if (0 != daemon(1, 0)) {
        throw std::runtime_error("Error entering daemon mode");
    }
}
