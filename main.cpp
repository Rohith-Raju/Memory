#include <cstddef>
#include <iostream>
#include <pool_allocator.h>

class Entity {
public:
  Entity(int a, int b, int c) : a{a}, b{b}, c{c} {};
  void printEntity() {
    std::cout << "Value of a and b are :" << a << " " << b << " " << c
              << std::endl;
  }

private:
  int a;
  int b;
  int c;
};

int main() {
  PoolMemory *pool = PoolMemory::init(1);
  Entity *entity = pool->assign<Entity>(1, 2, 4);
  Entity *entity1 = pool->assign<Entity>(1, 3, 4);
  entity->printEntity();
  pool->free(entity);
  delete pool;
}
