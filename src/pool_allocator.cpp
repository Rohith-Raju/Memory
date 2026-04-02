#include "pool_allocator.h"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <sys/mman.h>

PoolMemory::PoolMemory(std::byte *start_memory, FreeNode *free_list)
    : start_memory(start_memory), free_list(free_list) {}

PoolMemory *PoolMemory::init(size_t blocks, uint8_t size_per_block) {
  size_t actual_block_size =
      std::max((size_t)sizeof(FreeNode), (size_t)size_per_block);
  actual_block_size = (actual_block_size + 7) & ~7;

  std::size_t total_data_size = blocks * actual_block_size;

  std::size_t total_need = sizeof(PoolMemory) + total_data_size;

  std::byte *memory =
      (std::byte *)mmap(nullptr, total_data_size, PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANON, -1, 0);

  std::byte *start = memory + sizeof(PoolMemory);
  std::byte *head = start;
  std::byte *current = start;
  for (int i = 1; i < size_per_block; i++) {
    std::byte *addr = head + (i * actual_block_size);
    new (current) FreeNode{addr};
    current = addr;
  }

  std::byte *last_ptr = current - actual_block_size;
  FreeNode *last = (FreeNode *)last_ptr;
  last->next = nullptr;
  return new (memory) PoolMemory(start, (FreeNode *)head);
}

void PoolMemory::print_stats() {
  std::cout << "Size of one block : " << free_list->next - start_memory
            << std::endl;
}
