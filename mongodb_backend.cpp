/**
 *
 *
 **/
#include "mongodb_backend.hpp"

/**
 *
 *
 **/
mongodb_backend_t::mongodb_backend_t(std::string server_ip) {
}

/**
 *
 *
 **/
void mongodb_backend_t::save_messages_impl(const std::string & msg) {
    LOG4CPLUS_INFO(get_log(), "store message [" << msg << "]");
}
