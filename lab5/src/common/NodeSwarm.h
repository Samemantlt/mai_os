#pragma once

#include <list>
#include "Node.h"
#include "Transport.h"


class NodeSwarm : MessageVisitor {
public:
    int current_id{};
    Node *current;
    std::list<Node> nodes;

    void handle_ping(PingResponse &response) {}

    Transport transport{current, nodes, *this};

    void handle(SumNumbersResponse &response) override {
        transport.fire(*current, &response);
    }

    void handle(CreateNodeResponse &response) override {
        if (current_id != response.parent_id) {
            auto parent = std::find_if(nodes.begin(), nodes.end(),[&response](Node& n) {return n.related_ids.contains(response.parent_id); });

            parent->related_ids.insert(response.slave_id);
        }
        transport.fire(*current, &response);
    }

    void handle_pong(PingResponse &response) override {
        transport.fire(*current, &response);
    }

    void handle(ErrorResponse &response) override {
        transport.fire(*current, &response);
    }

    void handle(CreateNodeRequest &request) override {
        if (current_id == request.slave_id) {
            Node new_node = createProcess(request.id, request.heartbeat);
            nodes.push_back(new_node);

            CreateNodeResponse response;
            response.trace_id = request.trace_id;
            response.slave_id = request.id; // Use new node id
            response.sent = request.sent;
            response.pid = new_node.pid;
            response.parent_id = request.slave_id;

            handle(response);
            return;
        } else {
            for (auto &node: nodes) {
                if (node.related_ids.contains(request.slave_id)) {
                    transport.send(node, &request);
                }
            }
        }
    }

    void handle(SumNumbersRequest &request) override {
        if (current_id == request.slave_id) {
            Number sum{};
            for (auto num: request.numbers) {
                sum += num;
            }
            SumNumbersResponse response{sum};
            response.trace_id = request.trace_id;
            response.slave_id = request.slave_id;
            response.sent = request.sent;

            handle(response);
        } else {
            for (auto &node: nodes) {
                if (node.related_ids.contains(request.slave_id)) {
                    transport.send(node, &request);
                }
            }
        }
    }

    void handle(PingRequest &request) override {
        for (auto &node: nodes) {
            transport.fire(node, &request);
        }
    }

    void handle_timeout(int slave_id) override {

    }

    void handle_error(int slave_id, std::string error) override {
        ErrorResponse response;
        response.slave_id = current_id;
        response.trace_id = rand();
        response.sent = std::chrono::system_clock::now();
        response.message = error;

        transport.fire(*current, &response);
    }

    void timeout_step() {
        transport.timeout_step();
    }

    void receive_step() {
        transport.receive_step();
    }

    NodeSwarm(int current_id, Node *current) : current_id(current_id), current(current) {}
};