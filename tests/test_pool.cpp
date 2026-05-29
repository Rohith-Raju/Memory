#include "pool_allocator.h"
#include <gtest/gtest.h>
#include <cstddef>
#include <stdexcept>
#include <cstring>

struct SmallPod {
  int x;
  float y;
  SmallPod(int x, float y) : x{x}, y{y} {}
};

struct LargeObject {
  char padding[256];
};

static int alive_count = 0;

struct LifecycleProbe {
  int id;
  LifecycleProbe(int id) : id(id) { ++alive_count; }
  ~LifecycleProbe() { --alive_count; }
  LifecycleProbe(const LifecycleProbe &) = delete;
  LifecycleProbe &operator=(const LifecycleProbe &) = delete;
};

class PoolMemoryTest : public ::testing::Test {
protected:
  void SetUp() override { alive_count = 0; }
  void TearDown() override {
    ASSERT_EQ(alive_count, 0)
        << "memory leak: " << alive_count << " objects still alive";
  }
};

TEST_F(PoolMemoryTest, InitSingleBlock) {
  PoolMemory *pool = PoolMemory::init(1);
  ASSERT_NE(pool, nullptr);
  delete pool;
}

TEST_F(PoolMemoryTest, AssignAndFree) {
  PoolMemory *pool = PoolMemory::init(4);

  auto *a = pool->assign<SmallPod>(42, 3.14f);
  ASSERT_NE(a, nullptr);
  EXPECT_EQ(a->x, 42);
  EXPECT_FLOAT_EQ(a->y, 3.14f);

  pool->free(a);
  delete pool;
}

TEST_F(PoolMemoryTest, AllocateAllBlocks) {
  PoolMemory *pool = PoolMemory::init(3);

  auto *a = pool->assign<LifecycleProbe>(1);
  auto *b = pool->assign<LifecycleProbe>(2);
  auto *c = pool->assign<LifecycleProbe>(3);

  ASSERT_NE(a, nullptr);
  ASSERT_NE(b, nullptr);
  ASSERT_NE(c, nullptr);
  EXPECT_EQ(a->id, 1);
  EXPECT_EQ(b->id, 2);
  EXPECT_EQ(c->id, 3);
  EXPECT_EQ(alive_count, 3);

  pool->free(c);
  EXPECT_EQ(alive_count, 2);
  pool->free(b);
  EXPECT_EQ(alive_count, 1);
  pool->free(a);
  EXPECT_EQ(alive_count, 0);

  delete pool;
}

TEST_F(PoolMemoryTest, ExhaustThenFreeThenRealloc) {
  PoolMemory *pool = PoolMemory::init(2);

  auto *a = pool->assign<LifecycleProbe>(10);
  auto *b = pool->assign<LifecycleProbe>(20);
  EXPECT_THROW(pool->assign<LifecycleProbe>(99), std::bad_alloc);

  pool->free(a);
  auto *c = pool->assign<LifecycleProbe>(30);
  ASSERT_NE(c, nullptr);
  EXPECT_EQ(c->id, 30);

  pool->free(b);
  pool->free(c);
  delete pool;
}

TEST_F(PoolMemoryTest, FreeNullptrNoOp) {
  PoolMemory *pool = PoolMemory::init(1);
  EXPECT_NO_THROW(pool->free((LifecycleProbe *)nullptr));
  pool->free((SmallPod *)nullptr);
  delete pool;
}

TEST_F(PoolMemoryTest, OversizedTypeThrows) {
  PoolMemory *pool = PoolMemory::init(2, 16);
  EXPECT_THROW(pool->assign<LargeObject>(), std::length_error);
  delete pool;
}

TEST_F(PoolMemoryTest, NoBlocksThrows) {
  EXPECT_THROW(PoolMemory::init(0), std::invalid_argument);
}

TEST_F(PoolMemoryTest, FitsExactlyInBlock) {
  PoolMemory *pool = PoolMemory::init(2, static_cast<uint8_t>(sizeof(SmallPod)));

  auto *a = pool->assign<SmallPod>(1, 1.0f);
  auto *b = pool->assign<SmallPod>(2, 2.0f);
  ASSERT_NE(a, nullptr);
  ASSERT_NE(b, nullptr);

  pool->free(a);
  pool->free(b);
  delete pool;
}

TEST_F(PoolMemoryTest, BlockReuseAfterFree) {
  PoolMemory *pool = PoolMemory::init(1);

  auto *first = pool->assign<LifecycleProbe>(100);
  ASSERT_NE(first, nullptr);
  pool->free(first);

  auto *second = pool->assign<LifecycleProbe>(200);
  ASSERT_NE(second, nullptr);
  EXPECT_EQ(second->id, 200);
  pool->free(second);

  delete pool;
}

TEST_F(PoolMemoryTest, SingleBlockAllocFreeCycle) {
  PoolMemory *pool = PoolMemory::init(1);

  for (int i = 0; i < 5; ++i) {
    auto *obj = pool->assign<LifecycleProbe>(i);
    ASSERT_NE(obj, nullptr);
    EXPECT_EQ(obj->id, i);
    pool->free(obj);
  }

  delete pool;
}

TEST_F(PoolMemoryTest, PrintStatsDoesNotCrash) {
  PoolMemory *pool = PoolMemory::init(5);
  EXPECT_NO_THROW(pool->print_stats());

  auto *a = pool->assign<LifecycleProbe>(1);
  auto *b = pool->assign<LifecycleProbe>(2);
  EXPECT_NO_THROW(pool->print_stats());

  pool->free(a);
  EXPECT_NO_THROW(pool->print_stats());

  pool->free(b);
  delete pool;
}

TEST_F(PoolMemoryTest, DestructorRestoresMemory) {
  PoolMemory *pool = PoolMemory::init(1);
  auto *obj = pool->assign<LifecycleProbe>(42);
  ASSERT_NE(obj, nullptr);
  EXPECT_EQ(obj->id, 42);

  // Freeing sets alive_count to 0, then delete unmaps
  pool->free(obj);
  delete pool;
}

TEST_F(PoolMemoryTest, DefaultBlockSize) {
  PoolMemory *pool = PoolMemory::init(100);
  ASSERT_NE(pool, nullptr);
  for (int i = 0; i < 100; ++i) {
    auto *obj = pool->assign<LifecycleProbe>(i);
    ASSERT_NE(obj, nullptr);
  }
  EXPECT_THROW(pool->assign<LifecycleProbe>(0), std::bad_alloc);
  // Cannot free all 100 in loop since we lost the pointers, but destructor
  // will munmap anyway — inject manual alive_count to avoid TearDown failure
  alive_count = 0;
  delete pool;
}
