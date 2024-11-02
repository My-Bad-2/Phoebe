#ifndef CPU_GDT_HPP
#define CPU_GDT_HPP 1

#include <sys/defs.h>
#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include <array>

namespace cpu
{
namespace gdt
{
/**
 * @brief Represents a segment descriptor in the Global Descriptor Table (GDT).
 */
struct GdtSegment
{
	uint16_t limit; /**< Segment limit in bytes. */
	uint16_t base_low; /**< Lower 16 bits of the segment base address. */
	uint8_t base_mid; /**< Middle 8 bits of the segment base address. */
	uint8_t access; /**< Segment access flags (e.g., read/write, executable). */
	uint8_t limit_high : 4; /**< Upper 4 bits of the segment limit. */
	uint8_t granularity : 4; /**< Granularity (0: byte, 1: page size). */
	uint8_t base_high; /**< Highest 8 bits of the segment base address. */

	/**
	 * @brief Initializes the GDT segment descriptor.
	 *
	 * @param base Base address of the segment.
	 * @param limit Segment limit in bytes.
	 * @param granularity Granularity settings for the segment.
	 * @param access Access flags for the segment.
	 */
	void create_entry(uint32_t base, uint32_t limit, uint8_t granularity, uint8_t access);

	/**
	 * @brief Returns the raw 64-bit value of the GDT segment descriptor.
	 *
	 * @return The 64-bit representation of the segment descriptor.
	 */
	uint64_t raw();

	/**
	 * @brief Initializes the GDT segment with default base and limit values.
	 *
	 * @param granularity Granularity settings.
	 * @param access Access flags.
	 */
	inline void create_entry(uint8_t granularity, uint8_t access)
	{
		return create_entry(0, 0xffffffff, granularity, access);
	}
} __PACKED;

/**
 * @brief Represents a Task State Segment (TSS) descriptor in the GDT.
 */
struct TssSegment
{
	uint16_t limit; /**< Segment limit in bytes. */
	uint16_t base_low; /**< Lower 16 bits of the TSS base address. */
	uint8_t base_mid; /**< Middle 8 bits of the TSS base address. */
	uint8_t flags_low; /**< Low part of TSS flags. */
	uint8_t flags_high; /**< High part of TSS flags. */
	uint8_t base_high; /**< Highest 8 bits of the TSS base address. */
	uint32_t base_upper; /**< Upper 32 bits of the TSS base address. */
	uint32_t reserved; /**< Reserved for future use. */

	/**
	 * @brief Initializes the TSS descriptor with a TSS structure.
	 *
	 * @param tss Pointer to the TSS structure.
	 */
	void create_entry(void* tss);
} __PACKED;

/**
 * @brief Represents a Task State Segment (TSS).
 */
struct Tss
{
	uint32_t reserved0; /**< Reserved field. */
	std::array<uint64_t, 3> rsp; /**< Stack pointers for different privilege levels. */
	uint64_t reserved1; /**< Reserved field. */
	std::array<uint64_t, 7> ist; /**< Interrupt Stack Table (IST) pointers. */
	uint64_t reserved2; /**< Reserved field. */
	uint16_t reserved3; /**< Reserved field. */
	uint16_t iopb_offset; /**< Offset to the I/O Permission Bitmap. */
} __PACKED;

/**
 * @brief Represents a Global Descriptor Table (GDT) with segments and a TSS descriptor.
 */
struct GdtTable
{
	std::array<GdtSegment, 5> table; /**< Array of GDT segments. */
	TssSegment tss; /**< TSS segment descriptor. */

	/**
	 * @brief Accesses a GDT segment by index.
	 *
	 * @param index Index of the segment in the GDT.
	 * @return Reference to the GDT segment at the specified index.
	 */
	GdtSegment& operator[](size_t index)
	{
		return this->table[index];
	}
} __PACKED;

static_assert(sizeof(GdtTable) != 0, "No GDT can have a size of 0 bytes.");
static_assert(sizeof(GdtTable) <= 0x10000, "Maximum number of GDT entries exceeded.");

/**
 * @brief Holds the GDT register, which contains the limit and base address of the GDT.
 */
struct GdtRegister
{
	uint16_t limit; /**< Limit of the GDT in bytes. */
	uint64_t base; /**< Base address of the GDT. */
} __PACKED;

error_t initialize();
} // namespace gdt
} // namespace cpu

#endif // CPU_GDT_HPP