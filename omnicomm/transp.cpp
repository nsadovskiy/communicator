/**
 *
 *
 **/
#include <sstream>
#include <iomanip>
#include "utils.hpp"
#include "transp.hpp"
#include "../utils.hpp"
#include "message.pb.h"

using std::hex;
using std::map;
using std::fixed;
using std::string;
using std::setprecision;
using std::ostringstream;

template<class T>
std::basic_ostream<T> & operator <<(std::basic_ostream<T> & stream, const std::map<std::string, std::string> & map) {

    for (auto i = map.begin(); i != map.end(); ++i) {
        if (i != map.begin()) {
            stream << " ";
        }
        stream << (*i).first << "=\"" << (*i).second << "\"";
    }

    return stream;
};

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
omnicomm::transp_protocol_t::array_type omnicomm::transp_protocol_t::parse(const transport_header_t * hdr, message_array_type & messages) {

    switch (hdr->cmd) {

        case tm_code::c_controller_ident:
            return controller_ident(hdr);

        case tm_code::c_current_data:
            return current_data_response(hdr, messages);

        case tm_code::c_archive_data:
            return archive_data_response(hdr, messages);

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
    controller_id_ = msg->controller_id;
    firmware_version_ = msg->firmware_version;
    LOG4CPLUS_INFO(log_, "controller_ident[controller_id=" << msg->controller_id << ", firmware version=" << msg->firmware_version << "]");

    data_request_t request(0);
    const unsigned char * request_ptr = reinterpret_cast<const unsigned char *>(&request);

    return array_type(request_ptr, &request_ptr[sizeof(data_request_t)]);
}

/**
 *
 *
 **/
omnicomm::transp_protocol_t::array_type omnicomm::transp_protocol_t::archive_data_response(const transport_header_t * hdr, message_array_type & messages) {

    const data_response_t * msg = reinterpret_cast<const data_response_t *>(hdr);
    msg->check_length();
    LOG4CPLUS_TRACE(log_, "archive_data_response[last_mes_number=" << msg->last_mes_number << ", first_mes_time=" << omnicomm::otime_to_string(msg->first_mes_time) << " priority=" << static_cast<int>(msg->priority) << "]");
    LOG4CPLUS_TRACE(log_, "archive_data_response[data=[" << to_hex_string(msg->im_data, msg->get_im_length()) << "]]");

    process_info_messages(msg, messages);

    return array_type();
}

/**
 *
 *
 **/
omnicomm::transp_protocol_t::array_type omnicomm::transp_protocol_t::current_data_response(const transport_header_t * hdr, message_array_type & messages) {

    const data_response_t * msg = reinterpret_cast<const data_response_t *>(hdr);
    msg->check_length();
    LOG4CPLUS_TRACE(log_, "current_data_response[last_mes_number=" << msg->last_mes_number << ", first_mes_time=" << msg->first_mes_time << " priority=" << static_cast<int>(msg->priority) << "]");
    LOG4CPLUS_TRACE(log_, "current_data_response[data=[" << to_hex_string(msg->im_data, msg->get_im_length()) << "]]");

    process_info_messages(msg, messages);

    return array_type();
}

/**
 *
 *
 **/
size_t omnicomm::transp_protocol_t::process_info_messages(const data_response_t * response, message_array_type & messages) {

    ostringstream str;
    map<string, string> data;
    const info_message_t * msg = nullptr;
    size_t i = 0, im_data_size = response->get_im_length();

    while (i < im_data_size) {
        msg = reinterpret_cast<const info_message_t *>(&response->im_data[i]);
        LOG4CPLUS_TRACE(log_, "info_message[len=" << msg->data_size << "]");
        if (i + msg->size() > im_data_size) {
            throw std::length_error("Unexpected end of information message data block");
        }
        LOG4CPLUS_TRACE(log_, "info_message[data=[" << to_hex_string(msg->data, msg->data_size) << "]]");

        RecReg message;

        if (!message.ParseFromArray(msg->data, msg->data_size)) {
            LOG4CPLUS_ERROR(log_, "Wrong message format");
        } else {
            LOG4CPLUS_TRACE(log_, "Protobuf message parsed");
        }

        for (int i = 0; i < message.mid_size(); ++i) {
            LOG4CPLUS_TRACE(log_, "message [mID=" << hex << message.mid(i) << "]");

            data.clear();
            extract(message.mid(i), message, data);

            str.str("");
            str.clear();
            str << data;
            messages.push_back(str.str());
            
            LOG4CPLUS_DEBUG(log_, "Message [" << data << "]");
        }

        i += msg->size();
    }

    return 0;
}

/**
 *
 *
 **/
void omnicomm::transp_protocol_t::extract(unsigned message_id, const RecReg & message, map<string, string> & data) {

    ostringstream str;

    str.str("");
    str.clear();
    str << message_id;
    data["message_id"] = str.str();

    str.str("");
    str.clear();
    str << controller_id_;
    data["controller_id"] = str.str();

    str.str("");
    str.clear();
    str << firmware_version_;
    data["firmware"] = str.str();

    if (message.has_general()) {
        extract(message.general(), data);
    }

    if (message.has_nav()) {
        extract(message.nav(), data);
    }

    if (message.has_unidt()) {
        extract(message.unidt(), data);
    }

    if (message.has_llsdt()) {
        extract(message.llsdt(), data);
    }

    if (message.has_candt()) {
        extract(message.candt(), data);
    }
}

/**
 *
 *
 **/
void omnicomm::transp_protocol_t::extract(const RecReg_General & message, map<string, string> & data) {

    ostringstream str;

    if (message.has_time()) {
        str.str("");
        str.clear();
        str << omnicomm::otime_to_string(message.time());
        data["time"] = str.str();
    }

    if (message.has_idfas()) {
        str.str("");
        str.clear();
        str << message.idfas();
        data["controller_id"] = str.str();
    }

    if (message.has_iddrv()) {
        str.str("");
        str.clear();
        // str << message.iddrv();
        // data["driver_id"] = str.str();
    }

    if (message.has_flg()) {
        str.str("");
        str.clear();
        str << message.flg();
        data["flags"] = str.str();
    }

    if (message.has_mileage()) {
        str.str("");
        str.clear();
        str << message.mileage();
        data["mileage"] = str.str();
    }

    if (message.has_vimp()) {
        str.str("");
        str.clear();
        str << message.vimp();
        data["speed_by_sensor"] = str.str();
    }

    if (message.has_timp()) {
        str.str("");
        str.clear();
        str << message.timp();
        data["engine_speed"] = str.str();
    }

    if (message.has_uboard()) {
        str.str("");
        str.clear();
        str << (message.uboard() * 100);
        data["power_main"] = str.str();
    }

    if (message.has_batlife()) {
        str.str("");
        str.clear();
        str << message.batlife();
        data["battery_level"] = str.str();
    }

    if (message.has_sumacc()) {
        str.str("");
        str.clear();
        str << fixed << setprecision(2) << (message.sumacc() * 1e-2);
        data["a_total"] = str.str();
    }

    if (message.has_phone()) {
        data["phone"] = message.phone();
    }

    if (message.has_amtrx()) {
        str.str("");
        str.clear();
        str << message.amtrx();
        data["a_x"] = str.str();
    }

    if (message.has_amtry()) {
        str.str("");
        str.clear();
        str << message.amtry();
        data["a_y"] = str.str();
    }

    if (message.has_amtrz()) {
        str.str("");
        str.clear();
        str << message.amtrz();
        data["a_z"] = str.str();
    }
}

/**
 *
 *
 **/
void omnicomm::transp_protocol_t::extract(const RecReg_Photo & message) {
    // TODO: Доделать обработку сообщений с фоточками. Вдруг, водила начнет няшных
    // котиков фотать, а у нас, блять, поддержки нет. 
}

/**
 *
 *
 **/
void omnicomm::transp_protocol_t::extract(const RecReg_NAV & message, std::map<std::string, std::string> & data) {

    ostringstream str;

    str.str("");
    str.clear();
    str << fixed << setprecision(6) << (message.lat() * 1e-7f);
    data["lat"] = str.str();

    str.str("");
    str.clear();
    str << fixed << setprecision(6) << (message.lon() * 1e-7f);
    data["lon"] = str.str();

    str.str("");
    str.clear();
    str << fixed << setprecision(1) << (message.gpsvel() * 1e-1f);
    data["speed_by_gps"] = str.str();

    str.str("");
    str.clear();
    str << message.gpsdir();
    data["course"] = str.str();

    str.str("");
    str.clear();
    str << message.gpsnsat();
    data["satellites"] = str.str();

    str.str("");
    str.clear();
    str << fixed << setprecision(2) << (message.gpsalt() * 1e-1f);
    data["altitude"] = str.str();
}

/**
 *
 *
 **/
void omnicomm::transp_protocol_t::extract(const RecReg_UniDt & message, std::map<std::string, std::string> & data) {

    ostringstream str;

    if (message.has_unival0()) {
        str.str("");
        str.clear();
        str << message.unival0();
        data["univ0"] = str.str();
    }

    if (message.has_unival1()) {
        str.str("");
        str.clear();
        str << message.unival1();
        data["univ1"] = str.str();
    }

    if (message.has_unival2()) {
        str.str("");
        str.clear();
        str << message.unival2();
        data["univ2"] = str.str();
    }

    if (message.has_unival3()) {
        str.str("");
        str.clear();
        str << message.unival3();
        data["univ3"] = str.str();
    }
}

/**
 *
 *
 **/
void omnicomm::transp_protocol_t::extract(const RecReg_CanDt & message, std::map<std::string, std::string> & data) {
    // TODO: Доделать обработку сообщений CAN
}

/**
 *
 *
 **/
void omnicomm::transp_protocol_t::extract(const RecReg_LLSDt & message, std::map<std::string, std::string> & data) {

    ostringstream str;

    // LLS1
    if (message.has_clls1()) {
        str.str("");
        str.clear();
        str << message.clls1();
        data["lls1_value"] = str.str();
    }

    if (message.has_tlls1()) {
        str.str("");
        str.clear();
        str << message.tlls1();
        data["lls1_temp"] = str.str();
    }

    if (message.has_flls1()) {
        str.str("");
        str.clear();
        str << message.flls1();
        data["lls1_flags"] = str.str();
    }

    // LLS2
    if (message.has_clls2()) {
        str.str("");
        str.clear();
        str << message.clls2();
        data["lls2_value"] = str.str();
    }

    if (message.has_tlls2()) {
        str.str("");
        str.clear();
        str << message.tlls2();
        data["lls2_temp"] = str.str();
    }

    if (message.has_flls2()) {
        str.str("");
        str.clear();
        str << message.flls2();
        data["lls2_flags"] = str.str();
    }

    // LLS3
    if (message.has_clls3()) {
        str.str("");
        str.clear();
        str << message.clls3();
        data["lls3_value"] = str.str();
    }

    if (message.has_tlls3()) {
        str.str("");
        str.clear();
        str << message.tlls3();
        data["lls3_temp"] = str.str();
    }

    if (message.has_flls3()) {
        str.str("");
        str.clear();
        str << message.flls3();
        data["lls3_flags"] = str.str();
    }

    // LLS4
    if (message.has_clls4()) {
        str.str("");
        str.clear();
        str << message.clls4();
        data["lls4_value"] = str.str();
    }

    if (message.has_tlls4()) {
        str.str("");
        str.clear();
        str << message.tlls4();
        data["lls4_temp"] = str.str();
    }

    if (message.has_flls4()) {
        str.str("");
        str.clear();
        str << message.flls4();
        data["lls4_flags"] = str.str();
    }

    // LLS5
    if (message.has_clls5()) {
        str.str("");
        str.clear();
        str << message.clls5();
        data["lls5_value"] = str.str();
    }

    if (message.has_tlls5()) {
        str.str("");
        str.clear();
        str << message.tlls5();
        data["lls5_temp"] = str.str();
    }

    if (message.has_flls5()) {
        str.str("");
        str.clear();
        str << message.flls5();
        data["lls5_flags"] = str.str();
    }

    // LLS6
    if (message.has_clls6()) {
        str.str("");
        str.clear();
        str << message.clls6();
        data["lls6_value"] = str.str();
    }

    if (message.has_tlls6()) {
        str.str("");
        str.clear();
        str << message.tlls6();
        data["lls6_temp"] = str.str();
    }

    if (message.has_flls6()) {
        str.str("");
        str.clear();
        str << message.flls6();
        data["lls6_flags"] = str.str();
    }
}

/**
 *
 *
 **/
void omnicomm::transp_protocol_t::extract(const RecReg_LOG & message) {
    // TODO: Доделать обработку сообщений с отладочными логами
}