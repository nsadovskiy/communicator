/**
 *
 *
 **/
#include "base.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>

/**
 *
 *
 **/
communicator::backend::base_impl_t::base_impl_t(const std::string & login, const std::string & password, const std::string & path) :
    login_(login),
    password_(password),
    path_(path),
    log_(log4cplus::Logger::getInstance("main")) {
}

/**
 *
 *
 **/
communicator::backend::base_impl_t::~base_impl_t() {
    stop();
}

/**
 *
 *
 **/
void communicator::backend::base_impl_t::begin_batch(size_t num_messages) {
    begin_batch_impl(num_messages);
}

/**
 *
 *
 **/
size_t communicator::backend::base_impl_t::add_message(const std::string & msg) {

    LOG4CPLUS_TRACE(log_, "Enqueuing message [" << msg << "]");

    lock_guard_type lock(mutex_);

    messages_.push_back(msg);

    return messages_.size();
}

/**
 *
 *
 **/
void communicator::backend::base_impl_t::end_batch() {
    end_batch_impl();
}

/**
 *
 *
 **/
void communicator::backend::base_impl_t::start() {

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
void communicator::backend::base_impl_t::stop() {
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
void communicator::backend::base_impl_t::work_proc() {

    LOG4CPLUS_INFO(log_, "Storage work thread started");

    array_type messages;

    try {

        while (true) {

            boost::this_thread::sleep(boost::posix_time::seconds(5));

            try {

                begin_batch(messages.size());

                {
                    lock_guard_type lock(mutex_);
                    messages.assign(messages_.begin(), messages_.end());
                    messages_.clear();
                }

                if (!messages.empty()) {
                    
                    LOG4CPLUS_DEBUG(log_, "Found " << messages.size() << " undelivered message(s)");

                        for (auto msg : messages) {
                            save_message_impl(msg);
                            boost::this_thread::interruption_point();
                        }
                
                } else { //if (!messages_.empty())
                    LOG4CPLUS_TRACE(log_, "No undelivered messages found");
                } // if (!messages_.empty())

                end_batch();

            } catch (const std::exception & e) {
                LOG4CPLUS_ERROR(log_, e.what());

            } catch (...) {
                LOG4CPLUS_ERROR(log_, "Unknown backend error");
            }
        }

    } catch(boost::thread_interrupted &) {
        LOG4CPLUS_INFO(log_, "Storage work thread interrupted");

    } catch (std::exception & e) {
        LOG4CPLUS_ERROR(log_, e.what());
    }
}

/**
 *
 *
 **/
void communicator::backend::base_impl_t::begin_batch_impl(size_t num_messages) {
}

/**
 *
 *
 **/
void communicator::backend::base_impl_t::end_batch_impl() {
}
