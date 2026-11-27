#include "DHTProtocol.h"
#include "../../include/network.h"
#define TABLE_SIZE 8  //identificador entero de 8 bits

namespace {
NodeRef read_node(const Message& message, const std::string& key) {
    const int node_id = std::stoi(message.args.at(key + "_id"));
    const int node_port = std::stoi(message.args.at(key + "_port"));
    if (node_id < 0 || node_id > 255 || node_port <= 0 || node_port > 65535) {
        throw std::runtime_error("Nodo fuera de rango");
    }
    return NodeRef{static_cast<uint8_t>(node_id),
                   message.args.at(key + "_ip"), node_port};
}
void write_node(Message& message, const std::string& key, NodeRef node) {
    message.args[key + "_id"] = std::to_string(static_cast<unsigned>(node.id));
    message.args[key + "_ip"] = node.ip;
    message.args[key + "_port"] = std::to_string(node.port);
}
bool same_node(const NodeRef& a, const NodeRef& b) {
    return a.id == b.id && a.ip == b.ip && a.port == b.port;
}
bool valid_node(const NodeRef& node) {
    return !node.ip.empty() && node.port > 0;
}
}

Node::Node() {}
Node::Node(const uint8_t& id, const int& sock, const std::string& ip, const int& port){
    this->id = id;
    this->socket = sock;
    this->ip = ip;
    this->port = port;
    this->fingers = std::vector<NodeRef>(TABLE_SIZE);
}

NodeRef Node::self_ref() {
    return NodeRef{this->id, this->ip, this->port};
}

// Intervalo circular (start,end) o (start,end]
bool Node::in_interval(uint8_t value, uint8_t start, uint8_t end, bool include_end) {
    if (start < end) return value > start && (value < end || (include_end && value == end));
    if (start > end) return value > start || value < end || (include_end && value == end);
    return include_end || value != start;
}

Message build_fsuccessor(uint32_t id, const std::string& ip, int port){
    Message message{};
    message.type = MessageType::FIND_SUCC;
    std::unordered_map<std::string,std::string> args{};
    args.insert({"target", std::to_string(id)});
    args.insert({"origin_ip", ip});
    args.insert({"origin_port", std::to_string(port)});
    args.insert({"origin_id", std::to_string(static_cast<unsigned>(hash_addr(ip + ":" + std::to_string(port))))});
    args.insert({"purpose", "find"});
    args.insert({"ttl", "256"});
    message.args = std::move(args);
    return message;
}

void Node::join(const std::string& ip, int port){
    // El nodo puede apuntarse a si mismo para iniciar un anillo nuevo.
    if (ip == this->ip && port == this->port) {
        std::lock_guard<std::mutex> lock(*state_mutex);
        if (joined || joining) return;
        const NodeRef self{.id = this->id, .ip = this->ip, .port = this->port};
        this->predecessor = self;
        this->has_predecessor = true;
        std::fill(fingers.begin(), fingers.end(), self);
        joined = true;
        std::cout << "Anillo creado: " << static_cast<unsigned>(this->id) << std::endl;
        return;
    }
    {
        std::lock_guard<std::mutex> lock(*state_mutex);
        if (joined || joining) return;
    }

    int target = connect_to(ip, port);
    if (target == -1){ //no hay ring aun
        const NodeRef self{.id = this->id, .ip = this->ip, .port = this->port};
        this->predecessor = self;
        std::fill(fingers.begin(), fingers.end(), self);
        joined = true;
        std::cout << "Anillo creado: " << static_cast<unsigned>(this->id) << std::endl;
    } else{
        close_socket(target); // la solicitud real usa send_message()
        {
            std::lock_guard<std::mutex> lock(*state_mutex);
            joining = true;
        }
        Message message = build_fsuccessor(this->id, this->ip, this->port);
        message.args["purpose"] = "join";
        NodeRef entry{0, ip, port};
        send_message(entry, message);
        std::cout << "JOIN enviado; esperando respuesta" << std::endl;
    }
    std::cout<<"El ID del nodo es: "<< static_cast<unsigned>(this->id) <<std::endl;
};


void Node::print_status() {
    std::lock_guard<std::mutex> lock(*state_mutex);
    std::cout << "ID: " << static_cast<unsigned>(id) << std::endl;
    std::cout << "Estado: " << (joined ? "joined" : joining ? "joining" : "sin anillo") << std::endl;
    if (joined) {
        NodeRef successor = fingers[0];
        std::cout << "Sucesor: " << noderef_to_str(successor) << std::endl;
        if (has_predecessor) {
            NodeRef pred = predecessor;
            std::cout << "Predecesor: " << noderef_to_str(pred) << std::endl;
        } else std::cout << "Predecesor: desconocido" << std::endl;
    }
}

void Node::find_successor(uint8_t id){
    {
        std::lock_guard<std::mutex> lock(*state_mutex);
        if (!joined) return;
    }

    Message message = build_fsuccessor(id, this->ip, this->port);
    handle_fsuccessor(message);
    
}

NodeRef Node::closest_preceding_finger(uint8_t target) {
    std::lock_guard<std::mutex> lock(*state_mutex);
    for (int i = TABLE_SIZE - 1; i >= 0; --i) {
        NodeRef finger = fingers[i];
        if (valid_node(finger) && !same_node(finger, self_ref()) &&
            in_interval(finger.id, id, target)) return finger;
    }
    return fingers[0];
}


void Node::handle_fsuccessor(const Message& message) {
    NodeRef successor;
    {
        std::lock_guard<std::mutex> lock(*state_mutex);
        if (!joined) return;
        successor = fingers[0];
    }
    const int t = std::stoi(message.args.at("target"));
    const int ttl = std::stoi(message.args.at("ttl"));
    if (t < 0 || t > 255 || ttl <= 0) return;
    const uint8_t target = static_cast<uint8_t>(t);

    if (target == this->id || same_node(successor, self_ref()) ||
        in_interval(target, this->id, successor.id, true)) {
        Message answer{};
        answer.type = MessageType::FOUND_SUCC;
        answer.args = message.args;
        write_node(answer, "result", target == this->id ? self_ref() : successor);
        NodeRef origin = read_node(message, "origin");
        // No es necesario un round-trip de red para resolver una consulta local.
        if (same_node(origin, self_ref())) handle_found_successor(answer);
        else send_message(origin, answer);
        return;
    }

    NodeRef next = closest_preceding_finger(target);
    if (!valid_node(next) || same_node(next, self_ref())) next = successor;
    if (same_node(next, self_ref())) return;
    Message forward = message;
    forward.args["ttl"] = std::to_string(ttl - 1);
    send_message(next, forward);
}

void Node::handle_found_successor(const Message& message) {
    NodeRef result = read_node(message, "result");
    const std::string purpose = message.args.at("purpose");
    if (purpose == "join") {
        {
            std::lock_guard<std::mutex> lock(*state_mutex);
            if (!joining) return;
            if (result.id == id && !same_node(result, self_ref())) {
                joining = false;
                std::cerr << "ID duplicado: " << static_cast<unsigned>(id)
                          << ". Cambie el puerto y vuelva a iniciar el nodo." << std::endl;
                return;
            }
            fingers[0] = result;
            joining = false;
            joined = true;
            has_predecessor = false;
        }
        create_fingers();
        Message notification{};
        notification.type = MessageType::NOTIFY;
        write_node(notification, "node", self_ref());
        send_message(result, notification);
        std::cout << "JOIN completado; sucesor: " << noderef_to_str(result) << std::endl;
    } else if (purpose == "finger") {
        const int index = std::stoi(message.args.at("index"));
        if (index < 1 || index >= TABLE_SIZE) return;
        std::lock_guard<std::mutex> lock(*state_mutex);
        if (joined) fingers[index] = result;
    } else if (purpose == "find") {
        std::cout << "succ(" << message.args.at("target") << ") = "
                  << noderef_to_str(result) << std::endl;
    }
}

void Node::notify(const NodeRef& node) {
    if (!valid_node(node) || same_node(node, self_ref()) || node.id == id) return;
    std::lock_guard<std::mutex> lock(*state_mutex);
    if (!joined) return;
    if (!has_predecessor || same_node(predecessor, self_ref()) ||
        in_interval(node.id, predecessor.id, id)) {
        predecessor = node;
        has_predecessor = true;
    }
    // En anillo de un unico nodo, el primer ingreso cambia tambien su sucesor
    if (same_node(fingers[0], self_ref())) fingers[0] = node;
}

void Node::stabilize() {
    NodeRef successor;
    {
        std::lock_guard<std::mutex> lock(*state_mutex);
        if (!joined) return;
        successor = fingers[0];
    }
    if (same_node(successor, self_ref())) return;
    Message request{};
    request.type = MessageType::GET_PRED;
    write_node(request, "origin", self_ref());
    send_message(successor, request);
}

void Node::handle_set_succ(const Message& message) {
    NodeRef origin = read_node(message, "node");
    {
        std::lock_guard<std::mutex> lock(*state_mutex);
        if(!this->joined) return;
        this->fingers[0] = origin;
        std::cout << "SUCCESSOR NOW: "
          << noderef_to_str(fingers[0]) << std::endl;
    }
    std::cout << "SUCCESSOR NOW: "
          << noderef_to_str(fingers[0]) << std::endl;
}

void Node::handle_set_pred(const Message& message) {
    NodeRef origin = read_node(message, "node");
    {
        std::lock_guard<std::mutex> lock(*state_mutex);
        if(!this->joined) return;
        this->predecessor = origin;
        std::cout << "PREDECESSOR NOW: "
          << noderef_to_str(predecessor) << std::endl;
    }
}




void Node::handle_pred_reply(const Message& message) {
    NodeRef old_successor;
    {
        std::lock_guard<std::mutex> lock(*state_mutex);
        if (!joined || !same_node(fingers[0], read_node(message, "source"))) return;
        old_successor = fingers[0];
    }
    if (message.args.at("has_pred") == "1") {
        NodeRef candidate = read_node(message, "pred");
        if (valid_node(candidate) && !same_node(candidate, self_ref()) &&
            in_interval(candidate.id, id, old_successor.id)) {
            std::lock_guard<std::mutex> lock(*state_mutex);
            if (same_node(fingers[0], old_successor)) fingers[0] = candidate;
        }
    }
    NodeRef successor;
    {
        std::lock_guard<std::mutex> lock(*state_mutex);
        if (!joined) return;
        successor = fingers[0];
    }
    Message notification{};
    notification.type = MessageType::NOTIFY;
    write_node(notification, "node", self_ref());
    send_message(successor, notification);
}

void Node::create_fingers() {
    std::lock_guard<std::mutex> lock(*state_mutex);
    if (!joined) return;
    // finger[0] es el sucesor. Los demas se inicializan con el mismo
    // nodo, hasta que fix_fingers() calcule sus destinos definitivos.
    for (int i = 1; i < TABLE_SIZE; ++i) fingers[i] = fingers[0];
    next_finger = 1;
}

void Node::fix_fingers() {
    int index;
    {
        std::lock_guard<std::mutex> lock(*state_mutex);
        if (!joined) return;
        index = next_finger;
        next_finger = (next_finger == TABLE_SIZE - 1) ? 1 : next_finger + 1;
    }
    const uint8_t start = static_cast<uint8_t>((static_cast<unsigned>(id) + (1u << index)) % 256u);
    Message message = build_fsuccessor(start, this->ip, this->port);
    message.args["purpose"] = "finger";
    message.args["index"] = std::to_string(index);
    handle_fsuccessor(message);
}

void Node::handle_message(const Message& message) {
    switch (message.type) {
        case MessageType::FIND_SUCC:
            handle_fsuccessor(message);
            break;
        case MessageType::FOUND_SUCC:
            handle_found_successor(message);
            break;
        case MessageType::NOTIFY:
            notify(read_node(message, "node"));
            break;
        case MessageType::GET_PRED: {
            Message reply{};
            reply.type = MessageType::PRED_REPLY;
            {
                std::lock_guard<std::mutex> lock(*state_mutex);
                if (!joined) break;
                reply.args["has_pred"] = has_predecessor ? "1" : "0";
                write_node(reply, "source", self_ref());
                if (has_predecessor) write_node(reply, "pred", predecessor);
            }
            NodeRef origin = read_node(message, "origin");
            send_message(origin, reply);
            break;
        }
        case MessageType::PRED_REPLY:
            handle_pred_reply(message);
            break;
        case MessageType::PING: {
            Message reply{};
            reply.type = MessageType::PONG;
            NodeRef origin = read_node(message, "origin");
            send_message(origin, reply);
            break;
        }
        case MessageType::SET_SUCCESSOR: {
            handle_set_succ(message);
        }
        case MessageType::SET_PREDECESSOR: {
            handle_set_pred(message);
        }
        default:
            break;
    }
}

void Node::maintenance(){
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        if (!running) break;
        stabilize();
        //fix_fingers();
    }
}


std::vector<NodeRef> Node::get_table() {
    std::lock_guard<std::mutex> lock(*state_mutex);
    return fingers;
}


int Node::get_socket(){
    return this->socket;
}

bool Node::is_running() {
    std::lock_guard<std::mutex> lock(*state_mutex);
    return running;
}

void Node::stop() {
    {
        std::lock_guard<std::mutex> lock(*state_mutex);
        running = false;
    }
}

void Node::leave(){
    NodeRef succ;
    NodeRef pred;
    {
        std::lock_guard<std::mutex> lock(*state_mutex);
        if(!joined) return;

        succ = fingers[0];
        pred = this->predecessor;
    }
    if(same_node(pred, self_ref()) && same_node(succ, self_ref())){
        std::cout<<"EL ANILLO TENÍA UN SOLO NODO"<<std::endl;
    } else {
        Message set_pred{};
        set_pred.type = MessageType::SET_PREDECESSOR;
        write_node(set_pred, "node", pred);
        send_message(succ, set_pred);
        std::cout << "SET_PREDECESSOR ENVIADO A NODO CON ID:"<< succ.id << " IP: " << succ.ip << " PUERTO: "<< succ.port << std::endl;
        Message set_succ{};
        set_succ.type = MessageType::SET_SUCCESSOR;
        write_node(set_succ, "node", succ);
        send_message(pred, set_succ);
        std::cout << "SET_SUCCESSOR ENVIADO A NODO CON ID:"<< pred.id << " IP: " << pred.ip << " PUERTO: "<< pred.port << std::endl;
    }
    {   
        std::lock_guard<std::mutex> lock(*state_mutex);
        this->running = false;
        this->joined = false;
        this->joining = false;
    }
    stop();
    
}



