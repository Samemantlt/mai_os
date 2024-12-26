#pragma once

#include "message.h"
#include "unordered_set"

class Node {
public:
    int id;
    int pid;
    void *context;
    void *socket;
    std::string address;
    std::unordered_set<int> related_ids;
};

Node createNode(int id, bool is_child);

Node createProcess(int id, int heartbeat);