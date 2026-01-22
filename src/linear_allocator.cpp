#include "linear_allocator.h"

/*
class LinearMemory {
public:
  char *memory;
  unsigned int offset;
  LinearMemory(char *mem) : memory(mem), offset(8) {}
};

template <typename T> bool mount_mmu(T, void *raw_mem) {
  int class_size = sizeof(T);
}

int main() {
  char *memory = (char *)mmap(nullptr, MEMORY_SIZE, PROT_READ | PROT_WRITE,
                              MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  if (memory == MAP_FAILED) {
    std::cout << "Map Failed" << std::endl;
  } else {
    std::cout << "Map Succeeded" << std::endl;
  }

  std::cout << "started at: " << (void *)memory << std::endl;

  LinearMemory *some = new (memory) LinearMemory(memory + sizeof(LinearMemory));

  std::cout << "currently pointed at: " << (void *)some->memory << std::endl;

  int unmapped = munmap(memory, MEMORY_SIZE);

  if (unmapped == -1) {
    std::cout << "Unmap failed" << std::endl;
  } else {
    std::cout << "Unmap completed" << std::endl;
  }
}
*/
