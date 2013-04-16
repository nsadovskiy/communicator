/**
 *
 *
 **/
#include <iostream>
#include "server.hpp"

using std::wcout;
using boost::system::error_code;

/**
 *
 *
 **/
server_t::server_t(const char * bind_addr, const char * port, size_t num_workers) :
    num_workers_(num_workers),
    signals_(io_service_),
    acceptor_(io_service_) {

    signals_.add(SIGINT);
    signals_.add(SIGTERM);
#if defined(SIGQUIT)
    signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)
    signals_.async_wait(
        [this](const error_code &, const int &) {
            this->handle_stop();
        }
    );
}

/**
 *
 *
 **/
void server_t::run() {
    io_service_.run();
}

/**
 *
 *
 **/
void server_t::handle_stop() {
    io_service_.stop();
    wcout << L"Bye!\n";
}
