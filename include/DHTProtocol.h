#ifndef DHT_TERCER_PROYECTO_NODE_H
#define DHT_TERCER_PROYECTO_NODE_H
#include <cstdint>
#include <vector>
#include "utils.h"
#pragma once


class Node {
    public:
        Node();
        Node(const uint8_t& id, const int& sock, const std::string& ip, const int& port);
        void join(const std::string& ip, int port);
        void find_successor(uint8_t id);
        void transfer();
        void print_status();
        int get_socket();
        std::vector<NodeRef> get_table();
    private:
        int socket;
        int port;
        std::string ip;
        uint8_t id;
        NodeRef predecessor;
        std::vector<NodeRef> fingers;
};

#endif //DHT_TERCER_PROYECTO_NODE_H
