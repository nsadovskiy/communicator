/**
 *
 *
 **/
#include "oracle.hpp"

#include <sstream>
#include <stdexcept>
#include <boost/regex.hpp>
#include <boost/thread.hpp>

using boost::shared_ptr;
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
    LOG4CPLUS_INFO(get_log(), "Created Oracle backend. Using Oracle Client " << major << "." << minor << "." << update << "." << patch << "." << port);

    open_connection();
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
        throw std::invalid_argument("URI string must match '[LOGIN:PASSWORD@]SERVER[:PORT]/SERVICE_NAME' format");
    }

    server_ = match[1];

    port_ = match[3];
    if (port_.empty()) {
        port_ = "1521";
    }

    service_name_ = match[4];

    std::ostringstream strm;
    strm << "(DESCRIPTION=(ADDRESS=(PROTOCOL=TCP)(HOST=" << server_ << ")(PORT=" << port_ << "))(CONNECT_DATA=(SID=" << service_name_ << ")))";
    connection_string_ = strm.str();
}

/**
 *
 *
 **/
void communicator::backend::oracle_t::open_connection() {

    try {

        if (!connection_) {

            LOG4CPLUS_TRACE(get_log(), "Connecting Oracle at " << connection_string_ << " using login '" << get_login() << "' and password '******'");
            
            connection_.reset(
                    environment_->createConnection(get_login().c_str(), get_password().c_str(), connection_string_.c_str()),
                    [this](Connection * connection) {
                        this->environment_->terminateConnection(connection);
                    }
                );
        }

        LOG4CPLUS_TRACE(get_log(), "Checking connection state");

        shared_ptr<Statement> stmt(
                connection_->createStatement("select 1 from dual"),
                [this](Statement * stmt) {
                    this->connection_->terminateStatement(stmt);
                }
            );

        shared_ptr<ResultSet> rst(
                stmt->executeQuery(),
                [stmt](ResultSet * rst) {
                    stmt->closeResultSet(rst);
                }
            );

        if (!rst->next() || rst->getInt(1) != 1) {
            throw std::runtime_error("Oracle connection check failed");
        }

        LOG4CPLUS_TRACE(get_log(), "Connection ready");

    } catch (const SQLException & e) {
        connection_.reset();
        throw std::runtime_error(e.getMessage());

    } catch (const std::exception & e) {
        connection_.reset();
        throw std::runtime_error(e.what());

    } catch (...) {
        connection_.reset();
        throw std::runtime_error("Unknown error while connecting Oracle");
    }
}

/**
 *
 *
 **/
void communicator::backend::oracle_t::begin_batch_impl(size_t num_messages) {
    open_connection();
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
    connection_->commit();
}
