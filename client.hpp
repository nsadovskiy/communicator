/**
 *
 *
 **/
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <vector>
#include <log4cplus/logger.h>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>

/**
 *
 *
 **/
class protocol_base_t;
class store_backend_t;

class client_t :
    public boost::enable_shared_from_this<client_t>,
    private boost::noncopyable {

public:
    typedef boost::shared_ptr<client_t> pointer_type;
    typedef boost::shared_ptr<protocol_base_t> impl_type;

public:
    virtual ~client_t();
    static pointer_type create(boost::asio::io_service & io_service, protocol_base_t * impl, store_backend_t & store_backend);

    boost::asio::ip::tcp::socket & get_socket() {
        return socket_;
    };

    void start();
    void stop();
    void send(const unsigned char * data, size_t len);

    store_backend_t & get_backend() {
        return store_backend_;
    }

private:
    client_t(boost::asio::io_service & io_service, protocol_base_t * impl, store_backend_t & store_backend);
    void handle_read(const boost::system::error_code & error, size_t len);
    void handle_write(const boost::system::error_code & ec, size_t bytes_transferred);

private:
    log4cplus::Logger log_;
    impl_type impl_;
    store_backend_t & store_backend_;
    boost::array<char, 8192> async_buffer_;
    std::vector<unsigned char> perm_buffer_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::io_service::strand strand_;
};

#endif // CLIENT_HPP
