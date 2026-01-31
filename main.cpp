#include <iostream>
#include <linear_allocator.h>

int main() {

  auto mem = LinearMemory::init(1);
  mem->print_stats();
}
