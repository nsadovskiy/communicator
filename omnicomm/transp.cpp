/**
 *
 *
 **/
#include "transp.hpp"
#include "../utils.hpp"

/**
 *
 *
 **/
omnicomm::transp_protocol_t::transp_protocol_t(protocol_t & impl) :
    log_(log4cplus::Logger::getInstance("main")),
    impl_(impl) {
}

/**
 *
 *
 **/
omnicomm::transp_protocol_t::array_type omnicomm::transp_protocol_t::parse(const transport_header_t * hdr) {

    switch (hdr->cmd) {

        case tm_code::c_controller_ident:
            return controller_ident(hdr);

        case tm_code::c_data:
            return data_response(hdr);

        case tm_code::c_delete_confirm:
            break;

        default:
            break;
    }

    return array_type();
}

/**
 *
 *
 **/
omnicomm::transp_protocol_t::array_type omnicomm::transp_protocol_t::controller_ident(const transport_header_t * hdr) {
    
    const controller_ident_t * msg = reinterpret_cast<const controller_ident_t *>(hdr);
    msg->check_length();
    LOG4CPLUS_INFO(log_, "controller_ident[controller_id=" << msg->controller_id << ", firmware version=" << msg->firmware_version << "]");

    data_request_t request(0);
    const unsigned char * request_ptr = reinterpret_cast<const unsigned char *>(&request);

    return array_type(request_ptr, &request_ptr[sizeof(data_request_t)]);
}

/**
 *
 *
 **/
omnicomm::transp_protocol_t::array_type omnicomm::transp_protocol_t::data_response(const transport_header_t * hdr) {

    const data_response_t * msg = reinterpret_cast<const data_response_t *>(hdr);
    LOG4CPLUS_INFO(log_, "data_response[last_mes_number=" << msg->last_mes_number << ", first_mes_time=" << msg->first_mes_time << " priority=" << static_cast<int>(msg->priority) << "]");

    return array_type();
}
