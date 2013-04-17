/**
 *
 *
 **/
#include <iostream>
#include "client.hpp"

using std::wcout;
using std::basic_ostream;
using boost::asio::buffer;
using boost::asio::io_service;
using boost::system::error_code;
using boost::asio::error::operation_aborted;

/**
 *
 *
 **/
namespace {

    template<class T>
    basic_ostream<T> & operator <<(basic_ostream<T> & stream, client_t & client) {
        stream << L"[" << &client << L"]";
        auto & s = client.get_socket();
        if (s.is_open()) {
            stream << L"[" << s.remote_endpoint() << L"]";
        }
        return stream;
    }
}

/**
 *
 *
 **/
client_t::pointer_type client_t::create(io_service & io_service, base_protocol_t * impl) {
    return pointer_type(new client_t(io_service, impl));
}

/**
 *
 *
 **/
client_t::client_t(io_service & io_service, base_protocol_t * impl) :
    impl_(impl),
    socket_(io_service),
    strand_(io_service) {

    wcout << *this << L" Клиент создан\n";
}

/**
 *
 *
 **/
client_t::~client_t() {
    wcout << *this  << L" Клиент уничтожен\n";
}

/**
 *
 *
 **/
void client_t::start() {
    wcout << *this << L" Прибыл клиент.\n";
    socket_.async_read_some(buffer(buffer_),
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
    wcout << *this << L" Клиент кончил\n";
    socket_.close();
}

/**
 *
 *
 **/
void client_t::handle_read(const error_code & error, size_t len) {
    wcout << *this << L" Прочитано " << len << L" байт\n";
    if (!error) {
        socket_.async_read_some(buffer(buffer_),
                strand_.wrap(
                    [this](const error_code & error, size_t len) {
                        this->handle_read(error, len);
                    }
                )
            );
    } else if (error != operation_aborted) {
        wcout << *this << L" [ERROR] " << error << L"\n";
        wcout << *this << L" Закрываем соединение.\n";
        stop();
    }
}
