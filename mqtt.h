#pragma once

#include <string>

/// Open a connection to a MQTT broker and publish topics
class MQTT
{
public:
    MQTT(std::string host, uint16_t port);
    ~MQTT();

    void setMainTopic(std::string topic);
    void publish(std::string subtopic, std::string value);
    void loopOnce();

private:
    std::string mqtt_host;
    uint16_t mqtt_port;
    std::string main_topic;
    std::string client_id;
    struct mosquitto *mosq;

    std::string removeLeadingAndTrailingSlashes(const std::string &topic);
};
