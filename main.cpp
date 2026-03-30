#include <cstddef>
#include <iostream>
#include <pool_allocator.h>

class Entity {
public:
  Entity(int a, int b, int c) : a{a}, b{b}, c{c} {};
  void printEntity() {
    std::cout << "Value of a and b are :" << a << " " << b << std::endl;
  }

private:
  int a;
  int b;
  int c;
};

int main() {
  PoolMemory *pool = PoolMemory::init(100);
  pool->print_stats();
}
