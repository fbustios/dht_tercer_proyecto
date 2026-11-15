#ifndef DHT_TERCER_PROYECTO_UTILS_H
#define DHT_TERCER_PROYECTO_UTILS_H
#include <string>
#include <cstdint>

#include "network.h"


enum class MessageType {JOIN, FIND_SUCC, NOTIFY, PING, PONG, TRANSFER };
struct Message{
    MessageType type;
    std::unordered_map<std::string, std::string> args;
};

__uint32_t hash(std::string value, u_int32_t mask);

std::string serialize_message(Message m);
Message deserialize_message(std::string value);

#endif //DHT_TERCER_PROYECTO_UTILS_H
