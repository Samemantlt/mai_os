#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <sstream>
#include <cstring>
#include "../main/common.h"

void syncWithMemory(std::ostringstream &oss, SharedData* shared1) {
    auto length = oss.view().size();
    if (length > SHARED_STR_SIZE)
    {
        perror("Too long output");
        exit(1);
    }

    auto charPtr = oss.view().data();
    strcpy(shared1->data, charPtr);
    shared1->size = length;
    shared1->done = true;
    msync(shared1, sizeof(SharedData), MS_SYNC);
}

int main() {
    int fd1 = open(MAPPED_FILE_PATH, O_RDWR);
    SharedData* shared1 = (SharedData*)mmap(nullptr, sizeof(SharedData),
                                            PROT_READ | PROT_WRITE,
                                            MAP_SHARED, fd1, 0);

    std::ostringstream oss;

    while (true) {
        float result;
        if (!(std::cin >> result))
            break;

        while (std::cin.peek() != '\n') {
            float divider;
            std::cin >> divider;
            if (divider == 0) {
                syncWithMemory(oss, shared1);
                return 1;
            }

            result /= divider;
        }

        oss << result << "\n";
    }

    syncWithMemory(oss, shared1);
    return 0;
}