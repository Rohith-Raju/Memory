#include "linear_allocator.h"
#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <cstring>

struct Point {
  double x;
  double y;
  Point(double x, double y) : x{x}, y{y} {}
};

struct CharArray {
  char data[128];
};

struct Align16 {
  alignas(16) int val;
};

class LinearMemoryTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(LinearMemoryTest, InitAndDestroy) {
  LinearMemory *arena = LinearMemory::init(4096);
  ASSERT_NE(arena, nullptr);
  delete arena;
}

TEST_F(LinearMemoryTest, AssignSingleObject) {
  LinearMemory *arena = LinearMemory::init(4096);

  auto *p = arena->assign<Point>(1.0, 2.0);
  ASSERT_NE(p, nullptr);
  EXPECT_DOUBLE_EQ(p->x, 1.0);
  EXPECT_DOUBLE_EQ(p->y, 2.0);

  delete arena;
}

TEST_F(LinearMemoryTest, AssignMultipleObjects) {
  LinearMemory *arena = LinearMemory::init(4096);

  auto *a = arena->assign<int>(42);
  auto *b = arena->assign<double>(3.14159);
  auto *c = arena->assign<Point>(1.0, 2.0);

  ASSERT_NE(a, nullptr);
  ASSERT_NE(b, nullptr);
  ASSERT_NE(c, nullptr);

  EXPECT_EQ(*a, 42);
  EXPECT_DOUBLE_EQ(*b, 3.14159);
  EXPECT_DOUBLE_EQ(c->x, 1.0);
  EXPECT_DOUBLE_EQ(c->y, 2.0);

  delete arena;
}

TEST_F(LinearMemoryTest, BumpPointerAdvances) {
  LinearMemory *arena = LinearMemory::init(1024);

  auto *first = arena->assign<int>(1);
  auto *second = arena->assign<int>(2);

  ASSERT_NE(first, nullptr);
  ASSERT_NE(second, nullptr);

  // second should be at a higher address than first
  EXPECT_GT(reinterpret_cast<uintptr_t>(second),
            reinterpret_cast<uintptr_t>(first));
  EXPECT_EQ(*first, 1);
  EXPECT_EQ(*second, 2);

  delete arena;
}

TEST_F(LinearMemoryTest, InvalidationOnBadAlignment) {
  EXPECT_THROW(LinearMemory::init(1024, 0), std::invalid_argument);
  EXPECT_THROW(LinearMemory::init(1024, 3), std::invalid_argument);
  EXPECT_THROW(LinearMemory::init(1024, 5), std::invalid_argument);
}

TEST_F(LinearMemoryTest, AlignmentRespected) {
  LinearMemory *arena = LinearMemory::init(4096, 64);

  auto *obj = arena->assign<Align16>();
  ASSERT_NE(obj, nullptr);
  EXPECT_EQ(reinterpret_cast<uintptr_t>(obj) % 16, 0);

  delete arena;
}

TEST_F(LinearMemoryTest, PrintStatsDoesNotCrash) {
  LinearMemory *arena = LinearMemory::init(1024);
  EXPECT_NO_THROW(arena->print_stats());

  arena->assign<int>(1);
  arena->assign<double>(2.0);
  EXPECT_NO_THROW(arena->print_stats());

  delete arena;
}

TEST_F(LinearMemoryTest, FreeIdempotent) {
  LinearMemory *arena = LinearMemory::init(4096);

  arena->assign<int>(42);
  bool first = arena->free();
  EXPECT_TRUE(first);

  bool second = arena->free();
  EXPECT_TRUE(second);

  delete arena;
}

TEST_F(LinearMemoryTest, LargeAllocationFits) {
  LinearMemory *arena = LinearMemory::init(1024 * 1024);

  auto *arr = arena->assign<CharArray>();
  ASSERT_NE(arr, nullptr);

  delete arena;
}

TEST_F(LinearMemoryTest, CustomBlockAlignment) {
  static constexpr std::size_t kAlignment = 64;
  LinearMemory *arena = LinearMemory::init(65536, kAlignment);

  uintptr_t base = reinterpret_cast<uintptr_t>(arena);
  EXPECT_EQ(base % kAlignment, 0) << "Arena base should be 64-byte aligned";

  auto *obj = arena->assign<int>(99);
  ASSERT_NE(obj, nullptr);
  EXPECT_EQ(reinterpret_cast<uintptr_t>(obj) % kAlignment, 0)
      << "First allocation should be 64-byte aligned";

  delete arena;
}
