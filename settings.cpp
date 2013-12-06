/**
 *
 *
 **/
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <libconfig.h++>

#include "server.hpp"
#include "settings.hpp"


using std::string;
using std::exception;
using std::ostringstream;
using std::runtime_error;
using communicator::options_t;

using namespace libconfig;


/**
 *
 *
 **/
options_t parse_settings(Config & config, const string & root) {
    
    string value;
    options_t option = {
        string(),
        string(),
        0,
        { string(), 0 },
        nullptr,
        {
            string(),
            { string(), 0 },
            string(),
            string(),
            string()
        }
    };
    
    string opt_name;
    try {
        opt_name = root + ".protocol";
        string protocol = config.lookup(opt_name);

        opt_name = root + ".workers";
        unsigned workers = config.lookup(opt_name);

        opt_name = root + ".listen.address";
        string listen_addr = config.lookup(opt_name);

        opt_name = root + ".listen.port";
        unsigned listen_port = config.lookup(opt_name);

        Setting * setting = nullptr;
        try {
            setting = &config.getRoot()[root.c_str()]["settings1"];
        }
        catch (const SettingNotFoundException &) {
        }

        opt_name = root + ".storage.kind";
        string storage = config.lookup(opt_name);

        opt_name = root + ".storage.address";
        string storage_ip = config.lookup(opt_name);

        opt_name = root + ".storage.port";
        unsigned storage_port = config.lookup(opt_name);

        opt_name = root + ".storage.name";
        string storage_name = config.lookup(opt_name);

        opt_name = root + ".storage.username";
        string storage_login = config.lookup(opt_name);

        opt_name = root + ".storage.password";
        string storage_password = config.lookup(opt_name);

        option = {
            root,
            protocol,
            workers,
            { listen_addr, listen_port },
            setting,
            {
                storage,
                { storage_ip, storage_port },
                storage_name,
                storage_login,
                storage_password
            }
        };
    }
    catch (const SettingNotFoundException &) {
        ostringstream err;
        err << "Option '" << opt_name << "' not found!";
        throw runtime_error(err.str());
    }

    return option;
}

/**
 *
 *
 **/
void read_settings_from_file(const char * config_path, std::vector<options_t> & options) {

    Config config;
    std::vector<options_t> result;

    try {
        config.readFile(config_path);
        auto & communicators = config.getRoot();
        int count = communicators.getLength();
        for (int i = 0; i < count; ++i) {
            result.push_back(parse_settings(config, string(communicators[i].getName())));
        }
        options = result;
    }
    catch (const FileIOException & e) {
        ostringstream err;
        err << "Error opening cofiguration file '" << config_path << "'";
        throw runtime_error(err.str());
    }
    catch(const ParseException & e) {
        ostringstream err;
        err << "Parse error at " << e.getFile() << ":" << e.getLine() << " - " << e.getError();
        throw runtime_error(err.str());
    }
    catch (const exception & e) {
        throw runtime_error(e.what());
    }
    catch (...) {
        throw runtime_error("Unexpected exception while reading configuration file");
    }
}

/**
 *
 *
 **/
void get_settings(int argc, const char * argv[], std::vector<communicator::options_t> & options, bool & daemon_mode) {

    using std::vector;
    using std::find_if;

    // Check mode daemon or console
    vector<const char *> kw = {
        "daemon", "-daemon", "--daemon", "/daemon", "\\daemon",
        "service", "-service", "--service", "/service", "\\service"
    };

    if (argc > 1) {
        const string param = argv[1];
        auto pos = find_if(kw.begin(), kw.end(), [param](const char * val) { return val == param; });
        if (kw.end() == pos) {
            ostringstream err;
            err << "Unknown option: '" << param << "'. Valid values are 'daemon' or 'service'\n";
            throw runtime_error(err.str());
        }
        daemon_mode = true;
    }

    // Read options from config file
    read_settings_from_file("communicator.conf", options);
}

/**
 *
 *
 **/
