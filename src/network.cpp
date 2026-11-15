#include "../include/network.h"
#include "../include/utils.h"
using namespace std;

int setup_socket(const string& ip, int port){
    const int s = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr;
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

void handle_connections(int sock){
    sockaddr_in addr;


}


