//
// Created by user on 12/26/24.
//

#include <zmq.h>
#include <csignal>
#include <iostream>
#include "Node.h"

Node createNode(int id, bool is_child)
{
    Node node;
    node.id = id;
    node.pid = getpid();

    node.context = zmq_ctx_new();
    node.socket = zmq_socket(node.context, ZMQ_DEALER);

    node.address = "tcp://127.0.0.1:" + std::to_string(5555 + id);
    node.related_ids.insert(id);

    if (is_child)
        zmq_connect(node.socket, node.address.c_str());
    else
        zmq_bind(node.socket, node.address.c_str());

    return node;
}

Node createProcess(int id, int heartbeat)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        // Дочерний процесс
        execl("./slave", "slave", std::to_string(id).c_str(), std::to_string(heartbeat).c_str(), NULL);
        throw std::runtime_error("Execl failed");
    }
    else if (pid == -1)
    {
        throw std::runtime_error("Fork failed failed");
    }

    // Родительский процесс
    Node node = createNode(id, false);
    node.id = id;
    node.related_ids.insert(id);
    node.pid = pid;
    return node;
}