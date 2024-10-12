#ifndef DRIVERS_INTERRUPTS_HPP
#define DRIVERS_INTERRUPTS_HPP

#include <errno.h>
#include <libs/function.hpp>
#include <sys/defs.h>
#include <cpu/registers.h>

#define TRIGGER_MODE_EDGE (0)
#define TRIGGER_MODE_LEVEL (1)

#define POLARITY_ACTIVE_HIGH (0)
#define POLARITY_ACTIVE_LOW (1)

namespace drivers
{
namespace interrupts
{
struct InterruptHandler
{
	std::function<void(Iframe*)> handler;
	bool reserved;
	bool polarity;
	bool trigger_mode;
	int vector;

	template<typename Func, typename... Args>
	inline error_t set(Func&& __func, Args&&... __args)
	{
		if(this->handler)
		{
			return SYSTEM_ERR_ALREADY_EXISTS;
		}

		this->handler = [func = std::forward<Func>(__func),
						 ... args = std::forward<Args>(__args)](struct Iframe* iframe) mutable {
			func(iframe, args...);
		};

		this->reserved = true;

		return SYSTEM_OK;
	}

	inline error_t reserve()
	{
		if(this->reserved)
		{
			return SYSTEM_ERR_ALREADY_EXISTS;
		}

		return this->reserved = true;
	}

	inline bool reset()
	{
		bool ret = bool(this->handler);
		this->handler = nullptr;
		return ret;
	}

	inline error_t operator()(struct Iframe* __iframe)
	{
		if(!this->reserved)
		{
			return SYSTEM_ERR_NOT_FOUND;
		}

		this->handler(__iframe);
		return SYSTEM_OK;
	}
};

void initialize();
std::pair<InterruptHandler&, int> allocate_handler(int __hint = 0);
InterruptHandler& get_handler(int __vector);

void set_interrupt_mask(int __vector);
void clear_interrupt_mask(int __vector);
void issue_eoi(int __vector);
} // namespace interrupts
} // namespace drivers

#endif // DRIVERS_INTERRUPTS_HPP