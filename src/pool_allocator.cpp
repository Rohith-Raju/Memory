#include "pool_allocator.h"
#include <algorithm>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <new>
#include <stdexcept>
#include <sys/mman.h>
#include <system_error>

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

PoolMemory::PoolMemory(FreeNode *free_list, size_t actual_block_size,
                       std::byte *map_base, std::size_t map_bytes)
    : free_list{free_list}, actual_block_size{actual_block_size},
      map_base_{map_base}, map_bytes_{map_bytes} {}

PoolMemory::~PoolMemory() {
  if (map_base_ && map_bytes_) {
    munmap(map_base_, map_bytes_);
    map_base_ = nullptr;
    map_bytes_ = 0;
  }
}

PoolMemory *PoolMemory::init(size_t blocks, uint8_t size_per_block) {
  if (blocks == 0) {
    throw std::invalid_argument("PoolMemory::init: blocks must be > 0");
  }

  size_t actual_block_size =
      std::max((size_t)sizeof(FreeNode), (size_t)size_per_block);
  actual_block_size = (actual_block_size + 7) & ~7;

  const std::size_t map_bytes = blocks * actual_block_size;

  void *raw = mmap(nullptr, map_bytes, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (raw == MAP_FAILED) {
    throw std::system_error(errno, std::generic_category(),
                            "PoolMemory::init mmap");
  }
  auto *arena = static_cast<std::byte *>(raw);

  FreeNode *free_head = reinterpret_cast<FreeNode *>(arena);
  for (std::size_t i = 0; i <= blocks; ++i) {
    std::byte *block = arena + i * actual_block_size;
    std::byte *next_block = arena + (i + 1) * actual_block_size;
    new (block) FreeNode{reinterpret_cast<FreeNode *>(next_block)};
  }
  std::byte *last_block = arena + (blocks - 1) * actual_block_size;
  new (last_block) FreeNode{nullptr};

  try {
    return new PoolMemory(free_head, actual_block_size, arena, map_bytes);
  } catch (...) {
    munmap(arena, map_bytes);
    throw;
  }
}

void PoolMemory::print_stats() {
  std::cout << "Block size (bytes): " << actual_block_size << std::endl;
  std::size_t free_count = 0;
  for (FreeNode *n = free_list; n != nullptr; n = n->next) {
    ++free_count;
  }
  std::cout << "Free blocks: " << free_count << std::endl;
}
