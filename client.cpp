/**
 *
 *
 **/
#include <iostream>
#include "client.hpp"
#include "protocol_base.hpp"

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
client_t::pointer_type client_t::create(io_service & io_service, protocol_base_t * impl) {
    return pointer_type(new client_t(io_service, impl));
}

/**
 *
 *
 **/
client_t::client_t(io_service & io_service, protocol_base_t * impl) :
    impl_(impl),
    perm_buffer_(async_buffer_.size()),
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
    socket_.async_read_some(buffer(async_buffer_),
            strand_.wrap(
                [this](const error_code & error, size_t len) {
                    this->handle_read(error, len);
                }
            )
        );
    impl_->init();
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

    if (!error) {
        
        wcout << *this << L" Прочитано " << len << L" байт\n";
        
        if (len && len < async_buffer_.size()) {
            perm_buffer_.assign(&async_buffer_[0], &async_buffer_[len]);
        } else {
            wcout << *this << L" [ERROR][handle_read] len=" << len << L", а capacity=" << async_buffer_.size() << "\n";
            perm_buffer_.clear();
        }
        
        socket_.async_read_some(buffer(async_buffer_),
                strand_.wrap(
                    [this](const error_code & error, size_t len) {
                        this->handle_read(error, len);
                    }
                )
            );
        
        if (!perm_buffer_.empty()) {
            impl_->recive(&perm_buffer_[0], perm_buffer_.size());
        }

    } else if (error != operation_aborted) {
        wcout << *this << L" [ERROR] " << error << L"\n";
        wcout << *this << L" Закрываем соединение.\n";
        stop();
    }
}
