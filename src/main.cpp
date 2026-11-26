#include <bits/stdc++.h>
#include <thread>
#include <functional>
#include <chrono>
#include "../include/network.h"
#include "../include/utils.h"
#include "../include/DHTProtocol.h"
using namespace std;


enum class Cmd{JOIN,  STATUS, FINGERS, LEAVE, INVALID, FIND};
static Node node;

static Cmd parse(const string& command){
    static unordered_map<string, Cmd> commands = {
        {"join", Cmd::JOIN},
        {"status", Cmd::STATUS},
        {"fingers", Cmd::FINGERS},
        {"leave", Cmd::LEAVE},
        {"find", Cmd::FIND},
    };
    const auto it = commands.find(command);
    return it != commands.end() ? it->second : Cmd::INVALID;
}
static vector<string> read_tokens(const string& s){
    vector<string> tokens; string token;
    istringstream ss(s);
    while (ss >> token){
        tokens.push_back(token);
    }
    return tokens;
}

static void initialize_node(string ip, int port){
    int sock = setup_socket(ip, port);
    uint8_t h = hash_addr(ip + ":" + to_string(port));
    node = Node(h, sock, ip, port);
}

static void handle_commands(const string& ip, int port){
    string command;
    thread handler_thread;
    thread maintenance_thread;
    bool joined = false;
    while (getline(cin,command)){
        vector<string> tokens = read_tokens(command);
        if (tokens.empty()) continue;
        switch (parse(tokens[0])){
        case Cmd::JOIN:
            if (tokens.size() == 3 && !joined){
                joined = true;
                initialize_node(ip, port);
                if (node.get_socket() < 0) {
                    cout << "No se pudo crear el socket" << endl;
                    joined = false;
                    break;
                }
                handler_thread = thread(handle_connections, ref(node));
                maintenance_thread = thread([] {
                    while (node.is_running()) {
                        this_thread::sleep_for(chrono::milliseconds(150));
                        if (!node.is_running()) break;
                        node.stabilize();
                        node.fix_fingers();
                    }
                });
                string& target_ip = tokens[1]; int target_port = stoi(tokens[2]);
                node.join(target_ip, target_port);

            } else cout << "bad usage" << endl;

            break;
        case Cmd::STATUS:
            if (!joined){
                cout << "not joined yet" << endl;
                break;
            }
            node.print_status();
            break;
        case Cmd::FINGERS:
            if (!joined){
                cout << "not joined yet" << endl;
                break;
            }
            for (auto entry : node.get_table()){
                cout << noderef_to_str(entry) << endl;
            }
            break;
        case Cmd::FIND:
            if (joined && tokens.size() == 2) {
                const int search_id = stoi(tokens[1]);
                if (search_id >= 0 && search_id <= 255) {
                    node.find_successor(static_cast<uint8_t>(search_id));
                } else cout << "ID debe estar entre 0 y 255" << endl;
            } else cout << "bad usage" << endl;
            break;
        case Cmd::LEAVE:
            if (!joined){
                cout << "not joined yet" << endl;
                break;
            }
            node.transfer();
            node.stop();
            if (handler_thread.joinable()) handler_thread.join();
            if (maintenance_thread.joinable()) maintenance_thread.join();
            return;
            break;
        case Cmd::INVALID:
            cout << "invalid commmand" << endl;
            break;
        }
    }
    if (joined) {
        node.stop();
        if (handler_thread.joinable()) handler_thread.join();
        if (maintenance_thread.joinable()) maintenance_thread.join();
    }
}



int main(const int argc, const char* argv[]){
    if (argc != 3){
        cerr << "usage: ./main ip port" << endl;
        return 1;
    }
    handle_commands(argv[1], atoi(argv[2]));
    return 0;
}

