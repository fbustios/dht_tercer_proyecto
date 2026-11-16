#include "../include/network.h"
#include "../include/utils.h"
using namespace std;

int setup_socket(const string& ip, int port){
    const int s = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);
    if (bind(s, (sockaddr*)&addr, sizeof(addr)) < 0){
        perror("bind error");
        close(s);
        return -1;
    };
    if (listen(s, SOMAXCONN) < 0){
        perror("listen error");
        close(s);
        return -1;
    };
    return s;
}
int connect_to(NodeRef& node){
    const int s = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(node.ip.c_str());
    addr.sin_port = htons(node.port);
    if (connect(s, (sockaddr*)&addr, sizeof(addr)) < 0){
        perror("connect error");
        close(s);
        return -1;
    }
    return s;
}

void send_message(NodeRef& ref, Message& message){
    const int s = connect_to(ref);
    string ser_message = serialize_message(message);
    if (send(s, ser_message.c_str(), ser_message.size(), 0) < 0){
        perror("send error");
        close(s);
    };
}
void handle_connections(int server){
    sockaddr_in addr;


}


