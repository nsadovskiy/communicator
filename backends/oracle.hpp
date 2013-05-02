/**
 *
 *
 **/
#ifndef ORACLE_BACKEND_HPP
#define ORACLE_BACKEND_HPP

#include <occi.h>
#include <boost/shared_ptr.hpp>
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
        class oracle_t : public base_impl_t {
        public:
            oracle_t(const std::string & login, const std::string & password, const std::string & path);
            virtual ~oracle_t();

        public:

            const std::string & get_server() const {
                return server_;
            }

            const std::string & get_port() const {
                return port_;
            }

            const std::string & get_servicename() const {
                return service_name_;
            }

        private:
            void parse_path();

        private:
            virtual void begin_batch_impl(size_t num_messages);
            virtual void save_message_impl(const std::string & msg);
            virtual void end_batch_impl();

        private:
            std::string server_;
            std::string port_;
            std::string service_name_;
            boost::shared_ptr<oracle::occi::Environment> environment_;
            boost::shared_ptr<oracle::occi::Connection> connection_;
        };
    };
};

#endif // ORACLE_BACKEND_HPP
