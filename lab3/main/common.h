#pragma once

#include <cstdlib>

#define MAX_LINE 40
#define SHARED_STR_SIZE (MAX_LINE * 100)  // Размер для mapped memory
#define MAPPED_FILE_PATH "/tmp/mai_os_mapped_file"

struct SharedData {
    char data[SHARED_STR_SIZE];
    size_t size;
    bool done;
};