/**
 *
 *
 **/
#include <iostream>
#include "server.hpp"
#include "client.hpp"

using std::cout;
using std::wcout;
using boost::shared_ptr;
using boost::asio::ip::tcp;
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
#endif
    signals_.async_wait(
        [this](const error_code &, const int &) {
            this->handle_stop();
        }
    );

    tcp::resolver resolver(io_service_);
    tcp::resolver::query query(bind_addr, port);
    tcp::endpoint endpoint = *resolver.resolve(query);
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();

    start_accept();
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
void server_t::start_accept() {

    client_type client(client_t::create(io_service_, nullptr));
    acceptor_.async_accept(
        client->get_socket(),
        [this, client](const error_code & error) {
            this->handle_accept(error, client);
        }
    );
}

/**
 *
 *
 **/
void server_t::handle_stop() {
    io_service_.stop();
    wcout << L"Bye!\n";
}

/**
 *
 *
 **/
void server_t::handle_accept(const boost::system::error_code & error, client_type client) {

    wcout << L"Connection accepted\n";

    if (!acceptor_.is_open()) {
        return;
    }

    if (!error) {
        client->start();
        clients_.push_back(client);
    }

    start_accept();
}
