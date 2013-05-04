/**
 *
 *
 **/
#ifndef OMNICOMM_PROTOCOL_HPP
#define OMNICOMM_PROTOCOL_HPP

#include <deque>
#include <vector>
#include <log4cplus/logger.h>
#include "info_package.hpp"
#include "../protocol_base.hpp"


/**
 *
 *
 **/
namespace omnicomm {

    /**
     *
     *
     **/
    class transport_protocol_t : public protocol_base_impl_t<transport_protocol_t> {
    private:
        typedef std::vector<unsigned char> array_type;

    public:
        transport_protocol_t();

    private:
        void store_data(const unsigned char * data, size_t len);

        void process_message(const transport_header_t & hdr);
        void process_controller_ident(const transport_header_t & hdr);
        void process_archive_data(const transport_header_t & hdr);
        void process_current_data(const transport_header_t & hdr);
        void process_info_messages(const data_response_t & hdr);

        void send(const unsigned char * data, size_t len);

    private:
        virtual void init_impl();
        virtual void connect_impl();
        virtual void recive_impl(const unsigned char * data, size_t len);

    private:
        log4cplus::Logger log_;
        int state_;
        unsigned controller_id_;
        unsigned firmware_version_;
        array_type buffer_;
        info_protocol_t info_protocol_;
    };
}

#endif // OMNICOMM_PROTOCOL_HPP
