/**
 *
 *
 **/
#ifndef SERVER_HPP
#define SERVER_HPP

#include <cstddef>
#include <boost/asio.hpp>

class server_t {

public:
    server_t(const char * bind_addr, const char * port, size_t num_workers);

public:
    void run();

private:
    void handle_stop();
    void handle_accept(const boost::system::error_code & error/*, boost::asio::ip::tcp::socket & socket*/);
    void start_accept();

private:
    size_t num_workers_;
    boost::asio::io_service io_service_;
    boost::asio::signal_set signals_;
    boost::asio::ip::tcp::acceptor acceptor_;
};

#endif
