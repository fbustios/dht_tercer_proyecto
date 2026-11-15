#ifndef DHT_TERCER_PROYECTO_NETWORK_H
#define DHT_TERCER_PROYECTO_NETWORK_H
#include <string>
#include <unordered_map>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
struct NodeRef{
    uint32_t id;
    std::string ip;
    int port;
};
int setup_socket(const std::string& ip, const int port);
void handle_connections(const int sock);
void send_message(int socket, std::string message);
void close_socket(int socket);
#endif //DHT_TERCER_PROYECTO_NETWORK_H
