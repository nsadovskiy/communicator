/**
 *
 *
 **/
#include "protocol_base.hpp"

/**
 *
 *
 **/
protocol_base_t::protocol_base_t() :
    manipulator_(nullptr),
    log_(log4cplus::Logger::getInstance("main")) {

    // LOG4CPLUS_TRACE(log_, "constructor");
}

/**
 *
 *
 **/
protocol_base_t::~protocol_base_t() {
    // LOG4CPLUS_TRACE(log_, "destructor");
}

/**
 *
 *
 **/
void protocol_base_t::init(client_t * manipulator) {
    manipulator_ = manipulator;
    LOG4CPLUS_TRACE(log_, "init " << manipulator_);
    init_impl();
}

/**
 *
 *
 **/
void protocol_base_t::connect() {
    // LOG4CPLUS_TRACE(log_, "connect");
    connect_impl();
}

/**
 *
 *
 **/
void protocol_base_t::recive(const unsigned char * data, size_t len) {
    // LOG4CPLUS_TRACE(log_, "recive");
    recive_impl(data, len);
}
