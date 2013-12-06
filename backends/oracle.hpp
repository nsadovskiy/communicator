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
            oracle_t(const storage_options_t & options, bool test_connection = true);
            virtual ~oracle_t();

        public:
            const std::string & get_servicename() const {
                return get_db_name();
            }

        private:
            void open_connection();

        private:
            virtual void begin_batch_impl(size_t num_messages);
            virtual void save_message_impl(const std::string & msg);
            virtual void end_batch_impl();

        private:
            std::string server_;
            std::string port_;
            std::string service_name_;
            std::string connection_string_;
            boost::shared_ptr<oracle::occi::Environment> environment_;
            boost::shared_ptr<oracle::occi::Connection> connection_;
        };
    };
};

#endif // ORACLE_BACKEND_HPP
