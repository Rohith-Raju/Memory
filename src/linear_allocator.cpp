#include "linear_allocator.h"
#include <iostream>
#include <new>
#include <sys/mman.h>

LinearMemory::LinearMemory(std::byte *memory_start, std::byte *current,
                           std::byte *soft_end, std::byte *hard_end)
    : memory_start(memory_start), current(current), soft_end(soft_end),
      hard_end(hard_end) {}

void LinearMemory::init(std::size_t mem_size, std::size_t allignment) {
  std::byte *memory =
      (std::byte *)mmap(nullptr, mem_size, PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  std::cout << "Memory assined at" << (void *)memory;

  std::byte *start = sizeof(LinearMemory) + memory;
  new (memory) LinearMemory(start, start, start + mem_size, start + mem_size);
}

void LinearMemory::print_stats() {
  std::cout << "starting address: " << (void *)memory_start << std::endl;
  std::cout << "current address : " << (void *)current << std::endl;
  std::cout << "soft_end address : " << (void *)soft_end << std::endl;
  std::cout << "hard_end address : " << (void *)hard_end << std::endl;
}

template <typename T> void *assign(T obj) {
  std::size_t class_size = sizeof(T);
}
