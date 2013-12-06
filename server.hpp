/**
 *
 *
 **/
#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <string>
#include <cstddef>

#include <log4cplus/logger.h>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

/**
 *
 *
 **/
class client_t;
class protocol_base_t;

/**
 *
 *
 **/
namespace communicator {

    namespace backend {
        class base_impl_t;
    }

    /**
     *
     *
     **/
    struct settings_t {

        unsigned num_workers = 1;

        struct cp {
            std::string ip_addr = "0.0.0.0";
            std::string port = "4010";
        } listen;

        struct {
            std::string protocol;
            std::string username;
            std::string password;
            std::string path;
        } store;
    };

    /**
     *
     *
     **/
    class server_t {

    public:
        typedef protocol_base_t * (*create_func_type)();

    private:
        typedef boost::shared_ptr<client_t> client_pointer_type;
        typedef boost::lock_guard<boost::mutex> lock_guard_type;

    public:
        server_t(const settings_t & settings, create_func_type create_func);
        // server_t(const char * bind_addr, const char * port, size_t num_workers, create_func_type create_func);
        virtual ~server_t();

    public:
        void run();

    private:
        void bind_signals();
        void start_listen(const std::string & ip_addr, const std::string & port);

        void handle_stop();
        void handle_timer(const boost::system::error_code & error);
        void handle_accept(const boost::system::error_code & error, client_pointer_type client);
        void start_accept();
        void drop_unused_clients();

    private:
        log4cplus::Logger log_;
        size_t interval_;
        size_t num_workers_;
        create_func_type create_client_func_;
        boost::mutex mutex_;
        boost::asio::io_service io_service_;
        boost::asio::signal_set signals_;
        boost::asio::ip::tcp::acceptor acceptor_;
        boost::asio::deadline_timer timer_;
        std::vector<client_pointer_type> clients_;
        boost::shared_ptr<backend::base_impl_t> store_backend_;
    };
}

#endif // SERVER_HPP
