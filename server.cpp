/**
 *
 *
 **/
#include <cassert>
#include <iostream>
#include <algorithm>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "server.hpp"
#include "client.hpp"

using std::wcout;
using std::distance;
using std::remove_if;
using boost::asio::ip::tcp;
using boost::system::error_code;
using boost::posix_time::seconds;

/**
 *
 *
 **/
server_t::server_t(const char * bind_addr, const char * port, size_t num_workers, create_func_type create_func) :
    interval_(5),
    num_workers_(num_workers),
    create_client_func_(create_func),
    signals_(io_service_),
    acceptor_(io_service_),
    timer_(io_service_, seconds(interval_)) {

    assert(create_client_func_);

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

    timer_.async_wait(
            [this](const error_code & error) {
                this->handle_timer(error);
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

    assert(create_client_func_);

    client_type client(client_t::create(io_service_, create_client_func_()));
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
void server_t::handle_timer(const error_code & error) {
    // wcout << L"Timer\n";
    if (!error) {
        timer_.expires_at(timer_.expires_at() + seconds(interval_));
        timer_.async_wait(
            [this](const error_code & error) {
                this->handle_timer(error);
            }
        );

        drop_unused_clients();
    }
}

/**
 *
 *
 **/
void server_t::handle_accept(const error_code & error, client_type client) {

    wcout << L"Connection accepted\n";

    if (!acceptor_.is_open()) {
        return;
    }

    if (!error) {
        client->start();
        {
            lock_guard_type lock(mutex_);
            clients_.push_back(client);
        }
    }

    start_accept();
}

/**
 *
 *
 **/
void server_t::drop_unused_clients() {

    lock_guard_type lock(mutex_);
    auto pos = remove_if(
            clients_.begin(),
            clients_.end(),
            [](const client_type & client) {
                return !client->get_socket().is_open();
            }
        );
    if (pos != clients_.end()) {
        wcout << L"Отключенных клиентов " << distance(pos, clients_.end()) << L". Удаляем.\n";
        clients_.erase(pos, clients_.end());
    }
}
