#ifndef ARCH_HPP
#define ARCH_HPP 1

#include <concepts>
#include <stdint.h>
#include <sys/defs.h>

#define pause() asm volatile("pause")
#define disable_interrupts() asm volatile("cli")
#define enable_interrupts() asm volatile("sti")
#define hlt() asm volatile("hlt")

namespace arch
{
template<std::unsigned_integral T>
	requires(sizeof(T) <= sizeof(uint32_t))
inline T inp(uint16_t port)
{
	T val = T(0);

	if constexpr(std::same_as<T, uint8_t>)
	{
		asm volatile("inb %1, %0" : "=a"(val) : "Nd"(port));
	}
	else if constexpr(std::same_as<T, uint16_t>)
	{
		asm volatile("inw %1, %0" : "=a"(val) : "Nd"(port));
	}
	else if constexpr(std::same_as<T, uint32_t>)
	{
		asm volatile("inl %1, %0" : "=a"(val) : "Nd"(port));
	}

	return val;
}

template<std::unsigned_integral T>
	requires(sizeof(T) <= sizeof(uint32_t))
inline void outp(uint16_t port, T val)
{
	if constexpr(std::same_as<T, uint8_t>)
	{
		asm volatile("outb %0, %1" ::"a"(val), "Nd"(port));
	}
	else if constexpr(std::same_as<T, uint16_t>)
	{
		asm volatile("outw %0, %1" ::"a"(val), "Nd"(port));
	}
	else if constexpr(std::same_as<T, uint32_t>)
	{
		asm volatile("outl %0, %1" ::"a"(val), "Nd"(port));
	}
}

__NO_RETURN inline void halt(bool interrupts = true)
{
	if(interrupts)
	{
		while(true)
		{
			hlt();
		}
	}

	while(true)
	{
		disable_interrupts();
		hlt();
	}
}

void initialize();
} // namespace arch

#endif // ARCH_HPP