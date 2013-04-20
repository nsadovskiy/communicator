/**
 *
 *
 **/
#ifndef OMNICOMM_PROTOCOL_HPP
#define OMNICOMM_PROTOCOL_HPP

#include <deque>
#include <vector>
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
#   pragma pack(push)
#   pragma pack(1)

    struct transport_header_t {
        unsigned char prefix;
        unsigned char cmd;
        unsigned short len;
        unsigned char data[1];
        unsigned short get_crc() const {
            return *reinterpret_cast<const unsigned short *>(&data[len]);
            // return (static_cast<short>(data[len]) << 8) | static_cast<short>(data[len + 1]);
        }
    };

#   pragma pack(pop)

    /**
     *
     *
     **/
    class protocol_t : public protocol_base_impl_t<protocol_t> {
    public:
        protocol_t();

    private:
        void store_data(const unsigned char * data, size_t len);

    private:
        virtual void init_impl();
        virtual void connect_impl();
        virtual void recive_impl(const unsigned char * data, size_t len);

    private:
        int state_;
        std::vector<unsigned char> buffer_;
    };
}

#endif // OMNICOMM_PROTOCOL_HPP
