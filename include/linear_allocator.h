#ifndef LINEAR
#define LINEAR

#include "utils.h"
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <new>

class LinearMemory {
private:
  std::byte *map_base_;
  std::size_t map_bytes_;
  std::byte *bump_start_;
  std::byte *current;
  std::byte *soft_end;
  std::byte *hard_end;
  bool released_;
  LinearMemory(std::byte *map_base, std::size_t map_bytes, std::byte *bump_start,
               std::byte *current, std::byte *soft_end, std::byte *hard_end);

public:
  ~LinearMemory();

  static LinearMemory *init(std::size_t mem_size,
                            std::size_t alignment = sizeof(std::max_align_t));

  template <typename T, typename... Args> T *assign(Args... args) {

    std::size_t size = sizeof(T);
    std::size_t req = alignof(T);

    uintptr_t curr_addr = (uintptr_t)current;
    uintptr_t aligned_addr = Utils::alignment(curr_addr, req);

    if ((std::byte *)aligned_addr + size > soft_end) {
      throw std::bad_alloc();
    }

    current = (std::byte *)(aligned_addr + size);

    return new ((void *)aligned_addr) T(args...);
  }

  void print_stats();

  bool free();
};
#endif // LINEAR
