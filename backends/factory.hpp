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

    struct storage_options_t;

    namespace backend {

        class base_impl_t;

        typedef boost::shared_ptr<base_impl_t> base_impl_pointer_type;

        /**
         *
         *
         **/
        namespace factory {

            base_impl_pointer_type create(const storage_options_t & options);
        }
    }
}

#endif // BACKEND_FACTORY_HPP
