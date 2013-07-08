/**
 *
 *
 **/
#include <sstream>
#include <stdexcept>
#include "utils.hpp"
#include "client.hpp"
#include "protocol_base.hpp"

#include <log4cplus/loggingmacros.h>

using std::exception;
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

    try {

        std::ostringstream os;
        os << socket_.remote_endpoint();
        endpoint_name_ = os.str();
    
        impl_->init(this);
        LOG4CPLUS_INFO(log_, "[net]{" << endpoint_name_ << "} New connection established");
        LOG4CPLUS_TRACE(log_, "[net]{" << endpoint_name_ << "} Start async read");
        socket_.async_read_some(buffer(async_buffer_),
                strand_.wrap(
                    [this](const error_code & error, size_t len) {
                        this->handle_read(error, len);
                    }
                )
            );

    } catch(const exception & e) {
        LOG4CPLUS_ERROR(log_, "[net]{" << endpoint_name_ << "} " << e.what());
        stop();

    } catch (...) {
        LOG4CPLUS_ERROR(log_, "[net]{" << endpoint_name_ << "} Unexpected exception");
        stop();
    }
}

/**
 *
 *
 **/
void client_t::stop() {
    
    try {
    
        LOG4CPLUS_INFO(log_, "[net]{" << endpoint_name_ << "} Client disconnected");
        socket_.close();
    
    } catch(const exception & e) {
        LOG4CPLUS_ERROR(log_, "[net]{" << endpoint_name_ << "} " << e.what());

    } catch (...) {
        LOG4CPLUS_ERROR(log_, "[net]{" << endpoint_name_ << "} Unexpected exception");
    }
}

/**
 *
 *
 **/
void client_t::send(const unsigned char * data, size_t len) {

    LOG4CPLUS_TRACE(log_, "[net]{" << endpoint_name_ << "} send [" << to_hex_string(data, len).c_str() << "]");

    try {
        LOG4CPLUS_TRACE(log_, "[net]{" << endpoint_name_ << "} Start async write");
        socket_.async_send(buffer(data, len),
                [this](const error_code & error, size_t len) {
                    this->handle_write(error, len);
                }
            );

    } catch(const exception & e) {
        LOG4CPLUS_ERROR(log_, "[net]{" << endpoint_name_ << "} " << e.what());
        stop();

    } catch (...) {
        LOG4CPLUS_ERROR(log_, "[net]{" << endpoint_name_ << "} Unexpected exception");
        stop();
    }
}

/**
 *
 *
 **/
void client_t::handle_read(const error_code & error, size_t len) {

    try {

        if (!error) {

            if (len && len < async_buffer_.size()) {
                perm_buffer_.assign(&async_buffer_[0], &async_buffer_[len]);
            } else {
                throw std::length_error("Input buffer length less than data size");
                // LOG4CPLUS_ERROR(log_, "[" << endpoint_name_ << "] len=" << len << ", but capacity=" << async_buffer_.size());
                // perm_buffer_.clear();
            }

            LOG4CPLUS_TRACE(log_, "[net]{" << endpoint_name_ << "} Readed " << len << " bytes [" << to_hex_string(&perm_buffer_[0], perm_buffer_.size()).c_str() << "]");

            LOG4CPLUS_TRACE(log_, "[net]{" << endpoint_name_ << "} Start async read");
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
            LOG4CPLUS_ERROR(log_, "[net]{" << endpoint_name_ << "} " << error << ". Connection closed.");
            stop();
        }

    } catch(const exception & e) {
        LOG4CPLUS_ERROR(log_, "[net]{" << endpoint_name_ << "} " << e.what());
        stop();

    } catch (...) {
        LOG4CPLUS_ERROR(log_, "[net]{" << endpoint_name_ << "} Unexpected exception");
        stop();
    }
}

/**
 *
 *
 **/
void client_t::handle_write(const error_code & error, size_t bytes_transferred) {

    if (!error) {
        LOG4CPLUS_TRACE(log_, "[net]{" << endpoint_name_ << "} Async write complete successful");

    } else {

        if (error != operation_aborted) {
            LOG4CPLUS_ERROR(log_, "[net]{" << endpoint_name_ << "} " << error << ". Connection closed.");
            stop();
        } else {
            LOG4CPLUS_TRACE(log_, "[net]{" << endpoint_name_ << "} " << error << ". Async operation terminated.");
        }
    }
}
