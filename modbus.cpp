#include "modbus.h"

#include <sstream>

#include <modbus/modbus.h>

Modbus::Modbus(std::string device, uint32_t baudrate, uint8_t parity, uint8_t data_bits, uint8_t stop_bits)
    : mb(nullptr)
{
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
}


Modbus::~Modbus()
{
    if (mb != nullptr) {
        modbus_close(mb);
        modbus_free(mb);
    }
}
