#ifndef POOL
#define POOL

#include <cstddef>
#include <cstdint>

class PoolMemory {
private:
  std::byte *start_memory;
  std::byte *current;
  std::byte *free_list;
  PoolMemory(std::byte *, std::byte *, std::byte *);
  ~PoolMemory();

public:
  static PoolMemory *init(size_t = 1024 * 1024, uint8_t = 64);

  template <typename T, typename... Args> T *assign(Args... args);

  void print_stats();

  bool free();
};

#endif // POOL
