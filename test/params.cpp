#define BOOST_AUTO_TEST_MAIN

#include <boost/test/unit_test.hpp>

#include "../config.h"
#include "../backends/factory.hpp"

#ifdef ORACLE_FOUND
#   include "../backends/oracle.hpp"
#endif
 
#define BOOST_TEST_MODULE testCommunicator

#ifdef ORACLE_FOUND

BOOST_AUTO_TEST_CASE(test_oracle_backend_construct) {

    BOOST_REQUIRE_NO_THROW(communicator::backend::oracle_t("login", "password", "10.10.3.23:1234/v3", false));

    communicator::backend::oracle_t oracle_backend("login", "password", "10.10.3.23:1234/v3", false);

    BOOST_CHECK_EQUAL(oracle_backend.get_login(), "login");
    BOOST_CHECK_EQUAL(oracle_backend.get_password(), "password");
    BOOST_CHECK_EQUAL(oracle_backend.get_server(), "10.10.3.23");
    BOOST_CHECK_EQUAL(oracle_backend.get_port(), "1234");
    BOOST_CHECK_EQUAL(oracle_backend.get_servicename(), "v3");
}

BOOST_AUTO_TEST_CASE(test_oracle_backend_default_port) {

    BOOST_REQUIRE_NO_THROW(communicator::backend::oracle_t("login", "password", "10.10.3.23:1234/v3", false));

    communicator::backend::oracle_t oracle_backend("login", "password", "10.10.3.23/v3", false);

    BOOST_CHECK_EQUAL(oracle_backend.get_login(), "login");
    BOOST_CHECK_EQUAL(oracle_backend.get_password(), "password");
    BOOST_CHECK_EQUAL(oracle_backend.get_server(), "10.10.3.23");
    BOOST_CHECK_EQUAL(oracle_backend.get_port(), "1521");
    BOOST_CHECK_EQUAL(oracle_backend.get_servicename(), "v3");
}

BOOST_AUTO_TEST_CASE(test_oracle_backend_invalid_params) {

    BOOST_CHECK_THROW(communicator::backend::oracle_t oracle_backend("login", "password", "huypizdadjigurda", false), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(test_oracle_backend_registration) {

    // BOOST_REQUIRE_NOTHROW(communicator::backend::factory::create("oracle", "login", "password", "huypizdadjigurda"));
}

#endif
