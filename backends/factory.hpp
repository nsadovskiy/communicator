/**
 *
 *
 **/
#ifndef BACKEND_FACTORY_HPP
#define BACKEND_FACTORY_HPP

#include <boost/shared_ptr.hpp>

/**
 *
 *
 **/
class store_backend_t;

/**
 *
 *
 **/
namespace communicator {
    namespace backend {

        /**
         *
         *
         **/
        namespace factory {
            boost::shared_ptr<store_backend_t> create(const std::string & protocol, const std::string & login, const std::string & password, const std::string & path);
        }
    }
}

#endif // BACKEND_FACTORY_HPP
