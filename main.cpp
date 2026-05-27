#include <cstddef>
#include <iostream>
#include <pool_allocator.h>
#include <linear_allocator.h>

struct Vector3 {
  float x, y, z;
  Vector3(float x, float y, float z) : x{x}, y{y}, z{z} {}
  void print() const {
    std::cout << "Vector3(" << x << ", " << y << ", " << z << ")\n";
  }
};

struct Enemy {
  int hp;
  Vector3 pos;
  Enemy(int hp, float x, float y, float z) : hp{hp}, pos{x, y, z} {}
  void print() const {
    std::cout << "Enemy(hp=" << hp << ", pos=)";
    pos.print();
  }
};

void demo_pool() {
  std::cout << "=== Pool Allocator Demo ===\n";

  PoolMemory *pool = PoolMemory::init(3, 64);
  pool->print_stats();

  auto *e1 = pool->assign<Enemy>(100, 0.0f, 0.0f, 0.0f);
  auto *e2 = pool->assign<Enemy>(200, 5.0f, 0.0f, 0.0f);
  auto *e3 = pool->assign<Enemy>(300, 10.0f, 0.0f, 0.0f);

  e1->print();
  e2->print();
  e3->print();
  pool->print_stats();

  std::cout << "\nFreeing enemy 1...\n";
  pool->free(e1);
  pool->print_stats();

  std::cout << "Reusing freed slot:\n";
  auto *e4 = pool->assign<Enemy>(999, 99.0f, 99.0f, 99.0f);
  e4->print();
  pool->print_stats();

  pool->free(e2);
  pool->free(e3);
  pool->free(e4);
  delete pool;
}

void demo_linear() {
  std::cout << "\n=== Linear Allocator Demo ===\n";

  LinearMemory *arena = LinearMemory::init(4096);
  arena->print_stats();

  auto *points = arena->assign<Vector3>(0, 0, 0);
  auto *count = arena->assign<int>(42);
  auto *name = arena->assign<char>('Z');

  std::cout << "Allocated: Vector3 at " << points
            << ", int at " << count
            << ", char at " << static_cast<void *>(name) << "\n";
  std::cout << "Values: *count=" << *count << ", *name=" << *name << "\n";
  points->print();
  arena->print_stats();

  std::cout << "\nReleasing linear arena (single munmap)...\n";
  delete arena;
  std::cout << "Released.\n";
}

int main() {
  demo_pool();
  demo_linear();
  return 0;
}
