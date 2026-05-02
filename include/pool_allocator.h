#ifndef POOL
#define POOL

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <new>
#include <stdexcept>

struct FreeNode {
  FreeNode *next;
};
class PoolMemory {
private:
  FreeNode *free_list;
  size_t actual_block_size;
  std::byte *map_base_;
  std::size_t map_bytes_;
  PoolMemory(FreeNode *free_list, std::size_t actual_block_size,
             std::byte *map_base, std::size_t map_bytes);

public:
  ~PoolMemory();

  static PoolMemory *init(size_t = 1024, uint8_t = 64);

  template <typename Entity, typename... Args> Entity *assign(Args... args) {
    if (sizeof(Entity) > actual_block_size) {
      std::cerr << "Entity bigger than block size\n";
      throw std::length_error("PoolMemory::assign: type larger than block");
    }
    if (!free_list) {
      throw std::bad_alloc();
    }
    FreeNode *node = free_list;
    free_list = free_list->next;
    return new ((void *)node) Entity(args...);
  }
  void print_stats();

  template <typename T> void free(T *ptr) {
    if (!ptr) {
      return;
    }
    ptr->~T();
    auto *reclaimed = reinterpret_cast<FreeNode *>(ptr);
    reclaimed->next = free_list;
    free_list = reclaimed;
  }
};

#endif // POOL
