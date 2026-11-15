#ifndef DHT_TERCER_PROYECTO_NODE_H
#define DHT_TERCER_PROYECTO_NODE_H
#include <cstdint>
#include <vector>
#include "utils.h"
#pragma once


class Node {
    public:
        Node();
        Node(const uint32_t& id, const int& sock, const std::string& ip, const int& port){
            this->socket = sock;
            this->id = id;
            this->ip = ip;
            this->port = port;

        }
        void create_fingers();
        void find_successor();
        void notify();
        void transfer();
    private:
        int socket;
        int port;
        std::string ip;
        uint32_t id;
        uint32_t mask = 0xFFFFFFFF;
        uint32_t predecessor;
        std::vector<NodeRef> fingers;
};

#endif //DHT_TERCER_PROYECTO_NODE_H
