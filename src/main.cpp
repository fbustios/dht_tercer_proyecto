#include <bits/stdc++.h>
using namespace std;


enum class Cmd{JOIN, PUT, GET, STATUS, FINGERS, LEAVE, INVALID};

Cmd parse(const string& command){
    static unordered_map<string, Cmd> commands = {
        {"join", Cmd::JOIN},
        {"put", Cmd::PUT},
        {"get", Cmd::GET},
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
            } else cout << "bad usage" << endl;

            break;
        case Cmd::PUT:
            if (tokens.size() == 3 && joined){}
            break;
        case Cmd::GET:
            if (tokens.size() == 2 && joined){}
            break;
        case Cmd::STATUS:
            break;
        case Cmd::FINGERS:
            break;
        case Cmd::LEAVE:
            break;
        case Cmd::INVALID:
            cout << "invalid commmand" << endl;
            break;
        }
    }
}


int main(){
    read_commands();
    return 0;
}

