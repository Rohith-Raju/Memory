#include "pool_allocator.h"
#include <iostream>
#include <sys/mman.h>

PoolMemory::PoolMemory(std::byte *start_memory, FreeNode *free_list)
    : start_memory(start_memory), free_list(free_list) {}

PoolMemory *PoolMemory::init(size_t mem_size, uint8_t block_nums) {
  int remainder = mem_size % block_nums;
  size_t block_size = (mem_size + remainder) / block_nums;

  std::size_t total_need = sizeof(PoolMemory) + mem_size;
  std::cout << "Your requested memory size (in bytes): " << mem_size
            << std::endl;
  std::cout << "Total size needed (LinearMemory + your needs): "
            << sizeof(PoolMemory) << " + " << mem_size << " = " << total_need
            << std::endl;

  std::byte *memory =
      (std::byte *)mmap(nullptr, mem_size + remainder, PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANON, -1, 0);

  std::byte *start = memory + sizeof(PoolMemory);
  std::byte *head = start;
  std::byte *current = start;
  for (int i = 1; i < block_nums; i++) {
    std::byte *addr = head + (i * block_size);
    new (current) FreeNode{addr};
    current = addr;
  }

  std::byte *last_ptr = current - block_size;
  FreeNode *last = (FreeNode *)last_ptr;
  last->next = nullptr;
  return new (PoolMemory)(start, (FreeNode *)head);
}

void PoolMemory::print_stats() {
  FreeNode *curr = free_list;

  // * difference = std::byte
  // std::cout<<"Distance between head and the next pointer is : "<<
}
