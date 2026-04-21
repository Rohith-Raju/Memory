#ifndef POOL
#define POOL

#include <cstddef>
#include <cstdint>
#include <iostream>

struct FreeNode {
  FreeNode *next;
};
class PoolMemory {
private:
  std::byte *start_memory;
  FreeNode *free_list;
  size_t actual_block_size;
  PoolMemory(std::byte *, FreeNode *, std::size_t);
  ~PoolMemory();

public:
  static PoolMemory *init(size_t = 1024 * 1024, uint8_t = 64);

  template <typename Entity, typename... Args>
  Entity *PoolMemory::*asign(Args... args) {
    if (sizeof(Entity) > actual_block_size)
      std::cout << "Entity bigger than block size";
    FreeNode *node = free_list;
    free_list = free_list->next;
    return new (node) Entity(args...);
  }
  void print_stats();

  bool free();
};

#endif // POOL
