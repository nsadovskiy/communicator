/**
 *
 *
 **/
#include <string>
#include <sstream>
#include <iostream>
#include <exception>
#include <stdexcept>

#include <libconfig.h++>

#include <boost/asio.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/loggingmacros.h>

#include "config.h"
#include "server.hpp"
#include "omnicomm/protocol.hpp"

using std::cerr;
using std::string;

void daemonize();

const log4cplus::Logger logger = log4cplus::Logger::getInstance("main");

/**
 *
 *
 **/
bool read_settings_from_file(const char * config_path, communicator::settings_t & settings) {

    using std::exception;
    using std::runtime_error;
    using std::ostringstream;
    using namespace libconfig;

    Config config;

    try {
        config.readFile(config_path);
    }
    catch (const FileIOException & e) {
        ostringstream err;
        err << "Error opening cofiguration file '" << config_path << "'";
        throw runtime_error(err.str());
    }
    catch(const ParseException & e) {
        ostringstream err;
        err << "Parse error at " << e.getFile() << ":" << e.getLine() << " - " << e.getError();
        throw runtime_error(err.str());
    }
    catch (const runtime_error & e) {
        throw;
    }
    catch (const exception & e) {
        throw runtime_error(e.what());
    }
    catch (...) {
        throw runtime_error("Unexpected exception while reading configuration file");
    }
    

    return true;
}

/**
 *
 *
 **/
bool fill_settings(int argc, const char * argv[], communicator::settings_t & settings) {

    if (argc < 2) {
        settings.daemon_mode = false;
        return read_settings_from_file("communicator.conf", settings);
    }


    string mode = argv[1];
    if (mode != "daemon" && mode != "nodaemon") {
        return false;
    }

    settings.daemon_mode = (mode == "daemon");


    if (argc == 2) {
        return read_settings_from_file("communicator.conf", settings);
    }


    if (argc == 3) {
        return read_settings_from_file(argv[2], settings);
    }


    boost::cmatch match;
    // boost::regex re("(\\w+)://((\\w+):(\\w+)@)?(([^/:]+)(:(\\d+))?/)?(.+)?");
    boost::regex re("(\\w+)://((\\w+):(\\w+)@)?(.+)?");

    if (argc < 6 || !boost::regex_match(argv[5], match, re)) {
        return false;
    }

    // for (auto m: match) {
    //     cerr << "Match: " << m << "\n";
    // }

    settings.num_workers = boost::lexical_cast<size_t>(argv[4]);
    settings.listen.ip_addr = argv[2];
    settings.listen.port = argv[3];
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

        communicator::settings_t settings;

        if (!fill_settings(argc, argv, settings)) {
            cerr << "Usage: communicator <mode> <ip-address> <port> <threads> <store>\n";
            cerr << "  <daemon|nodaemon>  - run mode console or daemon\n";
            cerr << "  <ip-address>       - listened address to bind\n";
            cerr << "  <port>             - listened port\n";
            cerr << "  <threads>          - number woring threads\n";
            cerr << "  <store>            - storage connection protocol://login:password@path\n";
            cerr << "\n";
            cerr << "Example:\n";
            cerr << "  communicator nodaemon 0.0.0.0 4010 4 oracle://login:password@db-server/database_sid\n";
            exit(1);
        }

        omnicomm::transport_protocol_t::init_protocol();

        if (settings.daemon_mode) {
            daemonize();
        }

        communicator::server_t server(settings, omnicomm::transport_protocol_t::create);
        server.run();

    } catch (const std::exception & e) {
        LOG4CPLUS_FATAL(logger, e.what());
    }

    LOG4CPLUS_INFO(logger, "Communicator finished");
}
