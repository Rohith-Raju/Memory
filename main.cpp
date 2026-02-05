#include <linear_allocator.h>

class Entity {
public:
  Entity(int a, int b) : a{a}, b{b} {};

private:
  int a;
  int b;
};

int main() {
  auto mem = LinearMemory::init(20000);
  mem->assign<Entity>(10, 20);
}
