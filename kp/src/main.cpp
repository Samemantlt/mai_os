#include <iostream>
#include <sys/mman.h>
#include "NaiveAllocator.h"
#include "tests/random_buffers_tests.h"
#include "tests/iterative_buffers_tests.h"
#include "tests/concrete_buffers_tests.h"


void print_result(Result result, AllocatorBase& allocator) {
    std::cout << result.time_us << "\t" << result.allocated_strings << "\t" << result.allocated_chars << "\t" << (result.allocated_chars / (float)allocator.size) << std::endl;
}

template<class T>
AllocatorBase* create(size_t size) {
    void* mem = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    char* memory = static_cast<char*>(mem);
    auto a = new T(memory, size);
    // fulfill_and_release(*a);
    return a;
}

void fulfill_and_release(AllocatorBase& allocator) {
    const int seed = 1;
    const int max_size = 100'000;

    std::vector<void*> blocks;
    /*
    srand(1);
    while (true) {
        auto b = allocator.alloc(1 << (rand() % max_size));
        if (b == nullptr)
            break;
        blocks.push_back(b);
    }*/

    for (int i = 0; i < max_size; i++) {
        auto buf = allocator.alloc(i);
        if (buf == nullptr)
            break;

        blocks.push_back(buf);
    }

    for (auto& b : blocks) {
        allocator.free(b);
    }
}

template<class T>
void runTests() {
    std::cout << "Random:" << std::endl;

    AllocatorBase* allocator = create<T>(100 * 1024 * 1024);
    print_result(random_allocate_test(*allocator, 1, 10'000), *allocator);
    delete allocator;

    allocator = create<T>(100 * 1024 * 1024);
    print_result(random_free_test(*allocator, 1, 10'000), *allocator);
    delete allocator;


    std::cout << "\n\nIterative:" << std::endl;

    allocator = create<T>(100 * 1024 * 1024);
    print_result(iterative_allocate_test(*allocator, 100'000), *allocator);
    delete allocator;

    allocator = create<T>(100 * 1024 * 1024);
    print_result(iterative_free_test(*allocator, 100'000), *allocator);
    delete allocator;


    std::cout << "\n\nConcrete:" << std::endl;

    allocator = create<T>(100 * 1024 * 1024);
    print_result(concrete_allocate_test(*allocator, 10'000), *allocator);
    delete allocator;

    allocator = create<T>(100 * 1024 * 1024);
    print_result(concrete_free_test(*allocator, 10'000), *allocator);
    delete allocator;
}


int main() {
    std::cout << "[Naive]:" << std::endl;
    runTests<NaiveAllocator>();

    std::cout << "\n\n\n[Binary]:" << std::endl;
    runTests<BinaryAllocator>();

    return 0;
}