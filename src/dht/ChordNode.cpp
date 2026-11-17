#include "DHTProtocol.h"

Node::Node() {}
Node::Node(const uint8_t& id, const int& sock, const std::string& ip, const int& port){
    this->id = id;
    this->socket = sock;
    this->ip = ip;
    this->port = port;
}

void create_fingers() {

}

void Node::print_status() {

}

void Node::find_successor(uint8_t id){

}

void Node::transfer(){}

int Node::get_socket(){
    return this->socket;
}


std::vector<NodeRef> Node::get_fingers() {
    return fingers;
}
void notify()
{

}

void join(const std::string& ip, int port);
