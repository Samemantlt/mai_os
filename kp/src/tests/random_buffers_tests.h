#pragma once

#include <cstdlib>
#include <vector>
#include <chrono>
#include "AllocatorBase.h"
#include "result.h"


Result random_allocate_test(AllocatorBase& allocator, int seed, int max_size) {
    std::srand(seed);
    std::vector<void*> buffers;
    Result result{};

    auto t1 = std::chrono::high_resolution_clock::now();

    while (true) {
        int size = std::rand() % max_size;
        auto buf = allocator.alloc(size);
        if (buf == nullptr)
            break;

        result.allocated_chars += size;
        buffers.push_back(buf);
    }

    auto t2 = std::chrono::high_resolution_clock::now();
    result.time_us = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    result.allocated_strings = buffers.size();

    return result;
}

Result random_free_test(AllocatorBase& allocator, int seed, int max_size) {
    std::srand(seed);
    std::vector<void*> buffers;
    Result result{};

    while (true) {
        int size = std::rand() % max_size;
        auto buf = allocator.alloc(size);
        if (buf == nullptr)
            break;

        result.allocated_chars += size;
        buffers.push_back(buf);
    }

    auto t1 = std::chrono::high_resolution_clock::now();

    for (auto& buf : buffers) {
        allocator.free(buf);
    }

    auto t2 = std::chrono::high_resolution_clock::now();

    result.time_us = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    result.allocated_strings = buffers.size();

    return result;
}
