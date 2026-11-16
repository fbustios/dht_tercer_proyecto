#include "../include/utils.h"

uint8_t hash_addr(std::string value){
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int len = 0;
    EVP_Digest(value.c_str(), value.size(), hash, &len, EVP_sha1(), nullptr);
    return hash[0]; //1 byte
}

std::string message_to_str(Message& m){
    std::string serialized_message;
    for (const auto& e: m.args){
        serialized_message+= e.first + ":" + e.second + "\n";
    }

    return serialized_message;
}
Message str_to_message(std::string value){

}