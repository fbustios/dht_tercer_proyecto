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

std:: string noderef_to_str(NodeRef &node);
uint8_t hash_addr(std::string value);
std::string message_to_str(Message& m);
Message str_to_message(std::string value);

#endif //DHT_TERCER_PROYECTO_UTILS_H
