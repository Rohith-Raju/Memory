#ifndef POOL
#define POOL

#include <cstddef>
#include <cstdint>

class PoolMemory {
private:
  size_t block_size;
  void *start_memory;
  void *free_list;
  PoolMemory(size_t);
  ~PoolMemory();

public:
  static PoolMemory *init(size_t, uint8_t);

  template <typename T, typename... Args> T *assign(Args... args);

  void print_stats();

  bool free();
};

#endif // POOL
