/**
 *
 *
 **/
#ifndef MONGODB_STORE_HPP
#define MONGODB_STORE_HPP

#include <string>
#include "store.hpp"

/**
 *
 *
 **/
class mongodb_backend_t : public store_backend_t {

public:
    mongodb_backend_t(std::string server_ip);

private:
    virtual void save_messages_impl(const std::string & msg);
};

#endif // MONGODB_STORE_HPP
