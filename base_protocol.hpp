/**
 *
 *
 **/
#ifdef BASE_PROTOCOL_HPP
#define BASE_PROTOCOL_HPP

struct base_protocol_t {

    virtual void init() = 0;
    virtual void connect() = 0;
    virtual void recive() = 0;
}

#endif // BASE_PROTOCOL_HPP
