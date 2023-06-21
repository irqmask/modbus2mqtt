#include "mqtt.h"

#include <cstring>
#include <iostream>
#include <stdexcept>
#include <sstream>

#include <mosquitto.h>
#include <unistd.h>

MQTT::MQTT(std::string host, uint16_t port)
    : mqtt_host(host)
    , mqtt_port(port)
    , main_topic("")
    , mosq(nullptr)
{
    int32_t rc;
    std::cerr << "MQTT connecting to " << mqtt_host << ":" << mqtt_port << std::endl;

    std::stringstream temp;
    temp << "mqttnotify_" << getpid();
    client_id = temp.str();
    mosq = mosquitto_new(client_id.c_str(), true, this);
    if (mosq == nullptr) {
        std::stringstream errmsg;
        errmsg << "Unable to create new mosquitto client!";
        throw std::runtime_error(errmsg.str());
    }

    rc = mosquitto_connect(mosq, mqtt_host.c_str(), mqtt_port, 60);
    if (rc != 0) {
        std::stringstream errmsg;
        errmsg << "Unable to connect to " << mqtt_host << ":" << mqtt_port << "! rc = " << rc;
        throw std::runtime_error(errmsg.str());
    }
    std::cerr << "MQTT connection established to " << mqtt_host << ":" << mqtt_port << std::endl;
}


MQTT::~MQTT()
{
    if (mosq != nullptr) {
        mosquitto_disconnect(mosq);
        mosquitto_destroy(mosq);
    }
}


void MQTT::setMainTopic(std::string topic)
{
    main_topic = removeLeadingAndTrailingSlashes(topic);
}


void MQTT::publish(std::string subTopic, std::string value)
{
    int32_t rc; 
    std::string topic = main_topic + "/" + subTopic;
    std::cerr << "MQTT dummy " << topic << " = " << value << std::endl;

    rc = mosquitto_publish(mosq, NULL, topic.c_str(), value.size(), value.c_str(), 0, false);
    if (rc != 0) {
        std::stringstream errmsg;
        errmsg << "Unable to publish topic " << topic << "; value " << value << "; rc = " << rc;
        throw std::runtime_error(errmsg.str());
    }

}


void MQTT::loopOnce()
{
    int32_t rc;
    rc = mosquitto_loop(mosq, -1, 1);
    if (rc != 0) {
        std::cerr << "MQTT connection error! retry in 10s" << std::endl;
        sleep(10);
        mosquitto_reconnect(mosq);
    }
}


std::string MQTT::removeLeadingAndTrailingSlashes(const std::string &topic)
{
    const auto str_begin = topic.find_first_not_of("/");
    if (str_begin == std::string::npos)
        return ""; // no content

    const auto str_end = topic.find_last_not_of("/");
    const auto str_range = str_end - str_begin + 1;

    return topic.substr(str_begin, str_range);
}
