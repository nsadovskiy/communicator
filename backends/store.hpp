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
class store_backend_t {

    typedef std::vector<std::string> array_type;
    typedef boost::lock_guard<boost::mutex> lock_guard_type;

public:
    store_backend_t(const std::string & login, const std::string & password, const std::string & path);
    virtual ~store_backend_t();

public:
    log4cplus::Logger & get_log() {
        return log_;
    }

    size_t add_message(const std::string & msg);
    size_t size() {
        lock_guard_type lock(mutex_);
        return messages_.size();
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

    const std::string & get_login() const {
        return login_;
    }

    const std::string & get_password() const {
        return password_;
    }

    const std::string & get_path() const {
        return path_;
    }

private:
    void work_proc();

private:
    virtual void add_message_impl(const std::string & msg);
    virtual void save_messages_impl(const std::string & msg) = 0;

private:
    std::string login_;
    std::string password_;
    std::string path_;
    size_t sleep_interval_;
    log4cplus::Logger log_;
    boost::mutex mutex_;
    array_type messages_;
    boost::shared_ptr<boost::thread> work_thread_;
};

#endif // STORE_HPP
