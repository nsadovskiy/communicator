/**
 *
 *
 **/
#include <vector>
#include <exception>

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/loggingmacros.h>

#include "config.h"
#include "server.hpp"
#include "settings.hpp"
#include "omnicomm/protocol.hpp"


/**
 *
 *
 **/

using std::vector;

void daemonize();

const log4cplus::Logger logger = log4cplus::Logger::getInstance("main");

/**
 *
 *
 **/
int main(int argc, const char * argv[]) {

    log4cplus::PropertyConfigurator::doConfigure("log4cplus.conf");

    try {

        LOG4CPLUS_INFO(logger, "Communicator v" << MAJOR_VERSION << "." << MINOR_VERSION << "." << PATCH_VERSION << ". Build " << BUILD_DATE << ". (C) VistGroup.");

#       if defined(BOOST_ASIO_HAS_EPOLL)
            LOG4CPLUS_INFO(logger, "Compiled for Linux, use epoll");
#       elif defined(BOOST_ASIO_HAS_IOCP)
            LOG4CPLUS_INFO(logger, "Compiled for frigging windows, use IoCompletionPort");
#       elif defined(BOOST_ASIO_HAS_KQUEUE)
            LOG4CPLUS_INFO(logger, "Compiled for FreeBSD or faggot Mac Os X, use kqueue");
#       elif defined(BOOST_ASIO_HAS_DEV_POLL)
            LOG4CPLUS_INFO(logger, "Compiled for Solaris, use /dev/poll");
#       else
            LOG4CPLUS_INFO(logger, "Where am I ?!? O_O");
#       endif

        bool daemon_mode = false;
        vector<communicator::options_t> options;

        get_settings(argc, argv, options, daemon_mode);

        if (options.empty()) {
            throw std::runtime_error("Configuration empty. Nothing to do.");
        }

        for (auto option: options) {
            LOG4CPLUS_TRACE(logger, "Communicator settings:");
            LOG4CPLUS_TRACE(logger, "\t[" << option.name << "] Protocol: " << option.protocol);
            LOG4CPLUS_TRACE(logger, "\t[" << option.name << "] Worker processes: " << option.workers);
            LOG4CPLUS_TRACE(logger, "\t[" << option.name << "] Listen: " << option.listen.ip_addr << ":" << option.listen.tcp_port);
            LOG4CPLUS_TRACE(logger, "\t[" << option.name << "] Storage type: " << option.storage.kind);
            LOG4CPLUS_TRACE(logger, "\t[" << option.name << "] Storage address: " << option.storage.connection.ip_addr << ":" << option.storage.connection.tcp_port);
            LOG4CPLUS_TRACE(logger, "\t[" << option.name << "] Storage name: " << option.storage.db_name);
            LOG4CPLUS_TRACE(logger, "\t[" << option.name << "] Storage login: " << option.storage.login);
            LOG4CPLUS_TRACE(logger, "\t[" << option.name << "] Storage password: ********");
        }

        omnicomm::transport_protocol_t::init_protocol();

        if (daemon_mode) {
            daemonize();
        }

        communicator::server_t server(options[0], omnicomm::transport_protocol_t::create);
        server.run();

    } catch (const std::exception & e) {
        LOG4CPLUS_FATAL(logger, e.what());
    }

    LOG4CPLUS_INFO(logger, "Communicator finished");
}
