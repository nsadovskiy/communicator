/**
 *
 *
 **/
#include <sstream>
#include "mongodb_backend.hpp"

using namespace mongo;
using std::string;
using std::ostringstream;

/**
 *
 *
 **/
mongodb_backend_t::mongodb_backend_t(std::string server_ip) :
    server_ip_(server_ip),
    connection_(true, nullptr, 0) {

    string err;

    if (connection_.connect(server_ip_.c_str(), err)) {
        LOG4CPLUS_INFO(get_log(), "Connected MongoDB at " << server_ip_ << " complete successful");
    } else {
        LOG4CPLUS_ERROR(get_log(), "Connection MongoDB at " << server_ip_ << " failed. " << err);
    }
    
}

/**
 *
 *
 **/
mongodb_backend_t::~mongodb_backend_t() {

}

/**
 *
 *
 **/
void mongodb_backend_t::save_messages_impl(const std::string & msg) {

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
