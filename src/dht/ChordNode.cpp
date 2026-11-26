#include "DHTProtocol.h"
#include "../../include/network.h"
#define TABLE_SIZE 8  //identificador entero de 8 bits

Node::Node() {}
Node::Node(const uint8_t& id, const int& sock, const std::string& ip, const int& port){
    this->id = id;
    this->socket = sock;
    this->ip = ip;
    this->port = port;
    this->fingers = std::vector<NodeRef>(TABLE_SIZE);
}

Message build_fsuccessor(uint32_t id, const std::string& ip, int port){
    Message message{};
    message.type = MessageType::FIND_SUCC;
    std::unordered_map<std::string,std::string> args{};
    args.insert({});
    args.insert({});
    args.insert({});
    return message;
}

void Node::join(const std::string& ip, int port){
    int target = connect_to(ip, port);
    if (target == -1){ //no hay ring aun
        const NodeRef self{.id = this->id, .ip = this->ip, .port = this->port};
        this->predecessor = self;
        std::fill(fingers.begin(), fingers.end(), self);
    } else{
        Message message = build_fsuccessor(this->id, ip, port);
        std::string result = message_to_str(message);
        //send_message(NodeRef{0, ip, port}, result);
    }
};


void Node::print_status() {}

void Node::find_successor(uint8_t id){}

std::vector<NodeRef> Node::get_table() {
    return;
}

void Node::transfer(){}

int Node::get_socket(){
    return this->socket;
}

void notify(){}

void create_fingers() {}

