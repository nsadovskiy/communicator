/**
 *
 *
 **/
#ifndef MONGODB_STORE_HPP
#define MONGODB_STORE_HPP

#include <string>
#include <mongo/client/dbclient.h>
#include "store.hpp"

/**
 *
 *
 **/
class mongodb_backend_t : public store_backend_t {

public:
    mongodb_backend_t(std::string server_ip);
    virtual ~mongodb_backend_t();

private:
    virtual void save_messages_impl(const std::string & msg);

private:
    std::string server_ip_;
    mongo::DBClientConnection connection_;
};

#endif // MONGODB_STORE_HPP
