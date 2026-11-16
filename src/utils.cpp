#include "../include/utils.h"

uint8_t hash_addr(std::string value){
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int len = 0;
    EVP_Digest(value.c_str(), value.size(), hash, &len, EVP_sha1(), nullptr);
    return hash[0]; //1 byte
}