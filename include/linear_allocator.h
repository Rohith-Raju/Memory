#ifndef LINEAR
#define LINEAR

#include "utils.h"
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <new>
#include <util.h>

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

  template <typename T, typename... Args> T *assign(Args... args) {

    std::size_t size = sizeof(T);
    std::size_t req = alignof(T);

    uintptr_t curr_addr = (uintptr_t)current;
    uintptr_t aligned_addr = Utils::alignment(curr_addr, req);

    if ((std::byte *)aligned_addr + size > soft_end) {
      std::cerr << "Out of memory!" << std::endl;
      std::cerr << "Memory Wasted: " << hard_end - soft_end << std::endl;
      exit(1);
    }

    current = (std::byte *)(aligned_addr + size);

    return new ((void *)aligned_addr) T(args...);
  }

  void print_stats();

  bool free();
};
#endif // LINEAR
