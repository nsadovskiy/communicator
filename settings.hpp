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

    struct options_t {

        std::string name;
    	std::string protocol;
    	unsigned workers;

    	struct {
    		std::string id_addr;
    		unsigned tcp_port;
    	} listen;

    	libconfig::Setting * settings;

    	struct {

    		std::string kind;

    		struct {
    			std::string ip_addr;
    			unsigned tcp_port;
    		} connection;

    		std::string db_name;
    		std::string login;
    		std::string password;

    	} storage;
    };
}

void get_settings(int argc, const char * argv[], std::vector<communicator::options_t> & options, bool & daemon_mode);

#endif // SETTINGS_HPP
