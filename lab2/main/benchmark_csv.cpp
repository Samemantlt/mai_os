#include <iostream>
#include <vector>
#include <chrono>

#include "sort.h"

// Сколько раз повторить сортировку
const int Times = 5;

void benchmarkSortIteration(std::vector<int>& originalArray, int threads) {
    ThreadsLimiter threadsLimiter(threads);

    std::vector<int> oeSorted = originalArray;
    oddEvenMergeSort(oeSorted, 0, oeSorted.size(), threadsLimiter);
}

double benchmarkSort(std::vector<int>& originalArray, int threads, int times) {
    benchmarkSortIteration(originalArray, threads);

    std::chrono::duration<double> sum{};

    for (int i = 0; i < times; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        benchmarkSortIteration(originalArray, threads);
        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> duration = end - start;
        sum += duration;
    }

    return (sum / times).count();
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

    for (int threads = 1; threads <= 64; threads++) {
        std::cout << threads << "; ";

        for (int i = 0; i < sizes.size(); i++) {
            auto seconds = benchmarkSort(originalArrays[i], threads, Times);
            std::cout << seconds << "; ";
        }
        std::cout << std::endl;
    }
    std::cout.flush();
}