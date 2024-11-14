#pragma once

#include <iostream>
#include <vector>
#include <pthread.h>

struct ThreadsLimiter {
    int currentCount = 1; // Первый поток - основной (который использует этот класс)
    int maxCount;

    bool lockThread();

    void releaseThread();

    [[maybe_unused]] explicit ThreadsLimiter(int maxCount);

    ~ThreadsLimiter();

private:
    pthread_mutex_t mutex{};
    bool disposed = false;
};

void oddEvenMergeSort(std::vector<int> &a, int startIndex, int length, ThreadsLimiter& threadsLimiter);

void oddEvenMerge(std::vector<int> &a, int startIndex, int length, int step);

void compareAndExchange(std::vector<int> &vector, int aIndex, int bIndex);

std::vector<int> createRandomValuesVector(size_t size);

struct ParallelSortArg {
    std::vector<int> &vector;
    int left;
    int right;
    ThreadsLimiter &threadsLimiter;
};