#include "linear_allocator.h"
#include <new>
#include <sys/mman.h>

LinearMemory::LinearMemory(std::byte *memory, unsigned int offset)
    : memory(memory_start), offset(offset) {}

auto LinearMemory::init(std::size_t mem_size, std::size_t allignment) {
  std::byte *memory =
      (std::byte *)mmap(nullptr, mem_size, PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  return new (memory) LinearMemory(sizeof(LinearMemory) + memory, 8);
}
