/**
 *
 *
 **/
#ifndef OMNICOMM_PROTOCOL_HPP
#define OMNICOMM_PROTOCOL_HPP

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
    class protocol_t : public protocol_base_impl_t<protocol_t> {
    private:
        virtual void init_impl();
        virtual void connect_impl();
        virtual void recive_impl();
    };
}

#endif // OMNICOMM_PROTOCOL_HPP
