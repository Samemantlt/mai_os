#pragma once


#include <cstdlib>

#define MAX_LINE 40
#define SHARED_STR_SIZE (MAX_LINE * 100)  // Размер для mapped memory
#define MAPPED_FILE1 "/tmp/mapped_file1"
#define SEMAPHORE_NAME "/mai_lab_os_3g"

struct SharedData {
    char data[SHARED_STR_SIZE];
    size_t size;
    bool done;
};