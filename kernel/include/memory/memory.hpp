#ifndef MEMORY_MEMORY_HPP
#define MEMORY_MEMORY_HPP 1

#include <kernel.h>
#include <concepts>
#include <stddef.h>

#define PAGE_SIZE 0x1000UL
#define PAGE_SIZE_2MiB 0x200000UL
#define PAGE_SIZE_1GiB 0x40000000UL

namespace memory
{
template<typename T>
using GetRetType =
	std::conditional_t<std::integral<T>,
					   std::conditional_t<std::unsigned_integral<T>, uintptr_t, uintptr_t>, T>;

inline constexpr bool is_higher_half(auto __addr)
{
	return uintptr_t(__addr) >= hhdm_request.response->offset;
}

template<typename T, typename U = GetRetType<T>>
inline constexpr U to_higher_half(T __addr)
{
	return is_higher_half(__addr) ? U(__addr) :
									U(uintptr_t(__addr) + hhdm_request.response->offset);
}

template<typename T, typename U = GetRetType<T>>
inline constexpr U from_higher_half(T __addr)
{
	return !is_higher_half(__addr) ? U(__addr) :
									 U(uintptr_t(__addr) - hhdm_request.response->offset);
}

template<std::integral T, std::integral U>
inline constexpr auto align_down(T __addr, U __size)
{
	return uintptr_t(__addr) & ~(size_t(__size) - 1);
}

inline constexpr auto align_up(std::integral auto __addr, std::integral auto __size)
{
	return align_down(__addr + __size - 1, __size);
}

inline constexpr auto div_roundup(std::integral auto __addr, std::integral auto __size)
{
	return align_down(__addr, __size) / __size;
}

void initialize();
} // namespace memory

#endif // MEMORY_MEMORY_HPP