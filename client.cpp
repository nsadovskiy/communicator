/**
 *
 *
 **/
#include <iostream>
#include "client.hpp"

using std::wcout;

/**
 *
 *
 **/
client_t::pointer_type client_t::create(boost::asio::io_service & io_service, base_protocol_t * impl) {
    return pointer_type(new client_t(io_service, impl));
}

/**
 *
 *
 **/
client_t::client_t(boost::asio::io_service & io_service, base_protocol_t * impl) :
    impl_(impl),
    socket_(io_service) {

    wcout << this << L" Клиент создан\n";
}

/**
 *
 *
 **/
client_t::~client_t() {
    wcout << this  << L" Клиент кончил\n";
}

/**
 *
 *
 **/
void client_t::start() {
    wcout << this  << L" Клиент начАл\n";
}
