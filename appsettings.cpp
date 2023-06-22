#include "appsettings.h"

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <chrono>
#include <climits>
#include <stdexcept>
#include <iostream>
#include <string>
#include <sstream>

#include "autoquery.h"

using namespace libconfig;

AppSettings::AppSettings(std::string pathAndFilename)
    : modbusBaudrate(9600)
    , modbusParity('N')
    , modbusDataBits(8)
    , modbusStopBits(1)
    , modbusAddress(10)
    , mqttServerAddress("127.0.0.1")
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
    
    // parse config file's Modbus settings
    try {
        const Setting &root = cfg.getRoot();
        Setting &cfgMqtt = root["modbus"];
        int tempDataBits, tempStopBits, tempModbusAddress;
        std::string tempParity;

        if (!(cfgMqtt.lookupValue("device", modbusDeviceName) &&
              cfgMqtt.lookupValue("baudrate", modbusBaudrate) && 
              cfgMqtt.lookupValue("databits", tempDataBits) &&
              cfgMqtt.lookupValue("parity", tempParity) &&
              cfgMqtt.lookupValue("stopbits", tempStopBits) &&
              cfgMqtt.lookupValue("deviceaddress", tempModbusAddress))) {
            throw std::runtime_error("Either node 'modbus' or contained properties 'device', 'baudrate', 'databits', 'parity', 'stopbits', 'device_address' are missing!");
        }

        if (tempDataBits < 7 || tempDataBits > 10) {
            std::stringstream errmsg;
            errmsg << "Invalid value " << tempDataBits << " for databits!";
            throw std::runtime_error(errmsg.str());
        }      
        if (tempParity.length() != 1 || (tempParity[0] != 'N' && tempParity[0] != 'E' && tempParity[0] != 'O')) {
            std::stringstream errmsg;
            errmsg << "Invalid value " << tempParity << " for parity!";
            throw std::runtime_error(errmsg.str());
        }
        if (tempStopBits < 0 || tempStopBits > 2) {
            std::stringstream errmsg;
            errmsg << "Invalid value " << tempStopBits << " for stopbits!";
            throw std::runtime_error(errmsg.str());
        }
        modbusDataBits = tempDataBits;
        modbusParity = tempParity[0];
        modbusStopBits = tempStopBits;
        modbusAddress = tempModbusAddress;
    } 
    catch (const SettingNotFoundException &ex) {
        std::stringstream errmsg;
        errmsg << "Error reading config node 'modbus'! " << ex.what(); 
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
        std::stringstream errmsg;
        errmsg << "Error reading config node 'mqtt'! " << ex.what(); 
        throw std::runtime_error(errmsg.str());
    }

    // parse config file's notifications settings
    try {
        const Setting &root = cfg.getRoot();
        Setting &cfgRegister = root["register"];
        int32_t count = cfgRegister.getLength();
        std::cout << count << " register found in config file" << std::endl;
        for (int32_t i=0; i<count; i++) {
            std::shared_ptr<AutoQuery::ItemQuery> q = std::make_shared<AutoQuery::ItemQuery>();
            Setting &cfgreg = cfgRegister[i];
            int interval;
            if (!(cfgreg.lookupValue("name", q->name)
                && cfgreg.lookupValue("interval", interval))) {
                continue;
            }
            q->interval = std::chrono::seconds(interval);
            cfgreg.lookupValue("topic", q->topic); // optional
            itemQueries.push_back(q);
        }
    }
    catch (const SettingNotFoundException &ex) {
        std::stringstream errmsg;
        errmsg << "Error reading config node 'register'! " << ex.what(); 
        throw std::runtime_error(errmsg.str());
    }
}


const std::string &AppSettings::getModbusDeviceName() const
{
    return modbusDeviceName;
}


uint32_t AppSettings::getModbusBaudrate() const
{
    return modbusBaudrate;
}


uint8_t AppSettings::getModbusDataBits() const
{
    return modbusDataBits;
}


char AppSettings::getModbusParity() const
{
    return modbusParity;
}


uint8_t AppSettings::getModbusStopBits() const
{
    return modbusStopBits;
}


uint16_t AppSettings::getModbusAddress() const
{
    return modbusAddress;
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


const std::vector<std::shared_ptr<AutoQuery::ItemQuery>> &AppSettings::getQueryTable() const
{
    return itemQueries;
}


std::string AppSettings::getDefaultConfigFile()
{
    std::stringstream s;

    s << "# Example application configuration file" << std::endl;
    s << "modbus:" << std::endl;
    s << "{" << std::endl;
    s << "    device = \"/dev/ttyUSB0\";" << std::endl;
    s << "    baudrate = 9600;" << std::endl;
    s << "    databits = 8;" << std::endl;
    s << "    parity = \"N\";" << std::endl;
    s << "    stopbits = 1;" << std::endl;
    s << "    topic = \"sun2000\";" << std::endl; 
    s << "}" << std::endl;
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
