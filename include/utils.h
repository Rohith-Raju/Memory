#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

struct Utils {
  static size_t alignment(size_t need, size_t multiple) {
    size_t push = need + (multiple - 1);
    size_t mask = ~(multiple - 1);
    return push & mask;
  }
};

#endif // UTILS_H
