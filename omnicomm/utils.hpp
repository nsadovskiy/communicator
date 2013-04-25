/**
 *
 *
 **/
#ifndef OMNICOMM_UTILS_HPP
#define OMNICOMM_UTILS_HPP

#include <string>
#include <cstddef>

/**
 *
 *
 **/
namespace omnicomm {


    /**
     *
     *
     **/
    unsigned short crc16(const unsigned char * data, size_t len);
    std::string otime_to_string(size_t omnicomm_time);
 }

#endif // OMNICOMM_UTILS_HPP
