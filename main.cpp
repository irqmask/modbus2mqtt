#include <cstdint>
#include <iostream>
#include <memory>

#include <modbus/modbus.h>

#include "appsettings.h"
#include "modbus.h"
#include "mqtt.h"

typedef struct _options {
    char            device[256];
    int             baudrate;
    int             data_bits;
    char            parity;
    int             stop_bits;
    int             device_number; //!< Modbus device number of solar-inverter
} options_t;


typedef enum _value_type {
    VAL_TYPE_NONE,
    VAL_TYPE_U16,
    VAL_TYPE_I16,
    VAL_TYPE_U32,
    VAL_TYPE_I32,
    VAL_TYPE_STR
} value_type_t;


typedef struct _modbus_register {
    const char*     name;
    uint16_t        address;
    uint16_t        num_regs;
    float           scale;
    value_type_t    type;
    int32_t         interval;   // >0 in seconds, 0 once, <0 never
} modbus_register_t;


options_t g_options = {
    "/dev/ttyUSB0",
    9600,
    8,
    'N',
    1,
    5
};

modbus_register_t g_modbus_regs[] = {
    { "Model",          30000,  15,     1.0f,       VAL_TYPE_STR,   0 },
    { "strings",        30071,  1,      1.0f,       VAL_TYPE_U16,   0 },
    { "PV_P",           32064,  1,      1000.0f,    VAL_TYPE_I32,   60 },
    { "temperature",    32087,  1,      10.0f,      VAL_TYPE_I16,   60 },
    { "ESU_soc",        37004,  1,      10.0f,      VAL_TYPE_U16,   60 },
    { NULL,             0,      0,      0.0f,       VAL_TYPE_NONE, -1 }
};


void print_regs(const char* name, uint16_t* reg_values, uint16_t num_values)
{
    printf("%s: ", name);
    for (int i=0; i<num_values; i++)
    {
        printf("%04x ", reg_values[i]);
    }
    printf("\n");
}


int main(int argc, char** argv)
{
    bool run = true;
    std::shared_ptr<AppSettings> settings;
    std::shared_ptr<MQTT> mqtt;
    // FIXME implement std::shared_ptr<Modbus> modbus;
    // FIXME implement std::shared_ptr<AutoQuerySUN2000> autoQuery;

    // Initialize components
    try {
        auto settings = std::make_shared<AppSettings>("./");
        auto mqtt = std::make_shared<MQTT>(settings->getMqttServerAddress(), settings->getMqttServerPort());
        // FIXME implement auto modbus = std::make_shared<Modbus>(settings->getDeviceName(), settings->getModbusAddress());
        // FIXME implement auto autoQuery = std::make_shared<AutoQuerySUN2000>(modbus);
    }
    catch (std::exception &e) {
        std::cerr << "Error initializing components: " << std::endl << e.what() << std::endl;
        return -1;
    }
    modbus_t *mb;
    uint16_t tab_reg[32];
    uint32_t temp;
    float value;

    mb = modbus_new_rtu(g_options.device,
                        g_options.baudrate,
                        g_options.parity,
                        g_options.data_bits,
                        g_options.stop_bits);
    //mb = modbus_new_tcp("127.0.0.1", 1502);
    modbus_connect(mb);

    modbus_set_slave(mb, g_options.device_number);

    modbus_register_t *regs = g_modbus_regs;
    while (regs->name != NULL) {
        modbus_read_registers(mb, regs->address, regs->num_regs, tab_reg);
        switch (regs->type) {
        case VAL_TYPE_U16:
            value = tab_reg[0];
            break;
        case VAL_TYPE_I16:
            value = tab_reg[0];
            break;
        case VAL_TYPE_U32:
            temp = (uint32_t)tab_reg[0] << 16 | tab_reg[1];
            value = temp;
            break;
        case VAL_TYPE_I32:
            temp = (uint32_t)tab_reg[0] << 16 | tab_reg[1];
            value = (int32_t)temp;
            break;
        default:
            print_regs(regs->name, tab_reg, regs->num_regs);
            regs++;
            continue;
        }
        value /= regs->scale;
        printf("%s: %.1f\n", regs->name, value);
        regs++;
    }
    
    modbus_close(mb);
    modbus_free(mb);
    run = false; // FIXME delete this line
    while (run) {
        try {
            // FIXME implement autoQuery->runOnce();
            // FIXME implement mqtt->runOnce();
        }
        catch (std::exception &e) {
            std::cerr << "Error " << e.what() << std::endl;
        }
    }

    return 0;
}
