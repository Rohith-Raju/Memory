#include "linear_allocator.h"
#include <iostream>
#include <sys/mman.h>

int MEMORY_SIZE = 100;

class LinearMemory {
public:
  void *memory;
  unsigned int offset;

  LinearMemory(void *mem) : memory(mem), offset(0) {}
};

template <typename T> bool mount_mmu(void *raw_mem) {
  int class_size = sizeof(T);
}

int main() {
  void *memory = mmap(nullptr, MEMORY_SIZE, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  if (memory == MAP_FAILED) {
    std::cout << "Map Failed" << std::endl;
  } else {
    std::cout << "Map Succeeded" << std::endl;
  }

  int unmapped = munmap(memory, MEMORY_SIZE);

  if (unmapped == -1) {
    std::cout << "Unmap failed" << std::endl;
  } else {
    std::cout << "Unmap completed" << std::endl;
  }
}
