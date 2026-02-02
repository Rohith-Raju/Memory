#include <iostream>
#include <linear_allocator.h>

int main() {

  auto mem = LinearMemory::init(20000);
  mem->print_stats();
}
