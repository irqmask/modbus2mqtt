#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <thread>

#include <modbus/modbus.h>

#include "appsettings.h"
#include "autoquery.h"
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


static void print_regs(const char* name, uint16_t* reg_values, uint16_t num_values)
{
    printf("%s: ", name);
    for (int i=0; i<num_values; i++)
    {
        printf("%04x ", reg_values[i]);
    }
    printf("\n");
}


static void testModbus()
{
    modbus_t *mb;
    uint16_t tab_reg[32];
    uint32_t temp;
    float value;

    mb = modbus_new_rtu(g_options.device,
                        g_options.baudrate,
                        g_options.parity,
                        g_options.data_bits,
                        g_options.stop_bits);
    modbus_connect(mb);

    modbus_set_slave(mb, g_options.device_number);

    modbus_register_t *regs = g_modbus_regs;
    while (regs->name != NULL) {
        value = 0;
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
}


AutoQuery::ItemDefinitions SUN2000Regs = { 
    {"strings",     {30071,     1,  AutoQuery::ValType::VAL_TYPE_U16,   1.0f,       ""}},
    {"State1",      {32001,     1,  AutoQuery::ValType::VAL_TYPE_U16,   1.0f,       ""}},
    {"State2",      {32002,     1,  AutoQuery::ValType::VAL_TYPE_U16,   1.0f,       ""}},
    {"State3",      {32003,     1,  AutoQuery::ValType::VAL_TYPE_U16,   1.0f,       ""}},
    {"Alarm1",      {32008,     1,  AutoQuery::ValType::VAL_TYPE_U16,   1.0f,       ""}},
    {"Alarm2",      {32009,     1,  AutoQuery::ValType::VAL_TYPE_U16,   1.0f,       ""}},
    {"Alarm3",      {32010,     1,  AutoQuery::ValType::VAL_TYPE_U16,   1.0f,       ""}},
    {"P_accum",     {32106,     2,  AutoQuery::ValType::VAL_TYPE_U32,   100.0f,     "kWh"}},
    {"P_daily",     {32106,     2,  AutoQuery::ValType::VAL_TYPE_U32,   100.0f,     "kWh"}},
    {"PV_P",        {32064,     1,  AutoQuery::ValType::VAL_TYPE_I32,   1.0f,       "W"}},
    {"U_A",         {32069,     1,  AutoQuery::ValType::VAL_TYPE_U16,   10.0f,      "V"}},
    {"U_B",         {32070,     1,  AutoQuery::ValType::VAL_TYPE_U16,   10.0f,      "V"}},
    {"U_C",         {32071,     1,  AutoQuery::ValType::VAL_TYPE_U16,   10.0f,      "V"}},
    {"I_A",         {32072,     2,  AutoQuery::ValType::VAL_TYPE_I32,   1000.0f,    "A"}},
    {"I_B",         {32074,     2,  AutoQuery::ValType::VAL_TYPE_I32,   1000.0f,    "A"}},
    {"I_C",         {32076,     2,  AutoQuery::ValType::VAL_TYPE_I32,   1000.0f,    "A"}},
    {"temperature", {32087,     1,  AutoQuery::ValType::VAL_TYPE_I16,   10.0f,      "°C"}},

    {"PV_U1",       {32016,     1,  AutoQuery::ValType::VAL_TYPE_I16,   10.0f,      "V"}},
    {"PV_I1",       {32017,     1,  AutoQuery::ValType::VAL_TYPE_I16,   100.0f,     "A"}},
    {"PV_U2",       {32018,     1,  AutoQuery::ValType::VAL_TYPE_I16,   10.0f,      "V"}},
    {"PV_I2",       {32019,     1,  AutoQuery::ValType::VAL_TYPE_I16,   100.0f,     "A"}},

    {"ESU_power",   {37001,     2,  AutoQuery::ValType::VAL_TYPE_I32,   1.0f,       "W"}},
    {"ESU_soc",     {37004,     1,  AutoQuery::ValType::VAL_TYPE_U16,   10.0f,      "%"}},

    {"M_P",         {37113,     2,  AutoQuery::ValType::VAL_TYPE_I32,   1.0f,       "W"}},
    {"M_PExp",      {37119,     2,  AutoQuery::ValType::VAL_TYPE_I32,   100.0f,     "kWh"}},
};


int main(int argc, char** argv)
{
    bool run = true;
    std::shared_ptr<AppSettings> settings;
    std::shared_ptr<MQTT> mqtt;
    std::shared_ptr<Modbus> modbus;
    std::shared_ptr<AutoQuery> autoQuery;

    // Initialize components
    try {
        settings = std::make_shared<AppSettings>("");
        mqtt = std::make_shared<MQTT>(settings->getMqttServerAddress(), settings->getMqttServerPort());
        mqtt->setMainTopic(settings->getMqttTopic());
        modbus = std::make_shared<Modbus>(settings->getModbusDeviceName(), 
                                               settings->getModbusBaudrate(),
                                               settings->getModbusDataBits(),
                                               settings->getModbusParity(),
                                               settings->getModbusStopBits(),
                                               settings->getModbusAddress());
        autoQuery = std::make_shared<AutoQuery>(SUN2000Regs, modbus, mqtt);
        autoQuery->loadQueryTable(settings->getQueryTable());
    }
    catch (std::exception &e) {
        std::cerr << "Error initializing components: " << std::endl << e.what() << std::endl;
        return -1;
    }

    while (run) {
        try {
            autoQuery->loopOnce();
            mqtt->loopOnce();
        }
        catch (std::exception &e) {
            std::cerr << "Error " << e.what() << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}
