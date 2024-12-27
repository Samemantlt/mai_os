#pragma once

#include <cstdlib>
#include <vector>
#include <chrono>
#include "AllocatorBase.h"
#include "result.h"

Result iterative_allocate_test(AllocatorBase& allocator, int max_size) {
    std::vector<void*> buffers;
    Result result{};

    auto t1 = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < max_size; i++) {
        auto buf = allocator.alloc(i);
        if (buf == nullptr)
            break;

        result.allocated_chars += i;
        buffers.push_back(buf);
    }

    auto t2 = std::chrono::high_resolution_clock::now();
    result.time_us = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    result.allocated_strings = buffers.size();

    return result;
}

Result iterative_free_test(AllocatorBase& allocator, int max_size) {
    std::vector<void*> buffers;
    Result result{};

    for (int i = 0; i < max_size; i++) {
        auto buf = allocator.alloc(i);
        if (buf == nullptr)
            break;

        result.allocated_chars += i;
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
