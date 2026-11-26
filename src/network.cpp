#include "../include/network.h"
#include "../include/utils.h"
#include <poll.h>
#include <cerrno>
using namespace std;

int setup_socket(const string& ip, int port){
    const int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) return -1;
    int reuse = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
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
    if (s < 0) return -1;
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);
    if (connect(s, (sockaddr*)&addr, sizeof(addr)) < 0){
        close(s);
        return -1;
    }
    return s;
}

void send_message(NodeRef& ref, Message& message){
    const int s = connect_to(ref.ip, ref.port);
    if (s < 0) {
        cerr << "No se pudo conectar con " << ref.ip << ":" << ref.port << endl;
        return;
    }
    string str_message = message_to_str(message);
    size_t sent = 0;
    while (sent < str_message.size()) {
        const ssize_t n = send(s, str_message.data() + sent,
                               str_message.size() - sent, MSG_NOSIGNAL);
        if (n <= 0) {
            perror("send error");
            break;
        }
        sent += static_cast<size_t>(n);
    }
    close(s);
}
void handle_connections(Node& node){
    while (node.is_running()){
        pollfd pfd{node.get_socket(), POLLIN, 0};
        if (poll(&pfd, 1, 200) <= 0) continue;
        if (!node.is_running()) break;
        sockaddr_in addr{};
        socklen_t addr_size = sizeof(addr);
        const int client = accept(node.get_socket(), (sockaddr*)&addr, &addr_size);
        if (client < 0) continue;
        timeval timeout{1, 0};
        setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        std::string raw;
        char buffer[1024];
        ssize_t n = 0;
        while ((n = recv(client, buffer, sizeof(buffer), 0)) > 0 && raw.size() < 8192) {
            raw.append(buffer, static_cast<size_t>(n));
        }
        close(client);
        if (!raw.empty()) {
            try {
                Message message = str_to_message(raw);
                node.handle_message(message);
            } catch (const std::exception& e) {
                cerr << "Mensaje invalido: " << e.what() << endl;
            }
        }
    }
}

void close_socket(int socket) {
    if (socket >= 0) close(socket);
}
