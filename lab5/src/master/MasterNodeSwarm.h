#pragma once

#include <iostream>
#include <unordered_set>
#include "common/NodeSwarm.h"

class MasterNodeSwarm : public NodeSwarm {
public:
    int heartbeat_time = 1000; // ms

    std::unordered_map<int, std::chrono::time_point<std::chrono::system_clock>> heartbeats;
    bool is_available(int slave_id) {
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - heartbeats[slave_id]).count();
        return ms < 4 * heartbeat_time;
    }

    std::unordered_set<int> all_ids;

    using NodeSwarm::NodeSwarm;
    using NodeSwarm::handle;

    void handle(CreateNodeResponse &response) override {
        if (current_id != response.parent_id) {
            auto parent = std::find_if(nodes.begin(), nodes.end(),[&response](Node& n) {return n.related_ids.contains(response.parent_id); });

            parent->related_ids.insert(response.slave_id);
        }

        all_ids.insert(response.slave_id);
        // heartbeats[response.slave_id] = std::chrono::system_clock::now();
        std::cout << "Ok: " << response.pid << std::endl;
    }

    void handle_pong(PingResponse &response) override {
        heartbeats[response.slave_id] = std::chrono::system_clock::now();
    }

    void handle(SumNumbersResponse &response) override {
        std::cout << "Ok:" << response.slave_id << ": " << response.sum << std::endl;
    }

    void handle(ErrorResponse &response) override {
        std::cout << response.message << std::endl;
    }

    void heartbeats_step() {
    }

    void handle_timeout(int slave_id) override {
        std::cout << "Error:" << slave_id << ": Node is unavailable" << std::endl;
    }

    void handle_error(int slave_id, std::string error) override {
        if (error.starts_with("Error:")) {
            std::cout << slave_id << ": " << error.substr(6) << std::endl;
        } else {
            std::cout << "Error:" << slave_id << ": " << error << std::endl;
        }
    }
};
