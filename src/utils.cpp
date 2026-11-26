#include "../include/utils.h"

uint8_t hash_addr(std::string value){
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int len = 0;
    EVP_Digest(value.c_str(), value.size(), hash, &len, EVP_sha1(), nullptr);
    return hash[0]; //1 byte
}

std::string message_to_str(Message& m){ //a medias falta el messageType
    std::string result;
    for (const auto& e: m.args){
        result+= e.first + ":" + e.second + "\n";
    }

    return result;
}
Message str_to_message(std::string value){
    return Message{};
}

std::string noderef_to_str(NodeRef &node)
{
    return "d";
};