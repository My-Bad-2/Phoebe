#include <stdlib.h>
#include <new>

void* operator new(std::size_t size)
{
	return malloc(size);
}

void* operator new[](std::size_t size)
{
	return malloc(size);
}

void operator delete(void* ptr)
{
	free(ptr);
}

void operator delete[](void* ptr)
{
	free(ptr);
}

void* operator new(std::size_t size, const std::nothrow_t&) noexcept
{
	return malloc(size);
}

void* operator new[](std::size_t size, const std::nothrow_t&) noexcept
{
	return malloc(size);
}

void operator delete(void* ptr, const std::nothrow_t&)
{
	free(ptr);
}

void operator delete[](void* ptr, const std::nothrow_t&)
{
	free(ptr);
}

void* operator new(std::size_t size, std::align_val_t)
{
	return malloc(size);
}

void* operator new(std::size_t size, std::align_val_t, const std::nothrow_t&) noexcept
{
	return malloc(size);
}

void operator delete(void* ptr, std::align_val_t)
{
	free(ptr);
}

void operator delete(void* ptr, std::align_val_t, const std::nothrow_t&)
{
	free(ptr);
}

void* operator new[](std::size_t size, std::align_val_t)
{
	return malloc(size);
}

void* operator new[](std::size_t size, std::align_val_t, const std::nothrow_t&) noexcept
{
	return malloc(size);
}

void operator delete[](void* ptr, std::align_val_t)
{
	free(ptr);
}

void operator delete[](void* ptr, std::align_val_t, const std::nothrow_t&)
{
	free(ptr);
}
