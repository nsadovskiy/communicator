/**
 *
 *
 **/
#include "utils.hpp"
#include "client.hpp"
#include "protocol_base.hpp"

#include <log4cplus/loggingmacros.h>

using std::basic_ostream;
using boost::asio::buffer;
using boost::asio::io_service;
using boost::system::error_code;
using boost::asio::error::operation_aborted;

/**
 *
 *
 **/
client_t::pointer_type client_t::create(io_service & io_service, protocol_base_t * impl, communicator::backend::base_impl_t & store_backend) {
    return pointer_type(new client_t(io_service, impl, store_backend));
}

/**
 *
 *
 **/
client_t::client_t(io_service & io_service, protocol_base_t * impl, communicator::backend::base_impl_t & store_backend) :
    log_(log4cplus::Logger::getInstance("main")),
    impl_(impl),
    store_backend_(store_backend),
    perm_buffer_(async_buffer_.size()),
    socket_(io_service),
    strand_(io_service) {
}

/**
 *
 *
 **/
client_t::~client_t() {
}

/**
 *
 *
 **/
void client_t::start() {
    impl_->init(this);
    LOG4CPLUS_INFO(log_, "New connection from " << socket_.remote_endpoint());
    socket_.async_read_some(buffer(async_buffer_),
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
    LOG4CPLUS_INFO(log_, "Client " << socket_.remote_endpoint() << " disconnected");
    socket_.close();
}

/**
 *
 *
 **/
void client_t::send(const unsigned char * data, size_t len) {

    // auto b = boost::asio::buffer(data, len);

    // LOG4CPLUS_INFO(log_, "   client_t::send: [" << to_hex_string(data, len).c_str() << "]");
    // LOG4CPLUS_INFO(log_, "   client_t::send: len=" << len);
    // LOG4CPLUS_INFO(log_, "   client_t::send: [" << data[0] << "]");
    
    socket_.async_send(buffer(data, len),
            [this](const error_code & error, size_t len) {
                this->handle_write(error, len);
            }
        );
    LOG4CPLUS_TRACE(log_, "Write complete");
}

/**
 *
 *
 **/
void client_t::handle_read(const error_code & error, size_t len) {

    if (!error) {
        LOG4CPLUS_TRACE(log_, "Readed " << len << " bytes");
        
        if (len && len < async_buffer_.size()) {
            perm_buffer_.assign(&async_buffer_[0], &async_buffer_[len]);
        } else {
            LOG4CPLUS_ERROR(log_, "len=" << len << ", but capacity=" << async_buffer_.size());
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
        LOG4CPLUS_ERROR(log_, error << ". Connection closed.");
        stop();
    }
}

/**
 *
 *
 **/
void client_t::handle_write(const boost::system::error_code & ec, size_t bytes_transferred) {
    LOG4CPLUS_TRACE(log_, "Write succeded");
}
