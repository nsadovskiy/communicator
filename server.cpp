/**
 *
 *
 **/
#include "server.hpp"

#include <cassert>
#include <algorithm>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "client.hpp"
#include "backends/factory.hpp"
#include "backends/mongodb.hpp"
#include "backends/rabbitmq.hpp"

using std::string;
using std::distance;
using std::remove_if;
using boost::asio::ip::tcp;
using boost::system::error_code;
using boost::posix_time::seconds;

/**
 *
 *
 **/
communicator::server_t::server_t(const settings_t & settings, create_func_type create_func) :
    log_(log4cplus::Logger::getInstance("main")),
    interval_(5),
    num_workers_(settings.num_workers),
    create_client_func_(create_func),
    signals_(io_service_),
    acceptor_(io_service_),
    timer_(io_service_, seconds(interval_)),
    store_backend_(backend::factory::create(settings.store.protocol, settings.store.username, settings.store.password, settings.store.path)) {

    LOG4CPLUS_TRACE(log_, "Initializing server");

    LOG4CPLUS_TRACE(log_, "Starting starage backend");
    store_backend_->start();
    bind_signals();
    start_listen(settings.listen.ip_addr, settings.listen.port);
}

/**
 *
 *
 **/
void communicator::server_t::bind_signals() {

    LOG4CPLUS_TRACE(log_, "Binding interruption signal handler");

    signals_.add(SIGINT);
    signals_.add(SIGTERM);
#   if defined(SIGQUIT)
        signals_.add(SIGQUIT);
#   endif

    LOG4CPLUS_TRACE(log_, "Start waiting for interruption signal");
    signals_.async_wait(
            [this](const error_code &, const int &) {
                this->handle_stop();
            }
        );
}

/**
 *
 *
 **/
void communicator::server_t::start_listen(const string & ip_addr, const string & port) {

    LOG4CPLUS_TRACE(log_, "Initializing listening socket");

    tcp::resolver resolver(io_service_);
    tcp::resolver::query query(ip_addr, port);
    tcp::endpoint endpoint = *resolver.resolve(query);

    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);

    LOG4CPLUS_TRACE(log_, "Start listening");

    acceptor_.listen();

    start_accept();
}

/**
 *
 *
 **/
// communicator::server_t::server_t(const char * bind_addr, const char * port, size_t num_workers, create_func_type create_func) :
//     log_(log4cplus::Logger::getInstance("main")),
//     interval_(5),
//     num_workers_(num_workers),
//     create_client_func_(create_func),
//     signals_(io_service_),
//     acceptor_(io_service_),
//     timer_(io_service_, seconds(interval_)),
//     store_backend_(new communicator::backend::mongodb_backend_t("", "", "10.10.3.25:27017")) {

//     assert(create_client_func_);

//     store_backend_->start();

//     signals_.add(SIGINT);
//     signals_.add(SIGTERM);
// #if defined(SIGQUIT)
//     signals_.add(SIGQUIT);
// #endif
//     signals_.async_wait(
//             [this](const error_code &, const int &) {
//                 this->handle_stop();
//             }
//         );

//     timer_.async_wait(
//             [this](const error_code & error) {
//                 this->handle_timer(error);
//             }
//         );

//     tcp::resolver resolver(io_service_);
//     tcp::resolver::query query(bind_addr, port);
//     tcp::endpoint endpoint = *resolver.resolve(query);
//     acceptor_.open(endpoint.protocol());
//     acceptor_.set_option(tcp::acceptor::reuse_address(true));
//     acceptor_.bind(endpoint);
//     acceptor_.listen();

//     start_accept();
// }

/**
 *
 *
 **/
communicator::server_t::~server_t() {
    store_backend_->stop();
}

/**
 *
 *
 **/
void communicator::server_t::run() {
    io_service_.run();
}

/**
 *
 *
 **/
void communicator::server_t::start_accept() {

    LOG4CPLUS_TRACE(log_, "Starting async accept operation");

    assert(create_client_func_);

    client_pointer_type client(client_t::create(io_service_, create_client_func_(), *store_backend_));
    acceptor_.async_accept(
        client->get_socket(),
        [this, client](const error_code & error) {
            this->handle_accept(error, client);
        }
    );
}

/**
 *
 *
 **/
void communicator::server_t::handle_stop() {
    io_service_.stop();
    LOG4CPLUS_INFO(log_, "Bye!");
}

/**
 *
 *
 **/
void communicator::server_t::handle_timer(const error_code & error) {

    if (!error) {
        timer_.expires_at(timer_.expires_at() + seconds(interval_));
        timer_.async_wait(
            [this](const error_code & error) {
                this->handle_timer(error);
            }
        );

        drop_unused_clients();
    }
}

/**
 *
 *
 **/
void communicator::server_t::handle_accept(const error_code & error, client_pointer_type client) {

    LOG4CPLUS_DEBUG(log_, "Connection accepted");

    if (!acceptor_.is_open()) {
        return;
    }

    if (!error) {
        client->start();
        {
            lock_guard_type lock(mutex_);
            clients_.push_back(client);
        }
    }

    start_accept();
}

/**
 *
 *
 **/
void communicator::server_t::drop_unused_clients() {

    lock_guard_type lock(mutex_);
    auto pos = remove_if(
            clients_.begin(),
            clients_.end(),
            [](const client_pointer_type & client) {
                return !client->get_socket().is_open();
            }
        );
    if (pos != clients_.end()) {
        LOG4CPLUS_INFO(log_, "Where are " << distance(pos, clients_.end()) << " disconnected client(s). Kill them all!!!");
        clients_.erase(pos, clients_.end());
    }
}
