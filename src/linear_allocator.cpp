#include "linear_allocator.h"
#include "utils.h"
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <new>
#include <stdexcept>
#include <system_error>
#include <sys/mman.h>
#include <unistd.h>

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

LinearMemory::LinearMemory(std::byte *map_base, std::size_t map_bytes,
                           std::byte *bump_start, std::byte *current,
                           std::byte *soft_end, std::byte *hard_end)
    : map_base_(map_base), map_bytes_(map_bytes), bump_start_(bump_start),
      current(current), soft_end(soft_end), hard_end(hard_end),
      released_(false) {}

LinearMemory::~LinearMemory() { free(); }

LinearMemory *LinearMemory::init(std::size_t mem_size,
                                 std::size_t alignment) {
  if (alignment == 0 || (alignment & (alignment - 1)) != 0) {
    throw std::invalid_argument(
        "LinearMemory::init: alignment must be a power of two and non-zero");
  }

  std::size_t page_size = (std::size_t)sysconf(_SC_PAGE_SIZE);
  if (page_size == 0 || (page_size & (page_size - 1)) != 0) {
    throw std::runtime_error("LinearMemory::init: invalid page size");
  }

  // Worst-case bytes skipped before the first alignment boundary at map_base.
  const std::size_t align_slack = alignment > 1 ? alignment - 1 : 0;
  const std::size_t total_need = mem_size + align_slack;

  std::cout << "Your requested memory size (in bytes): " << mem_size
            << std::endl;
  std::cout << "Minimum mapping (arena + worst-case align slack): " << total_need
            << std::endl;
  std::cout << "Size of one page on your machine (in bytes): " << page_size
            << std::endl;
  std::size_t map_bytes = Utils::alignment(total_need, page_size);
  std::cout << "Memory aligned to (in bytes): " << map_bytes << std::endl;

  void *raw = mmap(nullptr, map_bytes, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (raw == MAP_FAILED) {
    throw std::system_error(errno, std::generic_category(),
                            "LinearMemory::init mmap");
  }
  auto *map_base = static_cast<std::byte *>(raw);

  std::uintptr_t bump0 =
      Utils::alignment(reinterpret_cast<std::uintptr_t>(map_base), alignment);
  auto *start = reinterpret_cast<std::byte *>(bump0);

  if (start + mem_size > map_base + map_bytes) {
    munmap(map_base, map_bytes);
    throw std::runtime_error(
        "LinearMemory::init: internal error, mapped region too small");
  }

  try {
    return new LinearMemory(map_base, map_bytes, start, start, start + mem_size,
                            map_base + map_bytes);
  } catch (...) {
    munmap(map_base, map_bytes);
    throw;
  }
}

void LinearMemory::print_stats() {
  std::cout << "Mapping base: " << static_cast<void *>(map_base_) << " size "
            << map_bytes_ << std::endl;
  std::cout << "Bump range: " << static_cast<void *>(bump_start_) << " .. "
            << static_cast<void *>(current) << " (cursor)" << std::endl;
  std::cout << "Soft arena end: " << static_cast<void *>(soft_end)
            << ", mapping end: " << static_cast<void *>(hard_end) << std::endl;
  std::cout << "sizeof(LinearMemory): " << sizeof(LinearMemory) << std::endl;
  std::cout << "alignof(LinearMemory): " << alignof(LinearMemory) << std::endl;
  std::cout << "Bytes used from bump start: "
            << static_cast<std::size_t>(current - bump_start_) << std::endl;
  std::cout << "Slack after soft_end (bytes): "
            << static_cast<std::size_t>(hard_end - soft_end) << std::endl;
}

bool LinearMemory::free() {
  if (released_) {
    return true;
  }
  const int st = munmap(map_base_, map_bytes_);
  released_ = true;
  return st == 0;
}
