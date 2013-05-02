/**
 *
 *
 **/
#ifndef MONGODB_STORE_HPP
#define MONGODB_STORE_HPP

#include <string>
#include <mongo/client/dbclient.h>
#include "base.hpp"

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
        class mongodb_backend_t : public base_impl_t {

        public:
            mongodb_backend_t(const std::string & login, const std::string & password, const std::string & path);
            virtual ~mongodb_backend_t();

        private:
            virtual void save_message_impl(const std::string & msg);

        private:
            // std::string server_ip_;
            mongo::DBClientConnection connection_;
        };

    }
}

#endif // MONGODB_STORE_HPP
