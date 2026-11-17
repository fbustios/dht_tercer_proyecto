#include <bits/stdc++.h>
#include "../include/network.h"
#include <thread>
#include <functional>
#include "../include/utils.h"
#include "../include/DHTProtocol.h"
using namespace std;


enum class Cmd{JOIN,  STATUS, FINGERS, LEAVE, INVALID};
static Node node;

static Cmd parse(const string& command){
    static unordered_map<string, Cmd> commands = {
        {"join", Cmd::JOIN},
        {"status", Cmd::STATUS},
        {"fingers", Cmd::FINGERS},
        {"leave", Cmd::LEAVE},
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
    bool joined = false;
    while (getline(cin,command)){
        vector<string> tokens = read_tokens(command);
        if (tokens.empty()) continue;
        switch (parse(tokens[0])){
        case Cmd::JOIN:
            if (tokens.size() == 3 && !joined){
                joined = true;
                initialize_node(ip, port);
                handler_thread = thread(handle_connections, ref(node));
                string& target_ip = tokens[1]; int target_port = stoi(tokens[2]);
                if (connect_to(target_ip, target_port) == -1) {  //no hubo respuesta, iniciar el anillo
                    cout << "Target not available, starting new ring..." << endl;
                } else {  //anillo existente
                }

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
            for (auto finger : node.get_fingers()){
                cout << noderef_to_str(finger) << endl;
            }
            break;
        case Cmd::LEAVE:
            if (!joined){
                cout << "not joined yet" << endl;
                break;
            }
            node.transfer();
            handler_thread.join();
            break;
        case Cmd::INVALID:
            cout << "invalid commmand" << endl;
            break;
        }
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

