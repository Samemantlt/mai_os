#include <string>
#include <sstream>
#include <iostream>
#include <unordered_set>
#include <list>
#include <csignal>
#include "common/message.h"
#include "common/messaging.h"
#include "common/Node.h"
#include "common/Transport.h"
#include "common/NodeSwarm.h"
#include "MasterNodeSwarm.h"


MasterNodeSwarm swarm(-1, nullptr);


void create_slave(int id, int parentId = -1) {
    if (swarm.all_ids.contains(id)) {
        std::cout << "Error: Already exists" << std::endl;
        return;
    }
    if (!swarm.all_ids.contains(parentId)) {
        std::cout << "Error: Parent not found" << std::endl;
        return;
    }

    CreateNodeRequest request;
    request.trace_id = std::rand();
    request.sent = std::chrono::system_clock::now();
    request.id = id;
    request.slave_id = parentId;
    request.heartbeat = swarm.heartbeat_time;

    swarm.handle(request);
}

void exec_sum(int slave_id, const std::vector<Number>& numbers) {
    if (!swarm.all_ids.contains(slave_id)) {
        std::cout << "Error:" << slave_id << ": Not found" << std::endl;
        return;
    }

    SumNumbersRequest request;
    request.trace_id = std::rand();
    request.sent = std::chrono::system_clock::now();
    request.numbers = numbers;
    request.slave_id = slave_id;

    swarm.handle(request);
}

void new_heartbeat_command(int new_time){
    swarm.heartbeat_time = new_time;
    for (auto id : swarm.all_ids) {
        PingRequest request;
        request.trace_id = std::rand();
        request.sent = std::chrono::system_clock::now();
        request.slave_id = id;
        request.time = new_time;

        swarm.handle(request);
    }
    std::cout << "Ok" << std::endl;
}

void ping_command(int slave_id) {
    if (!swarm.all_ids.contains(slave_id)) {
        std::cout << "Error: Slave not found" << std::endl;
        return;
    }

    bool available = swarm.is_available(slave_id);
    std::cout << "Ok: " << (available ? 1 : 0) << std::endl;
}

void process_command(const std::string& command) {
    std::istringstream stream(command);

    std::string command_name;
    stream >> command_name;

    if (command_name == "create") {
        int id;
        stream >> id;

        int parent_id = -1;
        if (!stream.eof())
            stream >> parent_id;

        create_slave(id, parent_id);
        return;
    }

    if (command_name == "exec") {
        int slave_id = 0;
        stream >> slave_id;

        int count;
        stream >> count;

        if (count < 0) {
            std::cout << "Error: numbers' count cannot be less than zero" << std::endl;
        }

        std::vector<Number> numbers(count);
        for (int i = 0; i < count; i++) {
            stream >> numbers[i];
        }

        exec_sum(slave_id, numbers);
        return;
    }

    if (command_name == "heartbeat") {
        int time = 0;
        stream >> time;

        new_heartbeat_command(time);
        return;
    }

    if (command_name == "ping") {
        int slave_id = 0;
        stream >> slave_id;

        ping_command(slave_id);
        return;
    }

    std::cout << "Error: Unknown command" << std::endl;
}

bool input_available() {
    timeval tv{0, 0};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}

int main() {
    swarm.all_ids.insert(-1);

    while (true) {
        swarm.receive_step();
        swarm.timeout_step();
        swarm.heartbeats_step();

        if (!input_available()) {
            usleep(10 * 1000);
            continue;
        }

        std::string command;
        std::getline(std::cin, command);
        if (command == "exit")
            break;

        process_command(command);
    }

    return 0;
}