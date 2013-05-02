/**
 *
 *
 **/
#include "rabbitmq.hpp"
#include <amqp_tcp_socket.h>
#include <amqp.h>
#include <amqp_framing.h>

/**
 *
 *
 **/
communicator::backend::rabbitmq_t::rabbitmq_t(const std::string & login, const std::string & password, const std::string & path) :
    base_impl_t(login, password, path) {

    LOG4CPLUS_TRACE(get_log(), "RabbitMQ backend created");
}

/**
 *
 *
 **/
communicator::backend::rabbitmq_t::~rabbitmq_t() {
    LOG4CPLUS_TRACE(get_log(), "RabbitMQ backend destroyed");
}

/**
 *
 *
 **/
void communicator::backend::rabbitmq_t::save_message_impl(const std::string & msg) {
}

/**
 *
 *
 **/
