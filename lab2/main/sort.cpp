#include "sort.h"

void *parallelSort(void *u_arg) {
    auto *arg = static_cast<ParallelSortArg *>(u_arg);
    oddEvenMergeSort(*arg->vector, arg->left, arg->right, arg->powerOfParallelism);
    return nullptr;
}

void oddEvenMergeSort(std::vector<int> &a, int startIndex, int length, int powerOfParallelism = 0) {
    if (length <= 1)
        return;

    int halfLength = length / 2;

    if (powerOfParallelism > 0) {
        ParallelSortArg parallelArg{
                &a,
                startIndex,
                halfLength,
                powerOfParallelism - 1
        };

        pthread_t thread;
        pthread_create(&thread, nullptr, parallelSort, &parallelArg);

        oddEvenMergeSort(a, startIndex + halfLength, halfLength, powerOfParallelism - 1);
        pthread_join(thread, nullptr);
    } else {
        oddEvenMergeSort(a, startIndex, halfLength);
        oddEvenMergeSort(a, startIndex + halfLength, halfLength);
    }

    oddEvenMerge(a, startIndex, length, 1);
}

void oddEvenMerge(std::vector<int> &a, int startIndex, int length, int step) {
    int doubleStep = step * 2;
    if (doubleStep < length) {
        oddEvenMerge(a, startIndex, length, doubleStep);
        oddEvenMerge(a, startIndex + step, length, doubleStep);
        for (int i = startIndex + step; i + step < startIndex + length; i += doubleStep) {
            compareAndExchange(a, i, i + step);
        }
    } else {
        compareAndExchange(a, startIndex, startIndex + step);
    }
}

void compareAndExchange(std::vector<int> &vector, int aIndex, int bIndex) {
    if (vector[aIndex] > vector[bIndex]) {
        std::swap(vector[aIndex], vector[bIndex]);
    }
}

std::vector<int> createRandomValuesVector(size_t size) {
    std::vector<int> array(size);
    for (size_t i = 0; i < size; i++) {
        array[i] = std::rand() % 1024;
    }
    return array;
}