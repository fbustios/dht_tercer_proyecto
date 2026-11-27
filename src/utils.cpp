#include "../include/utils.h"
#include <sstream>
#include <stdexcept>

uint8_t hash_addr(std::string value){
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int len = 0;
    EVP_Digest(value.c_str(), value.size(), hash, &len, EVP_sha1(), nullptr);
    return hash[0]; //1 byte
}

std::string message_to_str(Message& m){ // se agrega el tipo para poder despachar mensajes
    std::string result = "type:" + std::to_string(static_cast<int>(m.type)) + "\n";
    for (const auto& e: m.args){
        result+= e.first + ":" + e.second + "\n";
    }
    return result;
}
Message str_to_message(std::string value){
    Message message{};
    std::istringstream stream(value);
    std::string line;
    bool has_type = false;
    while (std::getline(stream, line)) {
        const auto colon = line.find(':');
        if (colon == std::string::npos) continue;
        const std::string key = line.substr(0, colon);
        const std::string content = line.substr(colon + 1);
        if (key == "type") {
            const int type = std::stoi(content);
            if (type < 0 || type > static_cast<int>(MessageType::SET_SUCCESOR)) {
                throw std::runtime_error("Tipo de mensaje desconocido");
            }
            message.type = static_cast<MessageType>(type);
            has_type = true;
        } else {
            message.args[key] = content;
        }
    }
    if (!has_type) throw std::runtime_error("Mensaje sin tipo");
    return message;
}

std::string noderef_to_str(NodeRef &node)
{
    return std::to_string(static_cast<unsigned>(node.id)) + " (" + node.ip + ":" +
           std::to_string(node.port) + ")";
}