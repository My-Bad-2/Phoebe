#ifndef LIBS_BITMAP_HPP
#define LIBS_BITMAP_HPP 1

#include <stdint.h>
#include <stddef.h>
#include <assert.h>

enum BitmapMark
{
	BITMAP_FREE,
	BITMAP_USED,
};

class Bitmap
{
  public:
	constexpr Bitmap() = default;

	constexpr Bitmap(uint8_t* __buffer, size_t __size) : buffer_(__buffer), size_(__size)
	{
	}

	constexpr Bitmap(const Bitmap&) = delete;
	constexpr Bitmap(Bitmap&&) = delete;

	constexpr Bitmap& operator=(const Bitmap&) = delete;
	constexpr Bitmap& operator=(Bitmap&&) = delete;

	~Bitmap() = default;

	class Bit
	{
	  public:
		constexpr Bit(Bitmap& __parent, size_t __index) : parent_(__parent), index_(__index)
		{
		}

		constexpr void operator=(bool __val)
		{
			this->parent_.set(this->index_, __val);
		}

		constexpr operator bool() const
		{
			return this->parent_.bit(this->index_);
		}

	  private:
		Bitmap& parent_;
		size_t index_;
	};

	constexpr void initialize(uint8_t* __buffer, size_t __size)
	{
		assert((__buffer != nullptr) && (__size != 0));

		this->buffer_ = __buffer;
		this->size_ = __size;
	}

	constexpr uint8_t byte(size_t __index) const
	{
		assert(__index < this->size_);
		return this->buffer_[__index / 8];
	}

	constexpr bool bit(size_t __index) const
	{
		assert(__index < this->size_);
		return this->byte(__index) & (1 << (__index % 8));
	}

	constexpr Bit operator[](size_t __index)
	{
		return Bit(*this, __index);
	}

	constexpr void set(size_t __index, bool __val)
	{
		assert(__index < this->size_);

		if(__val)
		{
			this->buffer_[__index / 8] |= (1 << (__index % 8));
		}
		else
		{
			this->buffer_[__index / 8] &= ~(1 << (__index % 8));
		}
	}

	constexpr uint8_t* buffer() const
	{
		return this->buffer_;
	}

  private:
	uint8_t* buffer_;
	size_t size_;
};

#endif // LIBS_BITMAP_HPP