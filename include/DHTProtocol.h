#ifndef DHT_TERCER_PROYECTO_NODE_H
#define DHT_TERCER_PROYECTO_NODE_H
#include <cstdint>
#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <iostream>
#include <thread>
#include <chrono>
#include "utils.h"
#pragma once


class Node {
    public:
        Node();
        Node(const uint8_t& id, const int& sock, const std::string& ip, const int& port);
        void join(const std::string& ip, int port);
        void find_successor(uint8_t id);
        void print_status();
        int get_socket();
        std::vector<NodeRef> get_table();
        void handle_message(const Message& message);
        void stabilize();
        void fix_fingers();
        void create_fingers();
        void notify(const NodeRef& node);
        void maintenance();
        void leave();
        bool is_running();
    private:
        int socket;
        int port;
        std::string ip;
        uint8_t id;
        NodeRef predecessor;
        std::vector<NodeRef> fingers;
        bool joined = false;
        bool joining = false;
        bool has_predecessor = false;
        bool running = true;
        int next_finger = 1;
        // para mantener válida la asignación `node = Node(...)` de main.cpp.
        std::shared_ptr<std::mutex> state_mutex = std::make_shared<std::mutex>();

        NodeRef self_ref();
        static bool in_interval(uint8_t value, uint8_t start, uint8_t end, bool include_end = false);
        NodeRef closest_preceding_finger(uint8_t target);
        void handle_fsuccessor(const Message& message);
        void handle_found_successor(const Message& message);
        void handle_pred_reply(const Message& message);
        void handle_set_succ(const Message& message);
        void handle_set_pred(const Message& message);
        // Solo el hilo de mantenimiento inicia comprobaciones PING.
        std::shared_ptr<std::condition_variable> ping_cv =
            std::make_shared<std::condition_variable>();
        uint64_t ping_sequence = 0;
        std::string pending_ping;
        NodeRef pending_peer{};
        bool pong_received = false;
        bool probe(NodeRef peer);
        void recover_neighbors();
        void invalidate_peer(const NodeRef& peer);
        void stop();
};

#endif //DHT_TERCER_PROYECTO_NODE_H
