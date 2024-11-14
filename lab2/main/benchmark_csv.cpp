#pragma once

#include <iostream>
#include <vector>
#include <chrono>

#include "sort.h"

// Сколько раз повторить сортировку
const int Times = 7;

void benchmarkSortIteration(std::vector<int>& originalArray, int powerOfParallelism) {
    std::vector<int> oeSorted = originalArray;
    oddEvenMergeSort(oeSorted, 0, oeSorted.size(), powerOfParallelism);
}

double benchmarkSort(std::vector<int>& originalArray, int powerOfParallelism, int times) {
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

    return ((sum - min - max) / (times - 2)).count();
}

int main() {
    std::vector<size_t> sizes = {
        1 << 5,
        1 << 10,
        1 << 15,
        1 << 20,
        1 << 22,
    };

    std::cout << "Threads;";

    std::vector<std::vector<int>> originalArrays;
    for (auto size : sizes) {
        std::cout << "Size " << size << "; ";
        originalArrays.push_back(createRandomValuesVector(size));
    }
    std::cout << std::endl;

    for (int powerOfParallelism = 0; powerOfParallelism <= 6; powerOfParallelism++) {
        std::cout << (1 << powerOfParallelism) << "; ";

        for (int i = 0; i < sizes.size(); i++) {
            auto seconds = benchmarkSort(originalArrays[i], powerOfParallelism, Times);
            std::cout << seconds << "; ";
        }
        std::cout << std::endl;
    }
    std::cout.flush();
}