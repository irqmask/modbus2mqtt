#include "autoquery.h"

#include <chrono>
#include <iostream>

#include "modbus.h"
#include "mqtt.h"

AutoQuery::AutoQuery(const AutoQuery::ItemDefinitions &rds, std::shared_ptr<Modbus> mb, std::shared_ptr<MQTT> mqtt)
    : reg_defs(rds)
    , modbus(mb)
    , mqtt(mqtt)
{
    entries.clear();
}


void AutoQuery::loadQueryTable(const std::vector<std::shared_ptr<ItemQuery>> &table)
{
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch());
    entries.clear();
    for (auto item : table) {
        // only load item for which a register definition exists
        if (reg_defs.find(item->name) == reg_defs.end()) {
            std::cerr << "Register " << item->name << " unknown!" << std::endl;
            continue;
        }
        // if topic is empty use name as topic
        if (item->topic.length() == 0) {
            item->topic = item->name;
        }
        // query item the first time immediately
        item->last_query = now - item->interval;
        std::cerr << "set initial last query to " << item->last_query.count() << std::endl;
        entries.push_back(item);
    }
    std::cerr << "entries size = " << entries.size() << std::endl;
}


void AutoQuery::loopOnce()
{
    float val = 0;
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch());

    for (auto item : entries) {
        auto &rd = reg_defs[item->name];
        try {
            if ((now - item->last_query) > item->interval) {
                item->last_query += item->interval;
                switch (rd.type) {
                case AutoQuery::ValType::VAL_TYPE_U16:
                    val = modbus->readRegisterU16(rd.reg_number);
                    break;
                case AutoQuery::ValType::VAL_TYPE_I16:
                    val = modbus->readRegisterI16(rd.reg_number);
                    break;
                case AutoQuery::ValType::VAL_TYPE_U32:
                    val = modbus->readRegisterU32(rd.reg_number);
                    break;
                case AutoQuery::ValType::VAL_TYPE_I32:
                    val = modbus->readRegisterI32(rd.reg_number);
                    break;
                default:
                    break;
                }
                val /= rd.scaling_factor;
    
                std::cerr << now.count() << ": Item " << item->name << " value " << val << " posted under topic " << item->topic << std::endl;
                mqtt->publish(item->topic, std::to_string(val));
            }
        }
        catch (std::exception &e) {
            std::cerr << now.count() << ": Error reading item " << item->name << " register(s) " << rd.reg_number << std::endl;
        }
    }
}
