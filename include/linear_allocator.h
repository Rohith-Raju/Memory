#include <cstddef>

class LinearMemory {
private:
  std::byte *memory_start;
  std::byte *current;
  std::byte *soft_end;
  std::byte *hard_end;
  LinearMemory(std::byte *memory_start, std::byte *current, std::byte *soft_end,
               std::byte *hard_end);

  ~LinearMemory();

public:
  static LinearMemory *init(std::size_t mem_size,
                            std::size_t allignment = sizeof(std::max_align_t));

  template <typename T> void *assign(T obj);

  void print_stats();

  bool free();
};
