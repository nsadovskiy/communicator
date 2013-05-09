#define BOOST_AUTO_TEST_MAIN

#include <boost/test/unit_test.hpp>

#include "../config.h"

#ifdef ORACLE_FOUND
#include "../backends/oracle.hpp"
#endif
 
#define BOOST_TEST_MODULE testCommunicator

#ifdef ORACLE_FOUND

BOOST_AUTO_TEST_CASE(test_oracle_backend) {
    communicator::backend::oracle_t oracle_backend("login", "password", "10.10.3.23:1234/v3", false);
    BOOST_CHECK_EQUAL(oracle_backend.get_login(), "login");
    BOOST_CHECK_EQUAL(oracle_backend.get_password(), "password");
    BOOST_CHECK_EQUAL(oracle_backend.get_server(), "10.10.3.23");
    BOOST_CHECK_EQUAL(oracle_backend.get_port(), "1234");
    BOOST_CHECK_EQUAL(oracle_backend.get_servicename(), "v3");
}

#endif
