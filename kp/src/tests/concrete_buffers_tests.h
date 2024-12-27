#pragma once

#include <cstdlib>
#include <vector>
#include <chrono>
#include "AllocatorBase.h"
#include "result.h"


Result concrete_allocate_test(AllocatorBase& allocator, int size) {
    std::vector<void*> buffers;
    Result result{};

    auto t1 = std::chrono::high_resolution_clock::now();

    while (true) {
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

Result concrete_free_test(AllocatorBase& allocator, int size) {
    std::vector<void*> buffers;
    Result result{};

    while (true) {
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
