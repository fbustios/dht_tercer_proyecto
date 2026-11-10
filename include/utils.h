#ifndef DHT_TERCER_PROYECTO_UTILS_H
#define DHT_TERCER_PROYECTO_UTILS_H
#include <string>
#include <cstdint>

struct NodeRef{
    uint32_t id;
    std::string ip;
    std::string port;
};
__uint32_t hash(std::string value, u_int32_t mask);

#endif //DHT_TERCER_PROYECTO_UTILS_H
