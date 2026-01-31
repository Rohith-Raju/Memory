#include "linear_allocator.h"
#include <cstddef>
#include <iostream>
#include <sys/mman.h>

// TODO: Need the next possoble address based on alignment
std::size_t alignment(std::size_t need, std::size_t multiple) {
  int push = need + (multiple - 1);
  int mask = ~(multiple - 1);
  return push & mask;
}

LinearMemory::LinearMemory(std::byte *memory_start, std::byte *current,
                           std::byte *soft_end, std::byte *hard_end)
    : memory_start(memory_start), current(current), soft_end(soft_end),
      hard_end(hard_end) {}

LinearMemory::~LinearMemory() { this->free(); }

LinearMemory *LinearMemory::init(std::size_t mem_size,
                                 std::size_t default_allignment) {

  std::cout << "Your requested memory size (in bytes): " << mem_size
            << std::endl;
  std::size_t aligned = alignment(mem_size, default_allignment);
  std::cout << "Memory alligned to (in bytes): " << aligned << std::endl;

  // TODO: Get the actual end of the memory. like how much it assinged.
  std::byte *memory =
      (std::byte *)mmap(nullptr, aligned, PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  std::byte *start = sizeof(LinearMemory) + memory;
  return new (memory)
      LinearMemory(memory, start, start + mem_size, start + aligned);
}

void LinearMemory::print_stats() {
  std::cout << "Raw memory starting address: " << (void *)memory_start
            << std::endl;
  std::cout << "Linear memory starts from: " << (void *)memory_start
            << " and ends at" << (void *)current << std::endl;

  std::cout << "Actual sizeof Linear memory: " << sizeof(LinearMemory)
            << std::endl;
  std::cout << "Bytes allocated in memory for LinearMemory: "
            << static_cast<std::size_t>(current - memory_start) << std::endl;
  std::cout << "soft_end address: " << (void *)soft_end << std::endl;
  std::cout << "hard_end address: " << (void *)hard_end << std::endl;
}

template <typename T> void *assign(T obj) {
  std::size_t class_size = sizeof(T);
}

bool LinearMemory::free() {
  return !munmap(memory_start, (hard_end - memory_start));
}
