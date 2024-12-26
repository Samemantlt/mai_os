#include <iostream>
#include <csignal>
#include "common/messaging.h"
#include "common/NodeSwarm.h"
#include "SlaveNodeSwarm.h"

int main(int argc, char** argv) {
    int local_id = std::atoi(argv[1]);
    int heartbeat = std::atoi(argv[2]);
    Node node = createNode(local_id, true);
    SlaveNodeSwarm swarm(local_id, &node);
    swarm.heartbeat_time = heartbeat;

    while (true) {
        swarm.pong_step();
        swarm.receive_step();

        usleep(10 * 1000);
    }

    return 0;
}