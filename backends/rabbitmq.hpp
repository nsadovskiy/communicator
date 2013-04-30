/**
 *
 *
 **/
#ifndef RABBITMQ_STORE_HPP
#define RABBITMQ_STORE_HPP

#include "store.hpp"

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
        class rabbitmq_t : public store_backend_t {
        public:
            rabbitmq_t(const std::string & login, const std::string & password, const std::string & path);
            virtual ~rabbitmq_t();

        private:
            virtual void save_messages_impl(const std::string & msg);
        };
    };
};

#endif // RABBITMQ_STORE_HPP
