/**
 *
 *
 **/
#include <iostream>
#include <exception>

#include <boost/asio.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>

#include "config.h"
#include "server.hpp"
#include "omnicomm/protocol.hpp"

using std::cerr;

/**
 *
 *
 **/
bool fill_settings(int argc, const char * argv[], communicator::settings_t & settings) {

    boost::cmatch match;
    // boost::regex re("(\\w+)://((\\w+):(\\w+)@)?(([^/:]+)(:(\\d+))?/)?(.+)?");
    boost::regex re("(\\w+)://((\\w+):(\\w+)@)?(.+)?");

    if (argc < 5 || !boost::regex_match(argv[4], match, re)) {
        return false;
    }

    // for (auto m: match) {
    //     cerr << "Match: " << m << "\n";
    // }

    settings.num_workers = boost::lexical_cast<size_t>(argv[3]);
    settings.listen.ip_addr = argv[1];
    settings.listen.port = argv[2];
    settings.store.protocol = match[1];
    settings.store.username = match[3];
    settings.store.password = match[4];
    settings.store.path = match[5];

    return true;
}

/**
 *
 *
 **/
int main(int argc, const char * argv[]) {

    log4cplus::PropertyConfigurator::doConfigure("log4cplus.config");
    const log4cplus::Logger logger = log4cplus::Logger::getInstance("main");

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

        communicator::settings_t settings;

        if (!fill_settings(argc, argv, settings)) {
            cerr << "Usage: communicator <ip-address> <port> <threads> <store>\n";
            cerr << "  <ip-address>  - listened address to bind\n";
            cerr << "  <port>        - listened port\n";
            cerr << "  <threads>     - number woring threads\n";
            cerr << "  <store>       - storage connection protocol://login:password@path\n";
            cerr << "\n";
            cerr << "Example:\n";
            cerr << "  communicator 0.0.0.0 4010 4 rabbitmq://guest:guest@path/\n";
            exit(1);
        }

        communicator::server_t server(settings, omnicomm::protocol_t::create);
        server.run();

    } catch (const std::exception & e) {
        LOG4CPLUS_FATAL(logger, e.what());
    }

    LOG4CPLUS_INFO(logger, "Communicator finished");
}
