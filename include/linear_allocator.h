#include <cstddef>

class LinearMemory {
private:
  std::byte *memory_start;
  std::byte *current;
  std::byte *soft_end;
  std::byte *hard_end;
  LinearMemory(std::byte *memory, unsigned int offset);

public:
  LinearMemory *init(std::size_t mem_size,
                     std::size_t allignment = sizeof(std::max_align_t));

  template <typename T> bool assign(T obj);

  void print();

  bool free();
};

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
