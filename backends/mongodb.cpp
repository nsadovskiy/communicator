/**
 *
 *
 **/
#include <sstream>
#include "mongodb.hpp"

using namespace mongo;
using std::string;
using std::ostringstream;

/**
 *
 *
 **/
communicator::backend::mongodb_backend_t::mongodb_backend_t(const std::string & login, const std::string & password, const std::string & path) :
    base_impl_t(login, password, path),
    // server_ip_(server_ip),
    connection_(true, nullptr, 0) {

    string err;

    if (connection_.connect(get_path().c_str(), err)) {
    //     LOG4CPLUS_INFO(get_log(), "Connected MongoDB at " << server_ip_ << " complete successful");
    } else {
    //     LOG4CPLUS_ERROR(get_log(), "Connection MongoDB at " << server_ip_ << " failed. " << err);
    }
}

/**
 *
 *
 **/
communicator::backend::mongodb_backend_t::~mongodb_backend_t() {

}

/**
 *
 *
 **/
void communicator::backend::mongodb_backend_t::save_message_impl(const std::string & msg) {

    try {

        ostringstream message;
        BSONObjBuilder builder;

        message << "<Message " << msg << "/>";
        builder.append("message", message.str());
        connection_.insert("test.omnicomm", builder.obj());
        LOG4CPLUS_TRACE(get_log(), "Stored message " << message.str());

    } catch (const mongo::DBException & e) {
        LOG4CPLUS_ERROR(get_log(), e.what());
        throw;

    } catch (const std::exception & e) {
        LOG4CPLUS_ERROR(get_log(), e.what());
        throw;

    } catch (...) {
        LOG4CPLUS_ERROR(get_log(), "Unexpected exception");
        throw;
    }
}
