/**
 *
 *
 **/
#ifndef OMNICOMM_PROTOCOL_HPP
#define OMNICOMM_PROTOCOL_HPP

#include <deque>
#include <vector>
#include <log4cplus/logger.h>
#include "transp.hpp"
#include "../protocol_base.hpp"


/**
 *
 *
 **/
class client_t;

/**
 *
 *
 **/
namespace omnicomm {

    /**
     *
     *
     **/
    class protocol_t : public protocol_base_impl_t<protocol_t> {
    private:
        typedef std::vector<unsigned char> array_type;

    public:
        protocol_t();

    private:
        void store_data(const unsigned char * data, size_t len);

        void send(const unsigned char * data, size_t len);

    private:
        virtual void init_impl();
        virtual void connect_impl();
        virtual void recive_impl(const unsigned char * data, size_t len);

    private:
        log4cplus::Logger log_;
        int state_;
        array_type buffer_;
        transp_protocol_t transport_protocol_;
    };
}

#endif // OMNICOMM_PROTOCOL_HPP
