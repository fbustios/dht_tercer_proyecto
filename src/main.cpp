#include <bits/stdc++.h>
#include "../include/network.h"
#include <thread>
#include "../include/utils.h"
#include "../include/chordNode.h"
using namespace std;


enum class Cmd{JOIN,  STATUS, FINGERS, LEAVE, INVALID};
int sock;
Cmd parse(const string& command){
    static unordered_map<string, Cmd> commands = {
        {"join", Cmd::JOIN},
        {"status", Cmd::STATUS},
        {"fingers", Cmd::FINGERS},
        {"leave", Cmd::LEAVE},
    };
    const auto it = commands.find(command);
    return it != commands.end() ? it->second : Cmd::INVALID;
}
vector<string> read_tokens(const string& s){
    vector<string> tokens; string token;
    istringstream ss(s);
    while (ss >> token){
        tokens.push_back(token);
    }
    return tokens;
}

void read_commands(){
    string command;
    bool joined = false;
    while (getline(cin,command)){
        vector<string> tokens = read_tokens(command);
        switch (parse(tokens[0])){
        case Cmd::JOIN:
            if (tokens.size() == 3 && !joined){
                joined = true;

                thread handler_thread(handle_connections, sock);
            } else cout << "bad usage" << endl;

            break;
        case Cmd::STATUS:
            if (!joined){}
            break;
        case Cmd::FINGERS:
            if (!joined){}
            break;
        case Cmd::LEAVE:
            if (!joined){}
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
    sock = setup_socket(argv[1], atoi(argv[2]));
    read_commands();

    return 0;
}

