/**
 *
 *
 **/
#include <iomanip>
#include <sstream>
#include "utils.hpp"

using std::hex;
using std::setw;
using std::string;
using std::setfill;
using std::ostringstream;

/**
 *
 *
 **/
string to_hex_string(const unsigned char * buffer, size_t len) {
    ostringstream strm;
    // strm << hex;
    for (size_t i = 0; i < len; ++i) {
        if (!!i) {
            strm << " ";
        }
        strm << hex << setw(2) << setfill('0') << int(buffer[i]);
    }
    return strm.str();
}
