#ifndef DHT_TERCER_PROYECTO_NETWORK_H
#define DHT_TERCER_PROYECTO_NETWORK_H
#include <string>
#include <unordered_map>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../include/chordNode.h"

int setup_socket(const std::string& ip, int port);
void handle_connections(Node& node);
int connect_to(NodeRef& node);
void send_message(NodeRef& node, Message& message);
void close_socket(int socket);
#endif //DHT_TERCER_PROYECTO_NETWORK_H
