#pragma once

#include <iostream>
#include <vector>

void oddEvenMergeSort(std::vector<int> &a, int startIndex, int length, int powerOfParallelism);

void oddEvenMerge(std::vector<int> &a, int startIndex, int length, int step);

void compareAndExchange(std::vector<int> &vector, int aIndex, int bIndex);

std::vector<int> createRandomValuesVector(size_t size);

struct ParallelSortArg {
    std::vector<int> *vector;
    int left;
    int right;
    int powerOfParallelism;
};