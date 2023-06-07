#pragma once

#include <cstdint>

class Modbus
{
public:
    Modbus(std::string device, uint32_t baudrate, uint8_t parity, uint8_t data_bits, uint8_t stop_bits);
    ~Modbus();

private:
    modbus_t *mb;
};
