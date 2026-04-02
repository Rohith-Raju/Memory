#ifndef POOL
#define POOL

#include <cstddef>
#include <cstdint>
#include <iostream>

struct FreeNode {
  std::byte *next;
};
class PoolMemory {
private:
  std::byte *start_memory;
  FreeNode *free_list;
  PoolMemory(std::byte *, FreeNode *);
  ~PoolMemory();

public:
  static PoolMemory *init(size_t = 1024 * 1024, uint8_t = 64);

  template <typename T, typename... Args>
  T PoolMemory::*assign(T entity, Args... args) {
    uint8_t block_size = free_list->next - start_memory;
    if (sizeof(entity) > block_size)
      std::cout << "Entity bigger than block size";
  }
  void print_stats();

  bool free();
};

#endif // POOL
