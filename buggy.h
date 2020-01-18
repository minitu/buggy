#pragma once

#include <cstdio>
#include <cstdlib>
#include <list>
#include <cmath>
#include <unordered_map>
#include <algorithm>
#include <mutex>

namespace buggy {

#ifdef DEBUG
#define DEBUG_PRINT(...) printf("[Buggy] " __VA_ARGS__)
#else
#define DEBUG_PRINT(...) do {} while (0)
#endif

  struct allocator {
    // Allocated block
    struct Block {
      uint8_t* ptr;
      size_t size;

      Block() : ptr(nullptr), size(0) {}

      Block(uint8_t* ptr, size_t size) {
        this->ptr = ptr;
        this->size = size;
      }
    };

    // Allocation limits
    size_t limit;
    int limit_log2;
    const size_t min_alloc;

    // Base pointer of the initial allocation
    uint8_t* base_ptr;

    // Mutex for thread-safe access
    std::mutex mutex;

    // Buckets each with a free list
    std::list<Block>* buckets;
    int bucket_count;

    // Map of allocated blocks
    std::unordered_map<uint8_t*, size_t> alloc_map;

    /* --------------------- */
    /* | Utility functions | */
    /* --------------------- */

    void print_status() {
      printf("(buckets)\n");
      for (int i = 0; i < bucket_count; i++) {
        printf("bucket[%d]: ", i);
        for (const auto& block : buckets[i]) {
          printf("{%p, %lu} ", block.ptr, block.size);
        }
        printf("\n");
      }

      printf("(alloc_map)\n");
      for (const auto& elem : alloc_map) {
        printf("ptr: %p, size: %lu\n", elem.first, elem.second);
      }
    }

    int get_bucket(size_t size) {
      return (int)std::ceil(std::log2((double)size)) - 2;
    }

    /* ------------------------ */
    /* | Allocation functions | */
    /* ------------------------ */

    allocator(size_t size = 1 << 26) : min_alloc(4), base_ptr(NULL) {
      // Request GPU memory (closest power of 2)
      limit_log2 = std::ceil(std::log2((double)size));
      limit = (size_t)std::pow(2, limit_log2);
      cudaMalloc(&base_ptr, limit);
      DEBUG_PRINT("Initialized base_ptr %p with %lu bytes\n", (void*)base_ptr, limit);

      // Initialize buckets and set up last bucket (for size min_alloc)
      bucket_count = limit_log2 - 1;
      buckets = new std::list<Block>[bucket_count];
      buckets[bucket_count-1].emplace_back(base_ptr, limit);
    }

    ~allocator() {
      cudaFree(base_ptr);
      delete[] buckets;
    }

    void* malloc(size_t request) {
      const std::lock_guard<std::mutex> lock(mutex);

      // Must be initialized
      if (!base_ptr) return nullptr;

      int bucket = get_bucket(request);
      int original_bucket = bucket;

      // Find an empty bucket
      while (buckets[bucket].empty() && bucket < bucket_count) {
        bucket++;
      }

      // All buckets were empty?
      if (bucket == bucket_count) {
        DEBUG_PRINT("No free blocks, malloc request %lu\n", request);
        return nullptr;
      }

      // Found bucket with free block, take it and start splitting if needed
      Block& block = buckets[bucket].back();
      uint8_t* ptr = block.ptr;
      size_t size = block.size;
      buckets[bucket].pop_back();

      while (bucket-- > original_bucket) {
        buckets[bucket].emplace_back(ptr, size / 2);

        ptr += size / 2;
        size /= 2;
      }

      // Store allocation info
      alloc_map.insert({ptr, size});

      DEBUG_PRINT("Allocated ptr %p (base_ptr + %lu) with %lu bytes\n",
          (void*)ptr, (size_t)(ptr - base_ptr), size);

#ifdef DEBUG
      print_status();
#endif

      return ptr;
    }

    void free(void* ptr) {
      const std::lock_guard<std::mutex> lock(mutex);

      // Find pointer in allocation map
      auto search = alloc_map.find((uint8_t*)ptr);
      if (search == alloc_map.end()) {
        fprintf(stderr, "Free invalid pointer: %p\n", ptr);
        std::abort();
      }

      size_t size = search->second;
      int bucket = get_bucket(size);

      // Add to free list
      buckets[bucket].emplace_back((uint8_t*)ptr, size);
    }
  }; // struct allocator

} // namespace buggy
