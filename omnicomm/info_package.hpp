/**
 *
 *
 **/
#ifndef OMNICOMM_TRANSP_HPP
#define OMNICOMM_TRANSP_HPP

#include <map>
#include <string>
#include <vector>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <log4cplus/logger.h>


/**
 *
 *
 **/
class RecReg;
class RecReg_General;
class RecReg_Photo;
class RecReg_NAV;
class RecReg_UniDt;
class RecReg_CanDt;
class RecReg_LLSDt;
class RecReg_LOG;

/**
 *
 *
 **/
namespace omnicomm {

    class data_response_t;
    class transport_header_t;
    class transport_protocol_t;

    unsigned short crc16(const unsigned char * data, size_t len);

    typedef std::vector<std::string> message_array_type;

    void parse_info_package(const data_response_t & msg_pack, message_array_type & messages);

    /**
     *
     *
     **/
    class info_protocol_t {
    public:
        typedef std::vector<unsigned char> array_type;
        typedef std::vector<std::string> message_array_type;

    public:
        info_protocol_t(transport_protocol_t & impl);

    public:
        array_type parse(const transport_header_t & hdr, message_array_type & messages);

    private:

        // Обработка транспортных сообщения
        array_type controller_ident(const transport_header_t & hdr);
        array_type archive_data_response(const transport_header_t & hdr, message_array_type & messages);
        array_type current_data_response(const transport_header_t & hdr, message_array_type & messages);
        size_t process_info_messages(const data_response_t & response, message_array_type & messages);

        void extract(unsigned message_id, const RecReg & message, std::map<std::string, std::string> & data);
        void extract(const RecReg_General & message, std::map<std::string, std::string> & data);
        void extract(const RecReg_Photo & message);
        void extract(const RecReg_NAV & message, std::map<std::string, std::string> & data);
        void extract(const RecReg_UniDt & message, std::map<std::string, std::string> & data);
        void extract(const RecReg_CanDt & message, std::map<std::string, std::string> & data);
        void extract(const RecReg_LLSDt & message, std::map<std::string, std::string> & data);
        void extract(const RecReg_LOG & message);


    private:
        log4cplus::Logger log_;
        transport_protocol_t & impl_;
        unsigned controller_id_;
        unsigned firmware_version_;
    };
}

#endif // OMNICOMM_TRANSP_HPP
