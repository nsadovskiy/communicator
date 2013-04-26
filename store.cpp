/**
 *
 *
 **/
#include "store.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>

/**
 *
 *
 **/
store_backend_t::store_backend_t() :
    log_(log4cplus::Logger::getInstance("main")) {
}

/**
 *
 *
 **/
store_backend_t::~store_backend_t() {
    stop();
}

/**
 *
 *
 **/
size_t store_backend_t::add_message(const std::string & msg) {

    LOG4CPLUS_TRACE(log_, "storing message [" << msg << "]");

    lock_guard_type lock(mutex_);

    messages_.push_back(msg);
    add_message_impl(msg);

    return messages_.size();
}

/**
 *
 *
 **/
void store_backend_t::start() {

    if (!work_thread_) {
        work_thread_.reset(
                new boost::thread( [this]() { this->work_proc(); })
            );
    }
}

/**
 *
 *
 **/
void store_backend_t::stop() {
    if (work_thread_) {
        work_thread_->interrupt();
        work_thread_->join();
        work_thread_.reset();
    }
}

/**
 *
 *
 **/
void store_backend_t::work_proc() {

    LOG4CPLUS_INFO(log_, "storage work thread started");

    array_type messages;

    try {

        while (true) {

            {
                lock_guard_type lock(mutex_);
                messages.assign(messages_.begin(), messages_.end());
                messages_.clear();
            }

            if (!messages_.empty()) {
                
                LOG4CPLUS_INFO(log_, "storing found " << messages_.size() << " undelivered message(s)");
                
                for (auto msg : messages) {
                    save_messages_impl(msg);
                    boost::this_thread::interruption_point();
                }
            
            } else { //if (!messages_.empty())
                LOG4CPLUS_TRACE(log_, "no undelivered messages found");
            } // if (!messages_.empty())

            boost::this_thread::sleep(boost::posix_time::seconds(5));
        }

    } catch(boost::thread_interrupted &) {
        LOG4CPLUS_INFO(log_, "storage work thread interrupted");

    } catch (std::exception & e) {
        LOG4CPLUS_ERROR(log_, e.what());
    }
}

/**
 *
 *
 **/
void store_backend_t::add_message_impl(const std::string & msg) {
}
