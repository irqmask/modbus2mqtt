#include "appsettings.h"

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <climits>
#include <stdexcept>
#include <iostream>
#include <string>
#include <sstream>


using namespace libconfig;

AppSettings::AppSettings(std::string pathAndFilename)
    : mqttServerAddress("127.0.0.1")
    , mqttServerPort(1883)
{
    const char* homedir;
    
    if (pathAndFilename.size() > 0) {
        cfgPathAndFilename = pathAndFilename + "/" + FILENAME;
    }
    else {
        if ((homedir = getenv("XDG_CONFIG_HOME")) == nullptr) {
            if ((homedir = getenv("HOME")) == nullptr) {
                homedir = getpwuid(getuid())->pw_dir;
                if (homedir == nullptr) {
                    throw std::runtime_error("unable to retrieve home directory");
                }
            }
        }
        // try to find config file in ~/.local/
        cfgPathAndFilename = homedir + std::string("/.local/") + FILENAME;
    }
    system("ls");
    
    try {
        cfg.readFile(cfgPathAndFilename.c_str());
    }
    catch (const FileIOException &ex) {
        std::stringstream errmsg;
        errmsg << "I/O error while reading file:" << cfgPathAndFilename << std::endl << "Create a file! Example content:" << std::endl;
        errmsg << getDefaultConfigFile() << std::endl;
        throw std::runtime_error(errmsg.str());
    }
    catch (const ParseException &ex) {
        std::stringstream errmsg;
        errmsg << "Parse error at " << ex.getFile() << ":" << ex.getLine() << " - " << ex.getError();
        throw std::runtime_error(errmsg.str());
    }
    
    // parse config file's MQTT settings
    try {
        const Setting &root = cfg.getRoot();
        Setting &cfgMqtt = root["mqtt"];
        std::string tempAddress, tempTopic;
        uint32_t port;
        if (!(cfgMqtt.lookupValue("address", tempAddress) && cfgMqtt.lookupValue("port", port) && cfgMqtt.lookupValue("topic", tempTopic))) {
            throw std::runtime_error("Either node 'mqtt' or contained properties 'address' or 'port' are missing!");
        }

        if (port > USHRT_MAX) {
            std::stringstream errmsg;
            errmsg << "Invalid value " << port << " for port number!";
            throw std::runtime_error(errmsg.str());
        }
        mqttServerAddress = tempAddress;
        mqttServerPort = static_cast<uint16_t>(port);
        mqttTopic = tempTopic;
    } 
    catch (const SettingNotFoundException &ex) {
        throw std::runtime_error("Either node 'mqtt' or contained properties 'address', 'port' or 'topic' are missing!");
    }
    // parse config file's notifications settings
    try {
        const Setting &root = cfg.getRoot();
        Setting &cfgRegister = root["register"];
        int32_t count = cfgRegister.getLength();
        std::cout << count << " register found in config file" << std::endl;
        for (int32_t i=0; i<count; i++) {
            AppSettings::Register r;
            Setting &cfgreg = cfgRegister[i];
            if (!(cfgreg.lookupValue("name", r.name)
                && cfgreg.lookupValue("interval", r.interval))) {
                continue;
            }
            modbusRegister.push_back(r);
        }
    }
    catch (const SettingNotFoundException &ex) {
        throw std::runtime_error("Either node 'register' or contained properties 'name' or 'interval' are missing!");
    }
}


const std::string &AppSettings::getMqttServerAddress() const
{
    return mqttServerAddress;
}


uint16_t AppSettings::getMqttServerPort() const
{
    return mqttServerPort;
}


const std::string &AppSettings::getMqttTopic() const
{
    return mqttTopic;
}


const std::vector<AppSettings::Register> &AppSettings::getRegister() const
{
    return modbusRegister;
}


std::string AppSettings::getDefaultConfigFile()
{
    std::stringstream s;

    s << "# Example application configuration file" << std::endl;
    s << "mqtt:" << std::endl;
    s << "{" << std::endl;
    s << "    address = \"127.0.0.1\";" << std::endl;
    s << "    port = 1883;" << std::endl;
    s << "    topic = sun2000";
    s << "}" << std::endl;
    s << "register = (" << std::endl;
    s << "{" << std::endl;
    s << "    name = \"hello\";" << std::endl;
    s << "    interval = 60;" << std::endl;
    s << "}" << std::endl;
    s << ")" << std::endl;
    return s.str();
}
