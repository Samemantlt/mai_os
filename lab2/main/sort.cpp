#include "sort.h"

void *parallelSort(void *u_arg) {
    auto *arg = static_cast<ParallelSortArg *>(u_arg);
    oddEvenMergeSort(arg->vector, arg->left, arg->right, arg->threadsLimiter);
    arg->threadsLimiter.releaseThread();
    return nullptr;
}

void oddEvenMergeSort(std::vector<int> &a, int startIndex, int length, ThreadsLimiter& threadsLimiter) {
    if (length <= 1)
        return;

    int halfLength = length / 2;

    if (threadsLimiter.lockThread()) {
        ParallelSortArg parallelArg{
                a,
                startIndex,
                halfLength,
                threadsLimiter
        };

        pthread_t thread;
        pthread_create(&thread, nullptr, parallelSort, &parallelArg);

        oddEvenMergeSort(a, startIndex + halfLength, halfLength, threadsLimiter);
        pthread_join(thread, nullptr);
    } else {
        oddEvenMergeSort(a, startIndex, halfLength, threadsLimiter);
        oddEvenMergeSort(a, startIndex + halfLength, halfLength, threadsLimiter);
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

bool ThreadsLimiter::lockThread() {
    bool result = false;

    pthread_mutex_lock(&mutex);
    if (currentCount < maxCount) {
        currentCount++;
        result = true;
    }
    pthread_mutex_unlock(&mutex);

    return result;
}

void ThreadsLimiter::releaseThread() {
    pthread_mutex_lock(&mutex);
    currentCount--;
    pthread_mutex_unlock(&mutex);
}

ThreadsLimiter::ThreadsLimiter(int maxCount) : maxCount(maxCount) {
    pthread_mutex_init(&mutex, nullptr);
}

ThreadsLimiter::~ThreadsLimiter() {
    if (disposed)
        return;

    pthread_mutex_destroy(&mutex);
    disposed = true;
}
