#pragma once

#include <cstdlib>

class AllocatorBase {
public:
    size_t size;

    explicit AllocatorBase(size_t size) : size(size) {}

    virtual ~AllocatorBase() = default;

    virtual void* alloc(size_t block_size) = 0;

    virtual void* free(void* block) = 0;
};