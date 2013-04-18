/**
 *
 *
 **/
#ifndef BASE_PROTOCOL_HPP
#define BASE_PROTOCOL_HPP

/**
 *
 *
 **/
class protocol_base_t {
public:
    protocol_base_t();
    virtual ~protocol_base_t();

public:
    void init();
    void connect();
    void recive();

private:
    virtual void init_impl() = 0;
    virtual void connect_impl() = 0;
    virtual void recive_impl() = 0;
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
