#pragma once

#include <vector>
#include <cstddef>
#include <map>
#include <unordered_map>
#include <list>
#include <sys/mman.h>
#include "AllocatorBase.h"

const int alignment = alignof(std::max_align_t);

class NaiveAllocator : public AllocatorBase {
public:
    char* memory;

    char* top_memory;

    struct Block {
        void* ptr;
        size_t size;
        bool is_free;
    };

    std::vector<Block> blocks{};

    NaiveAllocator(char* memory, size_t size) : AllocatorBase(size), memory(memory), top_memory(memory) {}
    ~NaiveAllocator() override {
        munmap(static_cast<void*>(memory), size);
    }

    void * alloc(size_t block_size) override {
        block_size = (block_size / alignment + 1) * alignment;
        for (auto& block : blocks) {
            if (block.is_free && block.size >= block_size) {
                block.is_free = false;
                return block.ptr;
            }
        }

        return alloc_new(block_size);
    }

    void* alloc_new(size_t block_size) {
        if (top_memory + block_size > memory + size)
            return nullptr; // Not enough memory

        Block block{top_memory, block_size, false};
        blocks.push_back(block);
        top_memory += block_size;

        return block.ptr;
    }

    void* free(void* block_ptr) override {
        for (auto& block : blocks) {
            if (block.ptr == block_ptr) {
                block.is_free = true;
                break;
            }
        }

        return block_ptr;
    }
};

class BinaryAllocator : public AllocatorBase {
public:
    char* memory;
    char* top_memory;

    struct Block {
        void* ptr;
        size_t size;
    };

    std::unordered_map<void*, Block> used;
    std::unordered_map<size_t, std::list<Block>> free_blocks;

    BinaryAllocator(char* memory, size_t size) : AllocatorBase(size), memory(memory), top_memory(memory) {
        for (int i = 1; (1 << i) <= size; i++) {
            free_blocks[1 << i] = std::list<Block>();
        }
    }

    ~BinaryAllocator() override {
        munmap(static_cast<void*>(memory), size);
    }

    void * alloc(size_t block_size) override {
        block_size = (block_size / alignment + 1) * alignment;

        for (int i = 1 << bits(block_size); i <= size; i <<= 1) {
            auto& blocks = free_blocks[i];
            if (blocks.empty())
                continue;

            auto block = blocks.back();

            used[block.ptr] = block;
            blocks.pop_back();
            return block.ptr;
        }

        return alloc_new(block_size);
    }

    void* alloc_new(size_t block_size) {
        size_t binary_size = 1 << bits(block_size);
        if (top_memory + binary_size > memory + size)
            return nullptr; // Not enough memory

        Block new_block{top_memory + binary_size, binary_size};
        used[new_block.ptr] = new_block;
        top_memory += binary_size;

        return new_block.ptr;
    }

    void* free(void *block_ptr) override {
        if (!used.contains(block_ptr))
            return nullptr;

        Block b = used[block_ptr];
        used.erase(block_ptr);
        free_blocks[b.size].push_back(b);

        return block_ptr;
    }

    int bits(size_t a) {
        int res = 0;
        while (a > 0) {
            a >>= 1;
            res++;
        }
        return res;
    }
};
