/**
 *
 *
 **/
#include "factory.hpp"
#include <map>
#include <string>
#include <stdexcept>
#include "../config.h"
#include "../settings.hpp"

#ifdef MongoDB_FOUND
#   include "mongodb.hpp"
#endif

#ifdef RABBITMQ_FOUND
#   include "rabbitmq.hpp"
#endif

#ifdef ORACLE_FOUND
#   include "oracle.hpp"
#endif


using std::map;
using std::pair;
using std::string;
using boost::shared_ptr;
using communicator::storage_options_t;
using communicator::backend::base_impl_pointer_type;

/**
 *
 *
 **/
namespace {

    /**
     *
     *
     **/
    template<class T>
    base_impl_pointer_type creator(const storage_options_t & options) {
        return base_impl_pointer_type(new T(options));
    };

    typedef base_impl_pointer_type (*creator_func_type)(const storage_options_t &);
};

/**
 *
 *
 **/
namespace communicator {
    namespace backend {
        namespace factory {

            /**
             *
             *
             **/
            base_impl_pointer_type create(const storage_options_t & options) {

                map<const string, creator_func_type> backends;
#               ifdef MongoDB_FOUND
					backends[string("mongo")] = creator<mongodb_backend_t>;
#               endif
#               ifdef RABBITMQ_FOUND
					backends[string("rabbit")] = creator<communicator::backend::rabbitmq_t>;
#               endif
#               ifdef ORACLE_FOUND
					backends[string("oracle")] = creator<communicator::backend::oracle_t>;
#               endif

				if (backends.count(options.kind) < 1) {
                     throw std::invalid_argument("Specified storage backend not supported");
                }

                return backends[options.kind](options);
            }
        }
    }
}
