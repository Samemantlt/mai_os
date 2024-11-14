#include <iostream>
#include <vector>
#include <chrono>

#include "sort.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <array_size> <threads_count>\n";
        return 1;
    }

    int arraySize = std::atoi(argv[1]);
    int threads = std::atoi(argv[2]);

    // Размер массива должен быть степенью 2
    if ((arraySize & (arraySize - 1)) != 0) {
        std::cerr << "Array size must be a power of 2\n";
        return 1;
    }

    std::vector<int> originalArray = createRandomValuesVector(arraySize);
    std::vector<int> oeSorted = originalArray;

    std::cout << "Starting sorting array with length: " << arraySize << "\n";
    std::cout << "Max threads: " << threads << std::endl;

    ThreadsLimiter threadsLimiter(threads);

    auto start = std::chrono::high_resolution_clock::now();

    oddEvenMergeSort(oeSorted, 0, oeSorted.size(), threadsLimiter);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Time taken: " << duration.count() << " seconds\n";

    for (int i = 1; i < arraySize; i++) {
        if (oeSorted[i] < oeSorted[i-1]) {
            std::cout << "Sorting failed!\n";
            return 0;
        }
    }
    std::cout << "Sorting successful!\n";

    return 0;
}
