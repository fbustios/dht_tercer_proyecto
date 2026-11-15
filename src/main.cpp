#include <bits/stdc++.h>
#include "../include/network.h"
#include <thread>
#include <functional>
#include "../include/utils.h"
#include "../include/chordNode.h"
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

static void read_commands(const string& ip, int port){
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
                int sock = setup_socket(ip, port);
                uint32_t h = 1;
                node = Node(h, sock, ip, port);
                handler_thread = thread(handle_connections, ref(node));

            } else cout << "bad usage" << endl;

            break;
        case Cmd::STATUS:
            if (!joined){}
            break;
        case Cmd::FINGERS:
            if (!joined){}
            break;
        case Cmd::LEAVE:
            if (joined){
                handler_thread.join();
            }
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
    int port = atoi(argv[2]);
    string ip = argv[1];
    read_commands(ip, port);
    return 0;
}

