#ifndef LIBS_BITMAP_HPP
#define LIBS_BITMAP_HPP 1

#include <stdint.h>
#include <stddef.h>
#include <assert.h>

enum BitmapMark
{
	BITMAP_FREE, // Indicates a free bit in the bitmap
	BITMAP_USED // Indicates a used bit in the bitmap
};

/**
 * @brief A simple bitmap class to manage individual bits within a byte array.
 */
class Bitmap
{
  public:
	constexpr Bitmap() = default;

	/**
	 * @brief Constructs a Bitmap instance with a given buffer and size.
	 *
	 * @param buffer Pointer to the byte array acting as the bitmap.
	 * @param size Size of the bitmap in bits.
	 */
	constexpr Bitmap(uint8_t* buffer, size_t size) : buffer_(buffer), size_(size)
	{
	}

	// Deleted copy and move constructors and assignment operators to avoid accidental copying
	constexpr Bitmap(const Bitmap&) = delete;
	constexpr Bitmap(Bitmap&&) = delete;
	constexpr Bitmap& operator=(const Bitmap&) = delete;
	constexpr Bitmap& operator=(Bitmap&&) = delete;

	~Bitmap() = default;

	/**
	 * @brief Helper class representing a single bit within the Bitmap.
	 *
	 * This class allows access and modification of a single bit within the Bitmap.
	 */
	class Bit
	{
	  public:
		/**
		 * @brief Constructs a Bit instance.
		 *
		 * @param parent Reference to the parent Bitmap object.
		 * @param index Index of the bit within the Bitmap.
		 */
		constexpr Bit(Bitmap& parent, size_t index) : parent_(parent), index_(index)
		{
		}

		/**
		 * @brief Assignment operator for setting the bit value.
		 *
		 * @param val Boolean value to set the bit (true = 1, false = 0).
		 */
		constexpr void operator=(bool val)
		{
			this->parent_.set(this->index_, val);
		}

		/**
		 * @brief Implicit conversion to bool to get the bit value.
		 *
		 * @return true if the bit is set, false otherwise.
		 */
		constexpr operator bool() const
		{
			return this->parent_.bit(this->index_);
		}

	  private:
		Bitmap& parent_;
		size_t index_;
	};

	/**
	 * @brief Initializes the bitmap with a buffer and size.
	 *
	 * @param buffer Pointer to the byte array.
	 * @param size Number of bits in the bitmap.
	 */
	constexpr void initialize(uint8_t* buffer, size_t size)
	{
		assert((buffer != nullptr) && (size != 0));
		this->buffer_ = buffer;
		this->size_ = size;
	}

	/**
	 * @brief Retrieves the byte containing the specified bit.
	 *
	 * @param index Index of the bit in the bitmap.
	 * @return The byte containing the bit.
	 */
	constexpr uint8_t byte(size_t index) const
	{
		assert(index < this->size_);
		return this->buffer_[index / 8];
	}

	/**
	 * @brief Checks if a specific bit is set.
	 *
	 * @param index Index of the bit in the bitmap.
	 * @return true if the bit is set, false otherwise.
	 */
	constexpr bool bit(size_t index) const
	{
		assert(index < this->size_);
		return this->byte(index) & (1 << (index % 8));
	}

	/**
	 * @brief Provides access to a specific bit within the Bitmap.
	 *
	 * @param index Index of the bit.
	 * @return A Bit object for accessing the bit.
	 */
	constexpr Bit operator[](size_t index)
	{
		return Bit(*this, index);
	}

	/**
	 * @brief Sets or clears a specific bit in the Bitmap.
	 *
	 * @param index Index of the bit in the bitmap.
	 * @param val Boolean value to set the bit (true = 1, false = 0).
	 */
	constexpr void set(size_t index, bool val)
	{
		assert(index < this->size_);
		if(val)
		{
			this->buffer_[index / 8] |= (1 << (index % 8));
		}
		else
		{
			this->buffer_[index / 8] &= ~(1 << (index % 8));
		}
	}

	/**
	 * @brief Returns the buffer associated with this Bitmap.
	 *
	 * @return Pointer to the byte array used as the bitmap buffer.
	 */
	constexpr uint8_t* buffer() const
	{
		return this->buffer_;
	}

  private:
	uint8_t* buffer_; ///< Pointer to the bitmap's byte array buffer.
	size_t size_; ///< Size of the bitmap in bits.
};

#endif // LIBS_BITMAP_HPP