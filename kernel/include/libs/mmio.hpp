#ifndef LIBS_MMIO_HPP
#define LIBS_MMIO_HPP 1

#include <concepts>

/**
 * @brief Reads a value from a memory-mapped I/O (MMIO) address.
 *
 * This function reads an unsigned integer of type `T` from a specified
 * memory-mapped I/O address. The template is constrained to unsigned integral
 * types. The address is cast to a pointer of the appropriate type and
 * dereferenced to retrieve the value.
 *
 * @tparam T The unsigned integral type of the value to read. Common types
 *           include `uint8_t`, `uint16_t`, `uint32_t`, etc.
 * @param addr The memory-mapped I/O address to read from.
 * @return The value read from the specified MMIO address, represented as type `T`.
 */
template<std::unsigned_integral T>
inline T mmio_in(auto addr)
{
	volatile T* ptr = reinterpret_cast<volatile T*>(addr);
	return *ptr;
}

/**
 * @brief Writes a value to a memory-mapped I/O (MMIO) address.
 *
 * This function writes an unsigned integer value of type `T` to a specified
 * memory-mapped I/O address. The template is constrained to unsigned integral
 * types. The address is cast to a pointer of the appropriate type and
 * dereferenced to store the value.
 *
 * @tparam T The unsigned integral type of the value to write. Common types
 *           include `uint8_t`, `uint16_t`, `uint32_t`, etc.
 * @param addr The memory-mapped I/O address to write to.
 * @param val The value to write to the specified MMIO address, represented as type `T`.
 */
template<std::unsigned_integral T>
inline void mmio_out(auto addr, T val)
{
	volatile T* ptr = reinterpret_cast<volatile T*>(addr);
	*ptr = val;
}

#endif // LIBS_MMIO_HPP