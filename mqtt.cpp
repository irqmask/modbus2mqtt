#include "mqtt.h"

#include <mosquitto.h>

MQTT::MQTT(std::string host, uint16_t port)
    : mainTopic("")
{

}

void MQTT::setMainTopic(std::string topic)
{
    mainTopic = removeLeadingAndTrailingSlashes(topic);
}

void MQTT::publish(std::string subTopic, std::string value)
{
    std::string topic = mainTopic + "/" + subTopic;

}

std::string MQTT::removeLeadingAndTrailingSlashes(const std::string &topic)
{
    const auto strBegin = topic.find_first_not_of("/");
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = topic.find_last_not_of("/");
    const auto strRange = strEnd - strBegin + 1;

    return topic.substr(strBegin, strRange);
}
