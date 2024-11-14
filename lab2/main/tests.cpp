#pragma once

#include "sort.h"

void printVector(std::vector<int> &vector) {
    std::cout << "{ ";
    for (auto &el: vector) {
        std::cout << el << ", ";
    }
    std::cout << " }";
}

void sortTestIteration(int count, int tries, int powerOfParallelism) {
    for (int tryIndex = 0; tryIndex < tries; tryIndex++) {
        std::vector<int> array = createRandomValuesVector(count);

        std::vector<int> oeSorted = array;
        oddEvenMergeSort(oeSorted, 0, oeSorted.size(), powerOfParallelism);

        std::vector<int> defaultSorted = array;
        std::sort(defaultSorted.begin(), defaultSorted.end());

        bool result = std::equal(oeSorted.begin(), oeSorted.end(), defaultSorted.begin(), defaultSorted.end());
        if (!result) {
            std::cout << "Error!!!\nInitial: ";
            printVector(array);

            std::cout << "\nActual:";
            printVector(oeSorted);

            std::cout << "\nExpected:";
            printVector(defaultSorted);

            std::cout << std::endl;
        }
    }
}

int main() {
    for (int powerOfParallelism = 1; powerOfParallelism < 10; powerOfParallelism++) {
        for (int i = 1; i <= 16; i *= 2) {
            sortTestIteration(i, 100, powerOfParallelism);
        }
    }

    std::cout << "End tests!" << std::endl;
}