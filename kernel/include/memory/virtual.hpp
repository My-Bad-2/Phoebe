#ifndef MEMORY_VIRTUAL_HPP
#define MEMORY_VIRTUAL_HPP 1

#include <memory/memory.hpp>

#define MAP_READ (0)
#define MAP_WRITE (1 << 0)
#define MAP_EXEC (1 << 2)
#define MAP_GLOBAL (1 << 3)
#define MAP_USER (1 << 4)
#define MAP_PAGE_1GB (1 << 5)
#define MAP_PAGE_2MB (1 << 6)
#define MAP_MMIO (1 << 7)
#define MAP_WRITE_THROUGH (1 << 6)
#define MAP_PROTECTED (1 << 7)
#define MAP_WRITE_COMBINING (1 << 8)
#define MAP_WRITE_BACK (1 << 9)
#define MAP_NO_CACHE (1 << 10)

#endif // MEMORY_VIRTUAL_HPP