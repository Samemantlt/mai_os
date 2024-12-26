#pragma once

#include <zmq.h>
#include <cstring>
#include "message.h"
#include "Node.h"

namespace {
    class MessageHeader {
        int slave_id;
        MessageType message_type;
    };
}


namespace messaging {
    void send_message(Node* node, Message& message);

    Message* receive_message(Node* node);
}