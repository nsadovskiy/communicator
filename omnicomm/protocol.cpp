/**
 *
 *
 **/
#include "protocol.hpp"

#include <sstream>
#include <cassert>
#include <iomanip>
#include <algorithm>
#include <log4cplus/loggingmacros.h>

#include "utils.hpp"
#include "../utils.hpp"
#include "../client.hpp"
#include "../backends/base.hpp"
#include "transport_messages.hpp"

using std::find;
using std::distance;
using std::exception;

/**
 *
 *
 **/
namespace stuff_state {
    enum {
        normal,
        possible_stuff
    };
}

namespace protocol_state {
    enum {
        wait_initial,
        wait_initial_delete_confirm,
        process_archive,
        process_current
    };
}

/**
 *
 *
 **/
omnicomm::transport_protocol_t::counterer_type omnicomm::transport_protocol_t::counterer_;
boost::mutex omnicomm::transport_protocol_t::mutex_;

/**
 *
 *
 **/
unsigned omnicomm::transport_protocol_t::get_last_counter(unsigned controller_id) {

    unsigned result = 0;

    lock_guard_type lock(mutex_);

    if (counterer_.count(controller_id) > 0) {
        result = counterer_[controller_id];
    }

    return result;
}

/**
 *
 *
 **/
void omnicomm::transport_protocol_t::set_last_counter(unsigned controller_id, unsigned counter) {
    lock_guard_type lock(mutex_);
    counterer_[controller_id] = counter;
}

/**
 *
 *
 **/
omnicomm::transport_protocol_t::transport_protocol_t() :
    log_(log4cplus::Logger::getInstance("main")),
    state_(stuff_state::normal),
    protocol_state_(protocol_state::wait_initial),
    info_protocol_(*this) {

}

/**
 *
 *
 **/
void omnicomm::transport_protocol_t::init_impl() {
}

/**
 *
 *
 **/
void omnicomm::transport_protocol_t::connect_impl() {
}

/**
 *
 *
 **/
void omnicomm::transport_protocol_t::recive_impl(const unsigned char * data, size_t len) {

    LOG4CPLUS_TRACE(log_, "Data: [" << to_hex_string(data, len).c_str() << "]");
    store_data(data, len);
    LOG4CPLUS_TRACE(log_, "Destuffed data: [" << to_hex_string(&buffer_[0], buffer_.size()).c_str() << "]");

    ptrdiff_t remain_len;
    array_type::iterator start = buffer_.begin(), end = buffer_.begin();

    while (start != buffer_.end()) {

        start = find(start, buffer_.end(), 0xc0);
        
        if (start != buffer_.end()) {

            remain_len = distance(start, buffer_.end());

            LOG4CPLUS_TRACE(log_, "Verifying header length");
            if (remain_len < static_cast<ptrdiff_t>(sizeof(transport_header_t))) {
                LOG4CPLUS_TRACE(log_, "Message length less than header size");
                break;
            }

            LOG4CPLUS_TRACE(log_, "Verifying full length");
            const transport_header_t * hdr = reinterpret_cast<const transport_header_t *>(&start[0]);

            if (remain_len < static_cast<ptrdiff_t>(hdr->get_full_length())) {
                LOG4CPLUS_TRACE(log_, "Message length less than full size. Header lenght=" << hdr->get_full_length() << " but readed " << remain_len << " bytes");
                break;
            }

            LOG4CPLUS_TRACE(log_, "Verifying checksum");
            if (!hdr->check_crc()) {
                LOG4CPLUS_WARN(log_, "Checksum error");
                get_manipulator()->stop();
                break;
                // ++start;
                // end = start;
                // continue;
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
            LOG4CPLUS_TRACE(log_, "   data: [" << to_hex_string(hdr->get_data(), hdr->data_len).c_str() << "]");

            process_message(*hdr);

            start += hdr->get_full_length();
            end = start;
        }
    }

    LOG4CPLUS_TRACE(log_, "Dropping " << std::distance(buffer_.begin(), start) << " bytes");
    buffer_.erase(buffer_.begin(), end);
    LOG4CPLUS_TRACE(log_, "Remain after dropping: [" << to_hex_string(&buffer_[0], buffer_.size()).c_str() << "]");
}

/**
 *
 *
 **/
void omnicomm::transport_protocol_t::process_message(const transport_header_t & hdr) {

    try {

        switch (hdr.cmd) {

            case tm_code::c_controller_ident:
                process_controller_ident(hdr);
                break;

            case tm_code::c_current_data:
                process_current_data(hdr);
                break;

            case tm_code::c_archive_data:
                process_archive_data(hdr);
                break;

            case tm_code::c_delete_confirm:
                process_delete_confirmation(hdr);
                break;

            default:
                break;
        }

    } catch (const exception & e) {
        LOG4CPLUS_ERROR(log_, e.what());
        get_manipulator()->stop();
        throw;

    } catch (...) {
        LOG4CPLUS_ERROR(log_, "[omnicomm::transport_protocol_t::process_message] Unexpected exception");
        get_manipulator()->stop();
        throw;
    }
}

/**
 *
 *
 **/
void omnicomm::transport_protocol_t::process_delete_confirmation(const transport_header_t & hdr) {

    const delete_confirmation_t & msg = static_cast<const delete_confirmation_t &>(hdr);

    LOG4CPLUS_TRACE(log_, "{" << controller_id_ << "} delete_data_confirmation [message=" << msg.mes_number << "]");

    if (protocol_state::wait_initial_delete_confirm == protocol_state_) {
        
        LOG4CPLUS_TRACE(log_, "{" << controller_id_ << "} Initial data delete complete. Start reading archive.");
        
        set_last_counter(controller_id_, msg.mes_number);
        
        data_request_t request(msg.mes_number);
        send(reinterpret_cast<const unsigned char *>(&request), sizeof(data_request_t));
        
        protocol_state_ = protocol_state::process_archive;
    }
}

/**
 *
 *
 **/
void omnicomm::transport_protocol_t::process_controller_ident(const transport_header_t & hdr) {

    const controller_ident_t & msg = static_cast<const controller_ident_t &>(hdr);

    msg.check_length();

    controller_id_ = msg.controller_id;
    firmware_version_ = msg.firmware_version;

    unsigned last_counter = get_last_counter(controller_id_);
    if (last_counter > 0) {
        ++last_counter;
    }
    LOG4CPLUS_INFO(log_, "{" << controller_id_ << "} controller_ident[controller_id=" << msg.controller_id << ", firmware version=" << msg.firmware_version << ", last recieved id=" << last_counter << "]");

    if (0 == last_counter) {

        LOG4CPLUS_TRACE(log_, "{" << controller_id_ << "} First controller appearance. Request from 0.");

        data_request_t request(0);
        send(reinterpret_cast<const unsigned char *>(&request), sizeof(data_request_t));

        protocol_state_ = protocol_state::process_archive;

    } else {

        LOG4CPLUS_TRACE(log_, "{" << controller_id_ << "} Last accepted message: " << last_counter);

        LOG4CPLUS_TRACE(log_, "{" << controller_id_ << "} Send delete_data[id=" << last_counter << "]");
        delete_data_t request(last_counter);
        send(reinterpret_cast<const unsigned char *>(&request), sizeof(delete_data_t));

        protocol_state_ = protocol_state::wait_initial_delete_confirm;
    }
}

/**
 *
 *
 **/
void omnicomm::transport_protocol_t::process_archive_data(const transport_header_t & hdr) {

    const data_response_t & msg = static_cast<const data_response_t &>(hdr);

    if (0 == msg.data_len) {

        unsigned last_counter = get_last_counter(controller_id_);

        LOG4CPLUS_TRACE(log_, "{" << controller_id_ << "} archive download completed successful. Deleting archive data till message " << last_counter);

        protocol_state_ = protocol_state::process_current;

        LOG4CPLUS_TRACE(log_, "{" << controller_id_ << "} Send delete_data[id=" << last_counter << "]");
        delete_data_t request(last_counter);
        send(reinterpret_cast<const unsigned char *>(&request), sizeof(delete_data_t));

    } else {

        msg.check_length();

        LOG4CPLUS_TRACE(log_, "{" << controller_id_ << "} archive_data_response[last_mes_number=" << msg.last_mes_number << ", first_mes_time=" << omnicomm::otime_to_string(msg.first_mes_time) << " priority=" << static_cast<int>(msg.priority) << "]");
        LOG4CPLUS_TRACE(log_, "{" << controller_id_ << "} archive_data_response[data=[" << to_hex_string(msg.im_data, msg.get_im_length()) << "]]");

        process_info_messages(msg);
        set_last_counter(controller_id_, msg.last_mes_number);
    }
}

/**
 *
 *
 **/
void omnicomm::transport_protocol_t::process_current_data(const transport_header_t & hdr) {

    const data_response_t & msg = static_cast<const data_response_t &>(hdr);

    msg.check_length();

    LOG4CPLUS_TRACE(log_, "{" << controller_id_ << "} current_data_response[last_mes_number=" << msg.last_mes_number << ", first_mes_time=" << msg.first_mes_time << " priority=" << static_cast<int>(msg.priority) << "]");
    LOG4CPLUS_TRACE(log_, "{" << controller_id_ << "} current_data_response[data=[" << to_hex_string(msg.im_data, msg.get_im_length()) << "]]");

    process_info_messages(msg);
    set_last_counter(controller_id_, msg.last_mes_number);

    LOG4CPLUS_TRACE(log_, "{" << controller_id_ << "} Send delete_data[id=" << msg.last_mes_number << "]");
    delete_data_t request(msg.last_mes_number);
    send(reinterpret_cast<const unsigned char *>(&request), sizeof(delete_data_t));
}

/**
 *
 *
 **/
void omnicomm::transport_protocol_t::process_info_messages(const data_response_t & msg) {

    info_protocol_t::message_array_type messages;

    // try {

        // omnicomm::info_protocol_t::array_type result = info_protocol_.parse(hdr, messages);
        omnicomm::parse_info_package(msg, messages);
        
        // if (!result.empty()) {
        //     unsigned short crc = reinterpret_cast<const transport_header_t *>(&result[0])->calc_crc();
        //     result.push_back(static_cast<unsigned char>(crc & 0xFF));
        //     result.push_back(static_cast<unsigned char>(crc >> 8));
        //     send(&result[0], result.size());
        // }

        std::ostringstream message;

        for (auto msg: messages) {
            message.str("");
            message.clear();
            message << "control_id=\"" << controller_id_ << "\" firmware=\"" << firmware_version_ << "\" " << msg;
            get_manipulator()->get_backend().add_message(message.str());
        }

        // delete_data_t request(msg.last_mes_number);
        // send(reinterpret_cast<const unsigned char *>(&request), sizeof(data_request_t));

    // } catch(const exception & e) {
    //     LOG4CPLUS_ERROR(log_, e.what());
    // }
}

/**
 *
 *
 **/
void omnicomm::transport_protocol_t::send(const unsigned char * data, size_t len) {

    if (1 > len) {
        return;
    }

    omnicomm::info_protocol_t::array_type buffer;
    for (size_t i = 0; i < len; ++i) {
        if (0xc0 == data[i] && i) {
            buffer.push_back(0xdb);
            buffer.push_back(0xdc);
        } else if (0xdb == data[i] && i) {
            buffer.push_back(0xdb);
            buffer.push_back(0xdd);
        } else {
            buffer.push_back(data[i]);
        }
    }
    
    unsigned short crc = crc16(&data[1], len - 1);
    buffer.push_back(static_cast<unsigned char>(crc & 0xFF));
    buffer.push_back(static_cast<unsigned char>(crc >> 8));
    
    LOG4CPLUS_TRACE(log_, "{" << controller_id_ << "}    sending: [" << to_hex_string(&buffer[0], buffer.size()).c_str() << "]");
    
    get_manipulator()->send(&buffer[0], buffer.size());
}

/**
 *
 *
 **/
void omnicomm::transport_protocol_t::store_data(const unsigned char * data, size_t len) {

    using stuff_state::normal;
    using stuff_state::possible_stuff;

    for (size_t i = 0; i < len; ++i) {

        switch (state_) {
            
            case normal:
                if (0xdb == data[i]) {
                    state_ = possible_stuff;
                } else {
                    buffer_.push_back(data[i]);
                }
                break;

            case possible_stuff:
                if (0xdc == data[i]) {
                    buffer_.push_back(0xc0);
                } else if (0xdd == data[i]) {
                    buffer_.push_back(0xdb);
                } else {
                    // Если символы 0x0c и 0xdb экранированы, по-идее, мы сюда попасть не должны...
                    LOG4CPLUS_WARN(log_, "{" << controller_id_ << "} Unexpected character in data stream");
                    buffer_.push_back(0xdb);
                    buffer_.push_back(data[i]);
                }
                state_ = normal;
                break;
        }
    }
}
