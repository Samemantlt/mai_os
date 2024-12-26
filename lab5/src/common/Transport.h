#pragma once

#include "message.h"
#include "messaging.h"
#include "Node.h"
#include <functional>
#include <utility>
#include <list>
#include <zmq.h>
#include <cstring>

const int TIMEOUT_MS = 5000;

class Transport {
public:
    struct PendingMessage {
        int trace_id;
        int slave_id;
        std::chrono::system_clock::time_point sent;
    };

    std::vector<PendingMessage> pending;
    std::list<Node> &nodes;
    Node *parent;
    MessageVisitor &ping_handler;

    explicit Transport(Node *parent, std::list<Node> &nodes, MessageVisitor &ping_handler) : parent(parent),
                                                                                             nodes(nodes), ping_handler(
                    ping_handler) {

    }

    void send(Node &node, Message *message) {
        fire(node, message);
        pending.push_back({message->trace_id, message->slave_id, message->sent});
    }

    void fire(Node &node, Message *message) {
        messaging::send_message(&node, *message);
    }

    void timeout_step() {
        std::vector<PendingMessage *> to_erase;
        for (int i = 0; i < pending.size(); i++) {
            if (std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now() - pending[i].sent).count() > TIMEOUT_MS) {
                PendingMessage &msg = pending[i];
                ping_handler.handle_timeout(msg.slave_id);

                to_erase.push_back(&msg);
            }
        }

        for (auto &e: to_erase) {
            std::erase_if(pending,
                          [&e](PendingMessage &msg2) { return msg2.trace_id == e->trace_id; });
        }
    }

    void receive_step() {
        if (parent) {
            Message *msg = messaging::receive_message(parent);
            if (msg != nullptr) {
                try {
                    msg->accept(ping_handler);
                } catch (std::exception& ex) {
                    ping_handler.handle_error(parent->id, std::string(ex.what()));
                }
                delete msg;
            }
        }

        for (auto &node: nodes) {
            Message *msg = messaging::receive_message(&node);
            if (msg == nullptr)
                continue;

            std::erase_if(pending,
                          [&msg](PendingMessage &msg2) { return msg2.trace_id == msg->trace_id; });
            try {
                msg->accept(ping_handler);
            } catch (std::exception& ex) {
                ping_handler.handle_error(parent == nullptr ? -1 : parent->id, std::string(ex.what()));
            }
            delete msg;
        }
    }
};
