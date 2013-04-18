/**
 *
 *
 **/
#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <cstddef>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

/**
 *
 *
 **/
class client_t;
class protocol_base_t;

class server_t {

public:
    typedef protocol_base_t * (*create_func_type)();

private:
    typedef boost::shared_ptr<client_t> client_type;
    typedef std::vector<client_type> client_array_type;
    typedef boost::lock_guard<boost::mutex> lock_guard_type;

public:
    server_t(const char * bind_addr, const char * port, size_t num_workers, create_func_type create_func);

public:
    void run();

private:
    void handle_stop();
    void handle_timer(const boost::system::error_code & error);
    void handle_accept(const boost::system::error_code & error, client_type client);
    void start_accept();
    void drop_unused_clients();

private:
    size_t interval_;
    size_t num_workers_;
    create_func_type create_client_func_;
    boost::mutex mutex_;
    boost::asio::io_service io_service_;
    boost::asio::signal_set signals_;
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::deadline_timer timer_;
    client_array_type clients_;
};

#endif // SERVER_HPP
