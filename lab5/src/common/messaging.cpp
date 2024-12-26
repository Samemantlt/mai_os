#include "messaging.h"

void messaging::send_message(Node *node, Message &message) {
    std::ostringstream stream;
    stream << static_cast<int>(message.get_message_type()) << " ";
    message.serialize(stream);

    std::string serialized = stream.str();

    zmq_msg_t request_message;
    zmq_msg_init_size(&request_message, serialized.length());
    std::memcpy(zmq_msg_data(&request_message), serialized.c_str(), serialized.length());
    zmq_msg_send(&request_message, node->socket, ZMQ_DONTWAIT);
}

Message *messaging::receive_message(Node *node) {
    zmq_msg_t request;
    zmq_msg_init(&request);
    auto size = zmq_msg_recv(&request, node->socket, ZMQ_DONTWAIT);
    if (size == -1)
        return nullptr;

    std::vector<char> buffer(size + 1);
    buffer[size] = '\0';

    std::memcpy(&buffer[0], zmq_msg_data(&request), size);

    std::string str(&buffer[0]);
    std::istringstream stream(str);
    int type_int;
    stream >> type_int;
    auto type = static_cast<MessageType>(type_int);

    Message* message = create_message_by_type(type);
    message->deserialize(stream);
    return message;
}
