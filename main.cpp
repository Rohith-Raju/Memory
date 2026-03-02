#include <cstddef>
#include <iostream>
#include <linear_allocator.h>

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
  auto mem = LinearMemory::init(100);
  Entity *entity = mem->assign<Entity>(10, 20, 30);
  mem->print_stats();
}
