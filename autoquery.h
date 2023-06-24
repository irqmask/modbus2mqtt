#pragma once

#include <chrono>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

class Modbus;
class MQTT;

class AutoQuery {
public:

    enum class ValType {
        VAL_TYPE_NONE,
        VAL_TYPE_U16,
        VAL_TYPE_I16,
        VAL_TYPE_U32,
        VAL_TYPE_I32,
        VAL_TYPE_STR
    };

    class ItemDefinition {
    public:
        uint16_t reg_number;
        uint16_t reg_count;
        AutoQuery::ValType type;
        float scaling_factor;
        std::string unit;
    };
    
    class ItemQuery {
    public:
        ItemQuery()
            : name("")
            , interval(std::chrono::seconds(0))
            , topic("")
        {    
        }

        ItemQuery(std::string n, std::chrono::seconds i, std::string t) 
            : name(n)
            , interval(i)
            , topic(t)
        {
        }

        std::string name; ///< item name as defined in register definitions
        std::chrono::milliseconds interval; ///< interval in seconds between each query
        std::string topic; ///< optional topic, if empty item_name will be used
        std::chrono::milliseconds  last_query; ///< last query (milliseconds since epoch)
    };

    typedef std::map<std::string, ItemDefinition> ItemDefinitions;


    AutoQuery(const ItemDefinitions &rds, std::shared_ptr<Modbus> mb, std::shared_ptr<MQTT> mqtt);

    void loadQueryTable(const std::vector<std::shared_ptr<ItemQuery>> &table);
    void loopOnce();
    
private:
    ItemDefinitions reg_defs;
    std::vector<std::shared_ptr<ItemQuery>> entries;
    std::shared_ptr<Modbus> modbus;
    std::shared_ptr<MQTT> mqtt;
};
