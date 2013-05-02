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
namespace communicator {
    namespace backend {

        class base_impl_t;

        typedef boost::shared_ptr<base_impl_t> base_impl_pointer_type;

        /**
         *
         *
         **/
        namespace factory {

            base_impl_pointer_type create(const std::string & protocol, const std::string & login, const std::string & password, const std::string & path);
        }
    }
}

#endif // BACKEND_FACTORY_HPP
