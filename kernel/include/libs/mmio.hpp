#ifndef LIBS_MMIO_HPP
#define LIBS_MMIO_HPP 1

#include <concepts>

template<std::unsigned_integral T>
inline T mmio_in(auto addr)
{
	volatile T* ptr = reinterpret_cast<volatile T*>(addr);
	return *ptr;
}

template<std::unsigned_integral T>
inline void mmio_out(auto addr, T val)
{
	volatile T* ptr = reinterpret_cast<volatile T*>(addr);
	*ptr = val;
}

#endif // LIBS_MMIO_HPP