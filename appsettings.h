#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <libconfig.h++>

#include "autoquery.h"

class AppSettings {
public:
    AppSettings(std::string pathAndFilename = "");
    
    const std::string FILENAME = "modbus2mqtt.cfg";
    
    const std::string &getModbusDeviceName() const;
    uint32_t getModbusBaudrate() const;
    uint8_t getModbusDataBits() const;
    char getModbusParity() const;
    uint8_t getModbusStopBits() const;
    uint16_t getModbusAddress() const;

    const std::string &getMqttServerAddress() const;
    uint16_t getMqttServerPort() const;
    const std::string &getMqttTopic() const;

    const std::vector<std::shared_ptr<AutoQuery::ItemQuery>> &getQueryTable() const;    

private:
    std::string                 cfgPathAndFilename;

    std::string                 modbusDeviceName;
    uint32_t                    modbusBaudrate;
    uint8_t                     modbusDataBits;
    char                        modbusParity;
    uint8_t                     modbusStopBits;
    uint16_t                    modbusAddress;

    std::string                 mqttServerAddress;
    uint16_t                    mqttServerPort;
    std::string                 mqttTopic;

    std::vector<std::shared_ptr<AutoQuery::ItemQuery>> itemQueries;
    libconfig::Config           cfg;

    std::string getDefaultConfigFile();

};
