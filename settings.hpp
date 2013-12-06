/**
 *
 *
 **/
#ifndef SETTINGS_HPP
#define SETTINGS_HPP


namespace libconfig {
    class Setting;
}

/**
 *
 *
 **/
namespace communicator {

    struct storage_options_t {
        std::string kind;

        struct {
            std::string ip_addr;
            unsigned tcp_port;
        } connection;

        std::string db_name;
        std::string login;
        std::string password;
    };

    struct options_t {

        std::string name;
    	std::string protocol;
    	unsigned workers;

    	struct {
    		std::string ip_addr;
    		unsigned tcp_port;
    	} listen;

    	libconfig::Setting * settings;

    	storage_options_t storage;
    };
}

void get_settings(int argc, const char * argv[], std::vector<communicator::options_t> & options, bool & daemon_mode);

#endif // SETTINGS_HPP
