/**
 *
 *
 **/
#ifndef STORE_HPP
#define STORE_HPP

#include <string>
#include <vector>
#include <log4cplus/logger.h>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

/**
 *
 *
 **/
namespace communicator {

    struct storage_options_t;

    namespace backend {

        /**
         *
         *
         **/
        class base_impl_t {

            typedef std::vector<std::string> array_type;
            typedef boost::lock_guard<boost::mutex> lock_guard_type;

        public:
            base_impl_t(const storage_options_t & options);
            virtual ~base_impl_t();

        public:
            log4cplus::Logger & get_log() {
                return log_;
            }

            void begin_batch(size_t num_messages);
            size_t add_message(const std::string & msg);
            void end_batch();

            size_t size() {
                lock_guard_type lock(mutex_);
                return messages_.size();
            }

            size_t get_capacity() const {
                return max_messages_;
            }

            void set_capacity(size_t capacity) {
                max_messages_ = capacity;
            }

            void start();
            void stop();

            size_t get_sleep_interval() {
                lock_guard_type lock(mutex_);
                return sleep_interval_;
            }

            void set_sleep_interval(size_t interval) {
                lock_guard_type lock(mutex_);
                sleep_interval_ = interval;
            }

            const std::string & get_ip_addr() const {
                return ip_addr_;
            }

            unsigned get_tcp_port() const {
                return tcp_port_;
            }

            const std::string & get_login() const {
                return login_;
            }

            const std::string & get_password() const {
                return password_;
            }

            const std::string & get_db_name() const {
                return db_name_;
            }

        private:
            void work_proc();

        private:
            virtual void begin_batch_impl(size_t num_messages);
            virtual void save_message_impl(const std::string & msg) = 0;
            virtual void end_batch_impl();

        private:
            size_t max_messages_;
            std::string ip_addr_;
            unsigned tcp_port_;
            std::string login_;
            std::string password_;
            std::string db_name_;
            size_t sleep_interval_;
            log4cplus::Logger log_;
            boost::mutex mutex_;
            array_type messages_;
            boost::shared_ptr<boost::thread> work_thread_;
        };
    }
}

#endif // STORE_HPP
