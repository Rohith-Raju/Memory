#include "linear_allocator.h"
#include "utils.h"
#include <cstddef>
#include <iostream>
#include <sys/mman.h>
#include <unistd.h>

LinearMemory::LinearMemory(std::byte *memory_start, std::byte *current,
                           std::byte *soft_end, std::byte *hard_end)
    : memory_start(memory_start), current(current), soft_end(soft_end),
      hard_end(hard_end) {}

LinearMemory::~LinearMemory() { this->free(); }

LinearMemory *LinearMemory::init(std::size_t mem_size,
                                 std::size_t default_allignment) {

  std::size_t page_size = (std::size_t)sysconf(_SC_PAGE_SIZE);
  std::size_t total_need = sizeof(LinearMemory) + mem_size;

  std::cout << "Your requested memory size (in bytes): " << mem_size
            << std::endl;
  std::cout << "Total size needed (LinearMemory + your needs): " << total_need
            << std::endl;
  std::cout << "Size of one page on your machine (in bytes): " << page_size
            << std::endl;
  std::size_t aligned = Utils::alignment(total_need, page_size);
  std::cout << "Memory alligned to (in bytes): " << aligned << std::endl;

  std::byte *memory =
      (std::byte *)mmap(nullptr, aligned, PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  std::byte *start = sizeof(LinearMemory) + memory;

  return new (memory)
      LinearMemory(memory, start, start + mem_size, memory + aligned);
}

void LinearMemory::print_stats() {
  std::cout << "Raw memory starting address: " << (void *)memory_start
            << std::endl;
  std::cout << "Linear memory starts from: " << (void *)memory_start
            << " and ends at " << (void *)current << std::endl;

  std::cout << "Actual sizeof Linear memory: " << sizeof(LinearMemory)
            << std::endl;
  std::cout << "Bytes allocated in memory for LinearMemory: "
            << static_cast<std::size_t>(current - memory_start) << std::endl;
  std::cout << "Difference between hard_end and soft_end(in bytes): "
            << (hard_end - soft_end);
}

bool LinearMemory::free() {
  return !munmap(memory_start, (hard_end - memory_start));
}
