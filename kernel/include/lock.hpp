#ifndef LOCK_HPP
#define LOCK_HPP 1

#include <atomic>
#include <stddef.h>
#include <arch.hpp>
#include <sys/defs.h>
#include <memory>

namespace lock
{
class TicketLock
{
  public:
	constexpr TicketLock() : next_ticket_(0), serving_ticket_(0)
	{
	}

	TicketLock(const TicketLock&) = delete;
	TicketLock& operator=(const TicketLock&) = delete;

	void lock()
	{
		size_t ticket = this->next_ticket_.fetch_add(1, std::memory_order_relaxed);
		while(this->serving_ticket_.load(std::memory_order_acquire) != ticket)
		{
			pause();
		}
	}

	bool is_locked() const
	{
		size_t current = this->serving_ticket_.load(std::memory_order_relaxed);
		size_t next = this->next_ticket_.load(std::memory_order_relaxed);

		return current != next;
	}

	void unlock()
	{
		if(!this->is_locked())
		{
			return;
		}

		size_t current = this->serving_ticket_.load(std::memory_order_relaxed);
		this->serving_ticket_.store(current + 1, std::memory_order_release);
	}

	bool try_lock()
	{
		if(this->is_locked())
		{
			return false;
		}

		this->lock();
		return true;
	}

  private:
	std::atomic_size_t next_ticket_;
	std::atomic_size_t serving_ticket_;
};

using mutex = TicketLock;

struct DeferLock
{
	explicit DeferLock() = default;
};

struct TryToLock
{
	explicit TryToLock() = default;
};

struct AdoptLock
{
	explicit AdoptLock() = default;
};

inline constexpr DeferLock defer_lock{};
inline constexpr TryToLock try_to_lock{};
inline constexpr AdoptLock adopt_lock{};

template<typename T>
class ScopedLock
{
  public:
	using mutex_type = T;

	ScopedLock() : mutex_(nullptr), locked_(false)
	{
	}

	ScopedLock(ScopedLock&& other) noexcept : mutex_(nullptr), locked_(false)
	{
		this->swap(other);
	}

	explicit ScopedLock(mutex_type& m) : mutex_(std::addressof(m)), locked_(true)
	{
		m.lock();
	}

	ScopedLock(mutex_type& m, DeferLock) noexcept : mutex_(std::addressof(m)), locked_(false)
	{
	}

	ScopedLock(mutex_type& m, TryToLock) : mutex_(std::addressof(m)), locked_(true)
	{
		m.try_lock();
	}

	ScopedLock(mutex_type& m, AdoptLock) : mutex_(std::addressof(m)), locked_(true)
	{
	}

	~ScopedLock()
	{
		if(this->locked_)
		{
			this->mutex_->unlock();
		}
	}

	ScopedLock& operator=(ScopedLock&& other)
	{
		if(this->locked_)
		{
			this->unlock();
		}

		this->swap(other);
		return *this;
	}

	void lock()
	{
		if(this->mutex_)
		{
			this->mutex_->lock();
			this->locked_ = true;
		}
	}

	bool try_lock()
	{
		if(this->mutex_)
		{
			bool ret = this->mutex_->try_lock();

			if(ret)
			{
				this->locked_ = true;
			}

			return ret;
		}

		return false;
	}

	void unlock()
	{
		if(this->mutex_)
		{
			this->mutex_->unlock();
			this->locked_ = false;
		}
	}

	void swap(ScopedLock& other) noexcept
	{
		using std::swap;
		swap(this->mutex_, other.mutex_);
		swap(this->locked_, other.locked_);
	}

	mutex_type* release() noexcept
	{
		T* ret = this->mutex_;

		this->mutex_ = nullptr;
		this->locked_ = false;

		return ret;
	}

	mutex_type* mutex() const noexcept
	{
		return this->mutex_;
	}

	bool owns_lock() const noexcept
	{
		return this->locked_;
	}

	explicit operator bool() const noexcept
	{
		return this->owns_lock();
	}

	friend void swap(ScopedLock& lhs, ScopedLock& rhs) noexcept
	{
		return lhs.swap(rhs);
	}

  private:
	T* mutex_;
	bool locked_;
};
} // namespace lock

#endif // LOCK_HPP