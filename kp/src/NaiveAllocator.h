#pragma once

#include <vector>
#include <cstddef>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <list>
#include <sys/mman.h>
#include "AllocatorBase.h"

const int alignment = alignof(std::max_align_t);

const int MIN_SPLIT_NEW_BLOCK_SIZE = 100;

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
        for (size_t i = 0; i < blocks.size(); ++i) {
            if (blocks[i].is_free && blocks[i].size >= block_size) {
                if (blocks[i].size > block_size + MIN_SPLIT_NEW_BLOCK_SIZE) { // Если остаток достаточно большой для нового блока
                    split_block(i, block_size);
                }

                blocks[i].is_free = false;
                return blocks[i].ptr;
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
        for (size_t i = 0; i < blocks.size(); ++i) {
            if (blocks[i].ptr == block_ptr) {
                blocks[i].is_free = true;
                // Проверяем соседние блоки
                merge_with_neighbors(i);
                break;
            }
        }

        return block_ptr;
    }

private:
    void split_block(size_t block_index, size_t block_size) {
        // Создаем новый свободный блок из оставшейся части
        Block new_block{
            reinterpret_cast<char*>(blocks[block_index].ptr) + block_size,
            blocks[block_index].size - block_size,
            true
        };
        blocks[block_index].size = block_size;
        blocks.insert(blocks.begin() + block_index + 1, new_block);
    }

    void merge_with_neighbors(size_t index) {
        // Проверяем блок справа
        if (index + 1 < blocks.size() && blocks[index + 1].is_free) {
            blocks[index].size += blocks[index + 1].size; // Увеличиваем размер текущего блока
            blocks.erase(blocks.begin() + (index + 1)); // Удаляем следующий блок
        }
        // Проверяем блок слева (если не первый)
        if (index > 0 && blocks[index - 1].is_free) {
            blocks[index - 1].size += blocks[index].size; // Увеличиваем размер блока слева
            blocks.erase(blocks.begin() + index); // Удаляем текущий блок
            // Нет необходимости уменьшать index, так как продолжаем с текущей позиции
        }
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
