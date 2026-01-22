#include <cstddef>
#include <sys/mman.h>

class LinearMemory {
private:
  std::byte *memory;
  unsigned int offset;
  LinearMemory(std::byte memory, unsigned int offset);

public:
  void init(std::size_t mem_size,
            std::size_t allignment = sizeof(std::max_align_t));

  template <typename T> bool assign(T obj);

  void print();

  bool free();
};
