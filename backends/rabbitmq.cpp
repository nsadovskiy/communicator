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
    store_backend_t(login, password, path) {
}

/**
 *
 *
 **/
communicator::backend::rabbitmq_t::~rabbitmq_t() {
}

/**
 *
 *
 **/
void communicator::backend::rabbitmq_t::save_messages_impl(const std::string & msg) {
}

/**
 *
 *
 **/
