#pragma once

#include <iostream>
#include <vector>
#include <chrono>

#include "sort.h"

void benchmarkSortIteration(std::vector<int>& originalArray, int threads) {
    ThreadsLimiter threadsLimiter(threads);

    std::vector<int> sorted = originalArray;
    oddEvenMergeSort(sorted, 0, sorted.size(), threadsLimiter);
}

void benchmarkSort(std::vector<int>& originalArray, int threads, int times) {
    benchmarkSortIteration(originalArray, threads);

    std::chrono::duration<double> sum{};
    std::chrono::duration<double> min = std::chrono::duration<double>::max();
    std::chrono::duration<double> max = std::chrono::duration<double>::min();

    for (int i = 0; i < times; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        benchmarkSortIteration(originalArray, threads);
        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> duration = end - start;
        sum += duration;
        if (duration < min)
            min = duration;
        if (duration > max)
            max = duration;
    }

    std::cout << "Threads: " << threads << "\tTimes: " << times << "\tAvg: " << (sum / times).count() << "\tMin: "<< min.count() << "\tMax: " << max.count() << "\n";
}

int main() {
    std::vector<int> originalArray = createRandomValuesVector(1024 * 16);

    for (int threads = 1; threads <= 64; threads++)
        benchmarkSort(originalArray, threads, 512);
}