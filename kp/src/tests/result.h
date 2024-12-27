#pragma once

#include <cstdint>

struct Result {
    uint64_t time_us;
    int allocated_chars;
    int allocated_strings;
};