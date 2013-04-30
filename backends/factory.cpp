/**
 *
 *
 **/
#include "factory.hpp"
#include <map>
#include "../config.h"

#ifdef MongoDB_FOUND
#   include "mongodb.hpp"
#endif

#ifdef RABBITMQ_FOUND
#   include "rabbitmq.hpp"
#endif

/**
 *
 *
 **/
using std::map;
using std::pair;
using std::string;
using boost::shared_ptr;

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
    shared_ptr<store_backend_t> creator(const std::string & login, const std::string & password, const std::string & path) {
        return shared_ptr<store_backend_t>(new T(login, password, path));
    };

    template<>
    shared_ptr<store_backend_t> creator<int>(const std::string &, const std::string &, const std::string &) {
        return shared_ptr<store_backend_t>();
    }

    typedef shared_ptr<store_backend_t> (*creator_func_type)(const std::string &, const std::string &, const std::string &);
};

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
        shared_ptr<store_backend_t> create(const std::string & protocol, const std::string & login, const std::string & password, const std::string & path) {

            map<string, creator_func_type> backends {
#               ifdef MongoDB_FOUND
                    pair<string, creator_func_type>("mongo", creator<mongodb_backend_t>),
#               endif
#               ifdef RABBITMQ_FOUND
                    pair<string, creator_func_type>("rabbit", creator<communicator::backend::rabbitmq_t>),
#               endif
                    pair<string, creator_func_type>("empty", creator<int>)
            };

            return shared_ptr<store_backend_t>();
        }
    }
}
