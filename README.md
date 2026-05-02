# mymalloc

Small C++17 examples of **arena-style memory** backed by `mmap(2)`: a **fixed-block pool allocator** (`PoolMemory`) and a **bump / linear allocator** (`LinearMemory`). The CMake project is named `mymalloc`; the executable links `main.cpp` with the allocator implementations.

## Requirements

- **CMake** 3.25 or newer  
- **C++17** compiler (Clang or GCC)  
- **POSIX** environment with `mmap` / `munmap` (macOS, Linux, and similar). This code is not portable to Windows without replacing the mapping layer.

## Build

```bash
cmake -S . -B build
cmake --build build
```

Run the demo:

```bash
./build/mymalloc
```

## Layout

| Path | Role |
|------|------|
| `include/pool_allocator.h`, `src/pool_allocator.cpp` | Fixed-size block pool |
| `include/linear_allocator.h`, `src/linear_allocator.cpp` | Linear bump allocator |
| `include/utils.h` | Alignment helper |
| `main.cpp` | Example using `PoolMemory` |

## `PoolMemory` (pool allocator)

**Idea:** Reserve a contiguous region, slice it into equal-sized blocks, and keep free blocks on a singly linked **free list**. Allocation pops a node from the list and placement-`new`s your object there.

### Construction

```cpp
static PoolMemory *init(size_t blocks = 1024 * 1024, uint8_t size_per_block = 64);
```

- **`blocks`** — number of blocks in the pool.  
- **`size_per_block`** — minimum payload bytes per block; internally clamped to at least `sizeof(FreeNode)` and rounded up to **8-byte** alignment.

The backing store is anonymous private memory from `mmap`.

### Allocation

```cpp
template <typename Entity, typename... Args>
Entity *assign(Args... args);
```

Constructs `Entity` in the next free block with `Entity(args...)`. If `sizeof(Entity)` exceeds the block size, the program prints a message and calls `exit(1)`.

### Deallocation

```cpp
template <typename T>
void free(T *ptr);
```

Returns the block to the free list after `delete ptr` (so `T` must have a trivial or appropriate destructor path for how you use the pool). The pointer must have come from `assign` on this pool.

### Other

- **`print_stats()`** — prints allocator-related diagnostics (implementation-specific).

## `LinearMemory` (bump / linear allocator)

**Idea:** One contiguous arena: a **cursor** advances on each `assign`, with **alignment** satisfied for each `T` before constructing the object. There is no per-object `free`; you reset or unmap the whole region.

### Construction

```cpp
static LinearMemory *init(std::size_t mem_size,
                          std::size_t alignment = sizeof(std::max_align_t));
```

- Reserves enough anonymous mapped memory for the `LinearMemory` header plus **`mem_size`** bytes of user arena, rounded up to a **whole number of pages** (using `sysconf(_SC_PAGE_SIZE)`).
- Logs requested size, total need, page size, and aligned mapping size to `stdout`.

### Allocation

```cpp
template <typename T, typename... Args>
T *assign(Args... args);
```

Aligns `current` to `alignof(T)`, bumps the cursor by `sizeof(T)`, placement-`new`s `T(args...)`. If the aligned object would cross **`soft_end`** (end of the user arena), prints an error (including “wasted” slack to `hard_end`) and `exit(1)`.

### Reset / teardown

```cpp
bool free();  // munmap entire mapping; returns true if munmap succeeded
```

The destructor calls `free()`.

### Other

- **`print_stats()`** — prints addresses, sizes, and slack between soft and hard end.

## `Utils::alignment`

```cpp
static size_t alignment(size_t need, size_t multiple);
```

Returns the smallest value `>= need` that is a multiple of `power-of-two` `multiple` (typical use: page size or alignments).

## Example (`main.cpp`)

```cpp
PoolMemory *pool = PoolMemory::init(100);
Entity *entity = pool->assign<Entity>(1, 2, 4);
entity->printEntity();
```

This creates a pool of **100** blocks (default **64**-byte blocks unless you pass a second argument), allocates one `Entity`, and prints it.

## Design notes

- **Pool:** Good for many same-sized or small objects; constant-time allocation from the free list when blocks fit.  
- **Linear:** Very fast sequential allocation and simple teardown; no individual frees—patterns like “frame allocator” or “parse this buffer in one pass” fit well.

For production use you would typically add error handling instead of `exit`, optional `munmap` in `PoolMemory`’s destructor, and tests that cover alignment edge cases and exhaustion.
