#include "modbus.h"

#include <modbus/modbus.h>

    Modbus::Modbus(std::string device, uint32_t baudrate, uint8_t parity, uint8_t data_bits, uint8_t stop_bits)
        : mb(nullptr)
    {
        mb = modbus_new_rtu(device,
                            baudrate,
                            parity,
                            data_bits,
                            stop_bits);
        if (mb == nullptr) {
            // FIXME throw error
        }
    }

    Modbus::~Modbus()
    {
        if (mb != nullptr) {
            modbus_close(mb);
            modbus_free(mb);
        }
    }