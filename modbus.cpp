#include "modbus.h"

#include <iostream>
#include <sstream>

#include <modbus/modbus.h>

Modbus::Modbus(std::string device, uint32_t baudrate, uint8_t data_bits, char parity, uint8_t stop_bits, uint16_t new_device_number)
    : mb(nullptr)
    , device_number(0)
{
    std::cerr << "Modbus open connection to " << device << " " << baudrate << "bd " << (int)data_bits << parity << (int)stop_bits << " device " << new_device_number << std::endl;
    mb = modbus_new_rtu(device.c_str(),
                        baudrate,
                        parity,
                        data_bits,
                        stop_bits);
    if (mb == nullptr) {
        std::stringstream errstr;
        errstr << "Unable to create Modbus RTU context! " << modbus_strerror(errno);
        throw std::runtime_error(errstr.str());
    }
    
    if (modbus_connect(mb) == -1) {
        std::stringstream errstr;
        errstr << "Modbus RTU connection failed! " << modbus_strerror(errno);
        throw std::runtime_error(errstr.str());
    }

    if (modbus_set_slave(mb, new_device_number) == -1) {
        std::stringstream errstr;
        errstr << "Modbus cannot set device number! " << modbus_strerror(errno);
        throw std::runtime_error(errstr.str());
    }
    device_number = new_device_number;
    std::cerr << "Modbus connection established" << std::endl;
}


Modbus::~Modbus()
{
    if (mb != nullptr) {
        modbus_close(mb);
        modbus_free(mb);
    }
}


void Modbus::setDeviceNumber(uint16_t new_device_number)
{
    if (modbus_set_slave(mb, new_device_number) == -1) {
        std::stringstream errstr;
        errstr << "Modbus cannot set device number! " << modbus_strerror(errno);
        throw std::runtime_error(errstr.str());
    }

    device_number = new_device_number;
}


uint16_t Modbus::deviceNumber()
{
    return device_number;
}


int16_t Modbus::readRegisterI16(int32_t reg_no)
{
    int16_t value = 0;
    if (modbus_read_registers(mb, reg_no, 1, (uint16_t*)&value) == -1) {
        std::stringstream errstr;
        errstr << "Modbus cannot read register " << reg_no << "! " << modbus_strerror(errno);
        throw std::runtime_error(errstr.str());
    }
    return value;
}


uint16_t Modbus::readRegisterU16(int32_t reg_no)
{
    uint16_t value = 0;
    if (modbus_read_registers(mb, reg_no, 1, &value) == -1) {
        std::stringstream errstr;
        errstr << "Modbus cannot read register " << reg_no << "! " << modbus_strerror(errno);
        throw std::runtime_error(errstr.str());
    }
    return value;
}


int32_t Modbus::readRegisterI32(int32_t reg_no)
{
    uint16_t reg_vals[2];
    uint32_t temp;
    if (modbus_read_registers(mb, reg_no, 2, reg_vals) == -1) {
        std::stringstream errstr;
        errstr << "Modbus cannot read registers " << reg_no << " - " << (reg_no + 1) << "! " << modbus_strerror(errno);
        throw std::runtime_error(errstr.str());
    }

    temp = (uint32_t)reg_vals[0] << 16 | reg_vals[1];
    return *((int32_t*)&temp);
}


uint32_t Modbus::readRegisterU32(int32_t reg_no)
{
    uint16_t reg_vals[2];
    if (modbus_read_registers(mb, reg_no, 2, reg_vals) == -1) {
        std::stringstream errstr;
        errstr << "Modbus cannot read registers " << reg_no << " - " << (reg_no + 1) << "! " << modbus_strerror(errno);
        throw std::runtime_error(errstr.str());
    }

    return (uint32_t)reg_vals[0] << 16 | reg_vals[1];
}
