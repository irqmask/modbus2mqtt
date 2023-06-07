#pragma once

#include <string>

/// Open a connection to a MQTT broker and publish topics
class MQTT
{
public:
    MQTT(std::string host, uint16_t port);

    void setMainTopic(std::string topic);
    void publish(std::string subtopic, std::string value);

private:
    std::string mainTopic;

    std::string removeLeadingAndTrailingSlashes(const std::string &topic);
};
