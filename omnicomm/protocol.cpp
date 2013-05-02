/**
 *
 *
 **/
#include "protocol.hpp"

#include <cassert>
#include <iomanip>
#include <algorithm>

#include "utils.hpp"
#include "../utils.hpp"
#include "../client.hpp"
#include "../backends/base.hpp"

using std::find;
using std::distance;
using std::exception;

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
    log_(log4cplus::Logger::getInstance("main")),
    state_(stuff_state::normal),
    transport_protocol_(*this) {

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

    LOG4CPLUS_TRACE(log_, "Data: [" << to_hex_string(data, len).c_str() << "]");
    store_data(data, len);
    LOG4CPLUS_TRACE(log_, "Destuffed data: [" << to_hex_string(&buffer_[0], buffer_.size()).c_str() << "]");

    ptrdiff_t remain_len;
    array_type::iterator start= buffer_.begin();
    transp_protocol_t::message_array_type messages;

    while (start != buffer_.end()) {
        
        start = find(start, buffer_.end(), 0xc0);
        
        if (start != buffer_.end()) {

            remain_len = distance(start, buffer_.end());

            if (remain_len < static_cast<ptrdiff_t>(sizeof(transport_header_t))) {
                LOG4CPLUS_WARN(log_, "Message length less than header size");
                break;
            }

            const transport_header_t * hdr = reinterpret_cast<const transport_header_t *>(&start[0]);

            if (remain_len < static_cast<ptrdiff_t>(hdr->get_full_length())) {
                LOG4CPLUS_TRACE(log_, "Message length less than full size");
                break;
            }

            if (!hdr->check_crc()) {
                LOG4CPLUS_WARN(log_, "Checksum error. Message ignored.");
                ++start;
                continue;
            } else {
                LOG4CPLUS_TRACE(log_, "Checksum OK");
            }

            using std::dec;
            using std::hex;

            LOG4CPLUS_TRACE(log_,
                    "Transport message [prefix=" 
                    << hex
                    << int(hdr->prefix)
                    << " command=" << int(hdr->cmd)
                    << dec
                    << " data length="  << hdr->data_len
                    << " full length=" << hdr->get_full_length()
                    << hex
                    << " crc=" << hdr->get_crc()
                );
            LOG4CPLUS_TRACE(log_, "   data: [" << to_hex_string(hdr->data, hdr->data_len).c_str() << "]");

            try {
                
                messages.clear();
                omnicomm::transp_protocol_t::array_type result = transport_protocol_.parse(hdr, messages);
                
                if (!result.empty()) {
                    unsigned short crc = reinterpret_cast<const transport_header_t *>(&result[0])->calc_crc();
                    result.push_back(static_cast<unsigned char>(crc & 0xFF));
                    result.push_back(static_cast<unsigned char>(crc >> 8));
                    send(&result[0], result.size());
                }

                for (auto msg: messages) {
                    get_manipulator()->get_backend().add_message(msg);
                }

            } catch(const exception & e) {
                LOG4CPLUS_ERROR(log_, e.what());
            }

            start += hdr->get_full_length();
        }
    }

    buffer_.erase(buffer_.begin(), start);
}

/**
 *
 *
 **/
void omnicomm::protocol_t::send(const unsigned char * data, size_t len) {

    omnicomm::transp_protocol_t::array_type buffer;
    for (size_t i = 0; i < len; ++i) {
        if (data[i] == 0xc0 && i) {
            buffer.push_back(0xdb);
            buffer.push_back(0xdc);
        } else if (data[i] == 0xdb && i) {
            buffer.push_back(0xdb);
            buffer.push_back(0xdd);
        } else {
            buffer.push_back(data[i]);
        }
    }
    LOG4CPLUS_TRACE(log_, "   sending: [" << to_hex_string(&buffer[0], buffer.size()).c_str() << "]");
    get_manipulator()->send(&buffer[0], buffer.size());
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
                    buffer_.push_back(0xc0);
                } else if (data[i] == 0xdd) {
                    buffer_.push_back(0xdb);
                } else {
                    // Если символы 0x0c и 0xdb экранированы, по-идее, мы сюда попасть не должны...
                    LOG4CPLUS_WARN(log_, "Unexpected character in data stream");
                    buffer_.push_back(0xdb);
                    buffer_.push_back(data[i]);
                }
                state_ = normal;
                break;
        }
    }
}
