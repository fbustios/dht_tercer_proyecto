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
    cout << "Socket ready at " + ip + ":" <<port << endl;
    return s;
}
int connect_to(const std::string& ip, int port){
    const int s = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);
    if (connect(s, (sockaddr*)&addr, sizeof(addr)) < 0){
        perror("connect error");
        close(s);
        return -1;
    }
    cout << "Socket connected to " << ip << ":" << port << endl;
    return s;
}

void send_message(NodeRef& ref, Message& message){
    const int s = connect_to(ref.ip, ref.port);
    string str_message = message_to_str(message);
    if (send(s, str_message.c_str(), str_message.size(), 0) < 0){
        perror("send error");
        close(s);
    };
}
void handle_connections(Node& node){
    while (true){
        sockaddr_in addr{};
        socklen_t addr_size;
        if (accept(node.get_socket(), (sockaddr*)&addr, &addr_size) > 0){
            char ip_str[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &addr.sin_addr, ip_str, INET_ADDRSTRLEN);
            cout << "Connection accepted from " << ip_str << ":" << addr.sin_port  << endl;
        };
    }
}


