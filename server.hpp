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

private:
    size_t num_workers_;
    boost::asio::io_service io_service_;
    boost::asio::signal_set signals_;
    boost::asio::ip::tcp::acceptor acceptor_;
};

#endif
