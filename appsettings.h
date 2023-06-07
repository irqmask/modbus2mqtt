#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <libconfig.h++>

class AppSettings {
public:
    AppSettings(std::string pathAndFilename = "");
    
    const std::string FILENAME = "sun2000mqtt.cfg";
    
    struct Register {
        std::string name;
        int32_t interval;
    };
   
    const std::string &getMqttServerAddress() const;
    uint16_t getMqttServerPort() const;
    const std::string &getMqttTopic() const;
    const std::vector<Register> &getRegister() const;    

private:
    std::string                 cfgPathAndFilename;
    std::string                 mqttServerAddress;
    uint16_t                    mqttServerPort;
    std::string                 mqttTopic;
    std::vector<Register>       modbusRegister;
    libconfig::Config           cfg;

    std::string getDefaultConfigFile();

};
