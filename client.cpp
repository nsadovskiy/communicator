/**
 *
 *
 **/
#include <iostream>
#include "client.hpp"

using std::wcout;
using boost::asio::io_service;
using boost::system::error_code;
using boost::asio::error::operation_aborted;

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
    socket_(io_service),
    strand_(io_service) {

    wcout << this << L" Клиент создан\n";
}

/**
 *
 *
 **/
client_t::~client_t() {
    wcout << this  << L" Клиент уничтожен\n";
}

/**
 *
 *
 **/
void client_t::start() {
    wcout << this  << L" Клиент начАл\n";
    socket_.async_read_some(boost::asio::buffer(buffer_),
            strand_.wrap(
                [this](const error_code & error, size_t len) {
                    this->handle_read(error, len);
                }
            )
        );
}

/**
 *
 *
 **/
void client_t::stop() {
    wcout << this  << L" Клиент кончил\n";
    socket_.close();
}

/**
 *
 *
 **/
void client_t::handle_read(const boost::system::error_code & error, size_t len) {
    wcout << this << L" Прочитано " << len << L" байт\n";
    if (!error) {
        socket_.async_read_some(boost::asio::buffer(buffer_),
                strand_.wrap(
                    [this](const error_code & error, size_t len) {
                        this->handle_read(error, len);
                    }
                )
            );
    } else if (error != operation_aborted) {
        wcout << this << L" Йа ашипко! Закрываемся.\n";
        stop();
    }
}
