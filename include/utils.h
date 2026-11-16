#ifndef DHT_TERCER_PROYECTO_UTILS_H
#define DHT_TERCER_PROYECTO_UTILS_H
#include <string>
#include <cstdint>
#include <openssl/evp.h>
#include "network.h"

enum class MessageType {JOIN, FIND_SUCC, NOTIFY, PING, PONG, TRANSFER };

struct Message{
    MessageType type;
    std::unordered_map<std::string, std::string> args;
};

struct NodeRef{
    uint32_t id;
    std::string ip;
    int port;
};
uint8_t hash_addr(std::string value);

std::string serialize_message(Message m);
Message deserialize_message(std::string value);

#endif //DHT_TERCER_PROYECTO_UTILS_H
