#pragma once

#include "common/NodeSwarm.h"

class SlaveNodeSwarm : public NodeSwarm {
public:
    using NodeSwarm::NodeSwarm;

    std::chrono::time_point<std::chrono::system_clock> last_pong{};
    int heartbeat_time = 1000; // ms

    void handle(PingRequest &request) override {
        heartbeat_time = request.time;

        NodeSwarm::handle(request);
    }

    void pong_step() {
        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - last_pong).count() < heartbeat_time)
            return;

        PingResponse response;
        response.slave_id = current_id;
        response.trace_id = 0;
        response.sent = std::chrono::system_clock::now();

        handle_pong(response);
        last_pong = std::chrono::system_clock::now();
    }
};