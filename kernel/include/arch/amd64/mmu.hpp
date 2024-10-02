#ifndef MMU_HPP
#define MMU_HPP 1

#define PAGE_FLAG_PRESENT (1 << 0)
#define PAGE_FLAG_WRITABLE (1 << 1)
#define PAGE_FLAG_USER_ACCESSIBLE (1 << 2)
#define PAGE_FLAG_WRITE_THROUGH (1 << 3)
#define PAGE_FLAG_NO_CACHE (1 << 4)
#define PAGE_FLAG_ACCESSED (1 << 5)
#define PAGE_FLAG_DIRTY (1 << 6)
#define PAGE_FLAG_SIZE_EXTENSION (1 << 7)
#define PAGE_FLAG_GLOBAL (1 << 8)
#define PAGE_FLAG_NO_EXECUTE (1UL << 63)

#define NEW_PAGE_FLAGS (PAGE_FLAG_PRESENT | PAGE_FLAG_WRITABLE | PAGE_FLAG_USER_ACCESSIBLE)

#endif // MMU_HPP