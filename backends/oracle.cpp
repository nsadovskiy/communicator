/**
 *
 *
 **/
#include "oracle.hpp"

#include <stdexcept>
#include <boost/regex.hpp>

using namespace oracle::occi;

/**
 *
 *
 **/
communicator::backend::oracle_t::oracle_t(const std::string & login, const std::string & password, const std::string & path) :
    base_impl_t(login, password, path),
    environment_(Environment::createEnvironment(Environment::DEFAULT), Environment::terminateEnvironment) {

    parse_path();

    int major, minor, update, patch, port;
    Environment::getClientVersion(major, minor, update, patch, port);
    LOG4CPLUS_INFO(get_log(), "Create Oracle backend. Using Oracle Client " << major << "." << minor << "." << update << "." << patch << "." << port);
}

/**
 *
 *
 **/
communicator::backend::oracle_t::~oracle_t() {
}

/**
 *
 *
 **/
void communicator::backend::oracle_t::parse_path() {

    boost::cmatch match;
    boost::regex re("([^/:]+)(:(\\d+))?/(\\w+)");

    if (!boost::regex_match(get_path().c_str(), match, re)) {
        throw std::invalid_argument("URI string must match 'SERVER[:PORT]/SERVICE_NAME' format");
    }

    server_ = match[1];
    port_ = match[3];
    service_name_ = match[4];
}

/**
 *
 *
 **/
void communicator::backend::oracle_t::begin_batch_impl(size_t num_messages) {

    LOG4CPLUS_TRACE(get_log(), "Opening Oracle connection");

    this->environment_->terminateConnection(this->connection_.get());
    connection_.reset(
            environment_->createConnection(get_login().c_str(), get_password().c_str(), get_servicename().c_str()),
            [this](oracle::occi::Connection * connection) {
                this->environment_->terminateConnection(connection);
            }
        );
}

/**
 *
 *
 **/
void communicator::backend::oracle_t::save_message_impl(const std::string & msg) {
}

/**
 *
 *
 **/
void communicator::backend::oracle_t::end_batch_impl() {
    LOG4CPLUS_TRACE(get_log(), "Closing Oracle connection");
}
