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

    class protocol_t;

    unsigned short crc16(const unsigned char * data, size_t len);

    /**
     *
     *
     **/
    namespace tm_code {
        // Коды сообщений. Префиксом обзначается сторона, инициирующая отправку.
        // "c" - сообщение отправляется с бортового контроллера, 
        // "s" - с коммуникационного сервера
        enum {
            c_controller_ident  = 0x80,
            s_server_ident      = 0x81,
            s_data_request      = 0x85,
            c_archive_data      = 0x86,
            s_delete_data       = 0x87,
            c_delete_confirm    = 0x88,
            c_current_data      = 0x95
        };
    }

    /**
     *
     *
     **/
#   pragma pack(push)
#   pragma pack(1)

    /**
     * Заголовок транспортного сообщения
     *
     **/
    struct transport_header_t {

        typedef unsigned short ushort;
        typedef unsigned char uchar;

        unsigned char prefix;       // Префикс транспортного сообщения 0xC0
        unsigned char cmd;          // Команда
        unsigned short data_len;    // Размер поля data
        unsigned char data[0];      // Данные

        transport_header_t(uchar command, unsigned short len):
            prefix(0xC0),
            cmd(command),
            data_len(len) {
        }

        unsigned short get_crc() const {
            return static_cast<ushort>(data[data_len]) << 8 | data[data_len + 1];
        }

        unsigned short calc_crc() const {
            return crc16(reinterpret_cast<const uchar *>(&cmd), data_len + sizeof(cmd) + sizeof(data_len));
        }

        bool check_crc() const {
            return get_crc() == calc_crc();
        }

        size_t get_full_length() const {
            return sizeof(transport_header_t) + data_len + 2;
        }
    };

    /**
     * Базовый класс транспортных сообщений
     *
     **/
    template<size_t C, class T>
    struct transport_message_t : public transport_header_t {
        transport_message_t() :
            transport_header_t(C, sizeof(T) - sizeof(transport_header_t)) {
        }

        void check_length() const {
            if (sizeof(transport_header_t) + data_len < sizeof(T)) {
                throw std::length_error("Wrong message size");
            }
        }
    };

    /**
     * Сообщение 0x80 "идентификация регистратора"
     *
     **/
    struct controller_ident_t : public transport_message_t<tm_code::c_controller_ident, controller_ident_t> {
        unsigned int controller_id;     // Номер контроллера
        unsigned int firmware_version;  // Версия прошивки
    };

    /**
     * Сообщение 0x85 "запрос данных"
     *
     **/
    struct data_request_t : public transport_message_t<tm_code::s_data_request, data_request_t> {
        
        unsigned int mes_number;     // Сквозной номер записи

        data_request_t(unsigned int mes_number) :
            mes_number(mes_number) {
        }
    };

    /**
     * Сообщение 0x86 "данные архива"
     *
     **/
    struct data_response_t : public transport_message_t<tm_code::s_data_request, data_response_t> {
        
        unsigned int last_mes_number;   // Сквозной номер последнего сообщения в архиве
        unsigned int first_mes_time;    // Время первого сообщения в архиве (omnicommtime)
        unsigned char priority;         // Приоритет контейнера, всех сообщений (резерв, обычно 0)
        unsigned char im_data[0];       // Цепочка информационных сообщений

        size_t get_im_length() const {
            return get_full_length() - sizeof(data_response_t) - 2;
        }
    };

    /**
     * Информационное сообщение
     *
     **/
    struct info_message_t {
        unsigned short data_size;   // Размер блока данных
        unsigned char data[0];      // Блок данных, упакованных protobuf-ом

        size_t size() const {
            return data_size + sizeof(info_message_t);
        }
    };

#   pragma pack(pop)

    /**
     *
     *
     **/
    class transp_protocol_t {
    public:
        typedef std::vector<unsigned char> array_type;

    public:
        transp_protocol_t(protocol_t & impl);

    public:
        array_type parse(const transport_header_t * hdr);

    private:

        // Обработка транспортных сообщения
        array_type controller_ident(const transport_header_t * hdr);
        array_type archive_data_response(const transport_header_t * hdr);
        array_type current_data_response(const transport_header_t * hdr);
        size_t process_info_messages(const data_response_t * response);

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
        protocol_t & impl_;
        unsigned controller_id_;
        unsigned firmware_version_;
    };
}

#endif // OMNICOMM_TRANSP_HPP
