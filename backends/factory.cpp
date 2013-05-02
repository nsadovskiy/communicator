/**
 *
 *
 **/
#include "factory.hpp"
#include <map>
#include <stdexcept>
#include "../config.h"

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
    base_impl_pointer_type creator(const std::string & login, const std::string & password, const std::string & path) {
        return base_impl_pointer_type(new T(login, password, path));
    };

    template<>
    base_impl_pointer_type creator<int>(const std::string &, const std::string &, const std::string &) {
        return base_impl_pointer_type();
    }

    typedef base_impl_pointer_type (*creator_func_type)(const std::string &, const std::string &, const std::string &);
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
            base_impl_pointer_type create(const std::string & protocol, const std::string & login, const std::string & password, const std::string & path) {

                map<string, creator_func_type> backends {
    #               ifdef MongoDB_FOUND
                        pair<string, creator_func_type>("mongo", creator<mongodb_backend_t>),
    #               endif
    #               ifdef RABBITMQ_FOUND
                        pair<string, creator_func_type>("rabbit", creator<communicator::backend::rabbitmq_t>),
    #               endif
    #               ifdef ORACLE_FOUND
                        pair<string, creator_func_type>("oracle", creator<communicator::backend::oracle_t>),
    #               endif
                        pair<string, creator_func_type>("empty", creator<int>)
                };

                if (backends.count(protocol) < 1) {
                    throw std::invalid_argument("Specified storage backend not supported");
                }

                return backends[protocol](login, password, path);
            }
        }
    }
}
