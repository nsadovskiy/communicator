/**
 *
 *
 **/
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>

class base_protocol_t;

/**
 *
 *
 **/
class client_t : public boost::enable_shared_from_this<client_t> {

public:
    typedef boost::shared_ptr<client_t> pointer_type;

public:
    virtual ~client_t();
    static pointer_type create(boost::asio::io_service & io_service, base_protocol_t * impl);

    boost::asio::ip::tcp::socket & get_socket() {
        return socket_;
    };

    void start();

private:
    client_t(boost::asio::io_service & io_service, base_protocol_t * impl);

private:
    base_protocol_t * impl_;
    boost::asio::ip::tcp::socket socket_;
};

#endif // CLIENT_HPP
