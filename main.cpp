/**
 *
 **/
#include <iostream>
#include <exception>

#include <boost/asio.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>

#include "server.hpp"
#include "omnicomm/protocol.hpp"

using namespace log4cplus;

using std::cerr;
using std::exception;
using boost::lexical_cast;

int main(int argc, const char * argv[]) {

    PropertyConfigurator::doConfigure("log4cplus.config");
    const Logger logger = Logger::getInstance("main");

    LOG4CPLUS_INFO(logger, "Starting Communicator");

#if defined(BOOST_ASIO_HAS_EPOLL)
    LOG4CPLUS_INFO(logger, "Compiled for Linux, use epoll");
#elif defined(BOOST_ASIO_HAS_IOCP)
    LOG4CPLUS_INFO(logger, "Compiled for frigging windows, use IoCompletionPort");
#elif defined(BOOST_ASIO_HAS_KQUEUE)
    LOG4CPLUS_INFO(logger, "Compiled for FreeBSD or faggot Mac Os X, use kqueue");
#elif defined(BOOST_ASIO_HAS_DEV_POLL)
    LOG4CPLUS_INFO(logger, "Compiled for Solaris, use /dev/poll");
#else
    LOG4CPLUS_INFO(logger, "Where am I ?!? O_O");
#endif

    boost::cmatch match;
    boost::regex re("(\\w+)://((\\w+):(\\w+)@)?(.*)?");

    if (argc < 5 || !boost::regex_match(argv[4], match, re)) {
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

    try {
        server_t server(argv[1], argv[2], lexical_cast<size_t>(argv[3]), omnicomm::protocol_t::create);
        server.run();

    } catch (const exception & e) {
        LOG4CPLUS_FATAL(logger, e.what());
    }
    LOG4CPLUS_INFO(logger, "Execution finished");
}
