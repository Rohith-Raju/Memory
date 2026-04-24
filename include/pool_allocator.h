#ifndef POOL
#define POOL

#include <cstddef>
#include <cstdint>
#include <cstdlib>
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

  template <typename Entity, typename... Args> Entity *assign(Args... args) {
    if (sizeof(Entity) > actual_block_size) {
      std::cout << "Entity bigger than block size exiting...";
      exit(1);
    }
    FreeNode *node = free_list;
    free_list = free_list->next;
    return new ((void *)node) Entity(args...);
  }
  void print_stats();

  template <typename T> void free(T *ptr) {
    delete ptr;
    FreeNode *reclaimed = reinterpret_cast<FreeNode *>(ptr);
    reclaimed->next = free_list;
    free_list = reclaimed;
  }
};

#endif // POOL
