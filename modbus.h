#pragma once

#include <cstdint>
#include <string>

#include <modbus/modbus.h>

class Modbus
{
public:
    Modbus(std::string device, uint32_t baudrate, uint8_t data_bits, char parity, uint8_t stop_bits, uint16_t new_device_number);
    ~Modbus();

    void setDeviceNumber(uint16_t new_device_number);
    uint16_t deviceNumber();

    int16_t readRegisterI16(int32_t reg_no);
    uint16_t readRegisterU16(int32_t reg_no);
    int32_t readRegisterI32(int32_t reg_no);
    uint32_t readRegisterU32(int32_t reg_no);

private:
    modbus_t *mb;
    uint16_t device_number;
};
