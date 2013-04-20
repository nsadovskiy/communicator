/**
 *
 *
 **/
#include "protocol.hpp"
#include <iostream>
#include "../utils.hpp"
 #include "utils.hpp"

using std::wcout;

/**
 *
 *
 **/
namespace {

    namespace stuff_state {
        enum {
            normal,
            possible_stuff
        };
    }
}

/**
 *
 *
 **/
omnicomm::protocol_t::protocol_t() :
    state_(stuff_state::normal) {

}

/**
 *
 *
 **/
void omnicomm::protocol_t::init_impl() {
}

/**
 *
 *
 **/
void omnicomm::protocol_t::connect_impl() {
}

/**
 *
 *
 **/
void omnicomm::protocol_t::recive_impl(const unsigned char * data, size_t len) {

    wcout << L"[" << this << L"][omnicomm::protocol_t::recive_impl] Data: [" << to_hex_string(data, len).c_str() << L"]\n";
    store_data(data, len);
    wcout << L"[" << this << L"][omnicomm::protocol_t::recive_impl] Destuffed data: [" << to_hex_string(&buffer_[0], buffer_.size()).c_str() << L"]\n";

    const transport_header_t * hdr = reinterpret_cast<const transport_header_t *>(&data[0]);
    wcout << std::hex;
    wcout << L"   prefix: " << hdr->prefix << L"\n";
    wcout << L"   command: " << hdr->cmd << L"\n";
    wcout << L"   length: " << hdr->len << L"\n";
    wcout << L"   crc: " << hdr->get_crc() << L"\n";
    wcout << L"   calcilated crc: " << omnicomm::crc16(reinterpret_cast<const unsigned char *>(&hdr->cmd), hdr->len + 3) << L"\n";
    wcout << std::dec;
}

/**
 *
 *
 **/
void omnicomm::protocol_t::store_data(const unsigned char * data, size_t len) {

    using stuff_state::normal;
    using stuff_state::possible_stuff;

    for (size_t i = 0; i < len; ++i) {

        switch (state_) {
            
            case normal:
                if (data[i] == 0xdb) {
                    state_ = possible_stuff;
                } else {
                    buffer_.push_back(data[i]);
                }
                break;

            case possible_stuff:
                if (data[i] == 0xdc) {
                    buffer_.push_back(0x0c);
                } else if (data[i] == 0xdd) {
                    buffer_.push_back(0xdb);
                } else {
                    // Если символы 0x0c и 0xdb экранированы, по-идее, мы сюда попасть не должны...
                    buffer_.push_back(0xdb);
                    buffer_.push_back(data[i]);
                }
                state_ = normal;
                break;
        }
    }
}
