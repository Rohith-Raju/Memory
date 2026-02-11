#include <cstddef>
#include <iostream>
#include <linear_allocator.h>

class Entity {
public:
  Entity(int a, int b) : a{a}, b{b} {};
  void printEntity() {
    std::cout << "Value of a and b are :" << a << " " << b << std::endl;
  }

private:
  int a;
  int b;
};

int main() {
  auto mem = LinearMemory::init(5);
  Entity *entity = mem->assign<Entity>(10, 20);
  mem->print_stats();
}
