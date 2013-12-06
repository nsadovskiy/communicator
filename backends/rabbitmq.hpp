/**
 *
 *
 **/
#ifndef RABBITMQ_STORE_HPP
#define RABBITMQ_STORE_HPP

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
        class rabbitmq_t : public base_impl_t {
        public:
            rabbitmq_t(const storage_options_t & options);
            virtual ~rabbitmq_t();

        private:
            virtual void save_message_impl(const std::string & msg);
        };
    };
};

#endif // RABBITMQ_STORE_HPP
