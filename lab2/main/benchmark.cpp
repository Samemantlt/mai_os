#pragma once

#include <iostream>
#include <vector>
#include <chrono>

#include "sort.h"

void benchmarkSortIteration(std::vector<int>& originalArray, int powerOfParallelism) {
    std::vector<int> sorted = originalArray;
    oddEvenMergeSort(sorted, 0, sorted.size(), powerOfParallelism);
}

void benchmarkSort(std::vector<int>& originalArray, int powerOfParallelism, int times) {
    benchmarkSortIteration(originalArray, powerOfParallelism);

    std::chrono::duration<double> sum{};
    std::chrono::duration<double> min = std::chrono::duration<double>::max();
    std::chrono::duration<double> max = std::chrono::duration<double>::min();

    for (int i = 0; i < times; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        benchmarkSortIteration(originalArray, powerOfParallelism);
        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> duration = end - start;
        sum += duration;
        if (duration < min)
            min = duration;
        if (duration > max)
            max = duration;
    }

    std::cout << "Threads: " << (1 << (powerOfParallelism - 1)) << "\tTimes: " << times << "\tAvg: " << (sum / times).count() << "\tMin: "<< min.count() << "\tMax: " << max.count() << "\n";
}

int main() {
    std::vector<int> originalArray = createRandomValuesVector(1024 * 16);

    for (int powerOfParallelism = 1; powerOfParallelism <= 12; powerOfParallelism++)
        benchmarkSort(originalArray, powerOfParallelism, 512);
}