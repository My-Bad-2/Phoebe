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
/**
 * @brief Reads a value from the specified I/O port.
 *
 * This function uses inline assembly to perform a read operation on the 
 * specified port, returning the value read as an unsigned integer of type `T`. 
 * The function template is constrained to unsigned integral types with a 
 * size up to `uint32_t`. 
 *
 * @tparam T The unsigned integral type to be read. Should be `uint8_t`, `uint16_t`, or `uint32_t`.
 * @param port The I/O port from which to read the value.
 * @return The value read from the I/O port, represented as type `T`.
 */
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

/**
 * @brief Writes a value to the specified I/O port.
 *
 * This function uses inline assembly to write a given unsigned integer value 
 * to the specified I/O port. The function template is constrained to unsigned 
 * integral types with a size up to `uint32_t`.
 *
 * @tparam T The unsigned integral type of the value to be written. Should be 
 *           `uint8_t`, `uint16_t`, or `uint32_t`.
 * @param port The I/O port to which the value will be written.
 * @param val The value to write to the I/O port, represented as type `T`.
 */
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

inline void io_wait()
{
	arch::outp<uint8_t>(0x80, 0);
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

bool interrupt_status();

void initialize();
void late_initialize();
} // namespace arch

#endif // ARCH_HPP