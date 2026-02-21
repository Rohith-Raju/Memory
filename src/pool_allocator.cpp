#include "pool_allocator.h"
#include <iostream>
#include <sys/mman.h>

PoolMemory::PoolMemory(std::byte *start_memory, std::byte *current,
                       std::byte *free_list)
    : start_memory(start_memory), current(current), free_list(free_list) {}

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

  std::byte *start = memory + sizeof(mem_size);

  for (int i = 0; i < block_size; i++) {
  }
}
