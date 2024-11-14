#include <iostream>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sstream>
#include <cstring>
#include "../main/common.h"
#include <pthread.h>
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>

int main() {
    int fd1 = open(MAPPED_FILE1, O_RDWR);
    SharedData* shared1 = (SharedData*)mmap(nullptr, sizeof(SharedData),
                                            PROT_READ | PROT_WRITE,
                                            MAP_SHARED, fd1, 0);

    sem_t* semaphore = sem_open(SEMAPHORE_NAME, O_RDWR);
    if (semaphore == SEM_FAILED)
    {
        std::cerr << "Ошибка открытия семафора" << std::endl;
        exit(1);
    }

    std::ostringstream oss;

    while (true) {
        float result;
        if (!(std::cin >> result))
            break;

        while (std::cin.peek() != '\n') {
            float divider;
            std::cin >> divider;
            if (divider == 0)
                return 1;

            result /= divider;
        }

        oss << result << "\n";
    }

    auto length = oss.view().size();
    if (length > SHARED_STR_SIZE)
    {
        perror("Too long output");
        sem_close(semaphore);
        return 1;
    }

    auto charPtr = oss.view().data();
    strcpy(shared1->data, charPtr);
    shared1->size = length;
    shared1->done = true;
    msync(shared1, sizeof(SharedData), MS_SYNC);

    sem_post(semaphore);
    sem_close(semaphore);

    return 0;
}