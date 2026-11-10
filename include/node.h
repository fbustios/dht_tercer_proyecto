#ifndef DHT_TERCER_PROYECTO_NODE_H
#define DHT_TERCER_PROYECTO_NODE_H
#include <cstdint>
#pragma once


class Node {
    public:
        Node();
    private:
        uint32_t id;
        uint32_t mask;
        uint32_t succesor;
        uint32_t predecessor;


};

#endif //DHT_TERCER_PROYECTO_NODE_H
