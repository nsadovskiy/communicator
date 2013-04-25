/**
 *
 *
 **/
#ifndef BASE_PROTOCOL_HPP
#define BASE_PROTOCOL_HPP

#include <cassert>
#include <cstddef>
#include <log4cplus/logger.h>


class client_t;

/**
 *
 *
 **/
class protocol_base_t {
public:
    protocol_base_t();
    virtual ~protocol_base_t();

public:
    void init(client_t * manipulator);
    void connect();
    void recive(const unsigned char * data, size_t len);
    client_t * get_manipulator() {
        assert(manipulator_);
        return manipulator_;
    };

private:
    virtual void init_impl() = 0;
    virtual void connect_impl() = 0;
    virtual void recive_impl(const unsigned char * data, size_t len) = 0;

private:
    log4cplus::Logger log_;
    client_t * manipulator_;
};

/**
 *
 *
 **/
template<class T>
class protocol_base_impl_t : public protocol_base_t {
public:
    static protocol_base_t * create() {
        return new T();
    }
};

#endif // BASE_PROTOCOL_HPP
