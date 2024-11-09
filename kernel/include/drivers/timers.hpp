#ifndef DRIVERS_TIMERS_HPP
#define DRIVERS_TIMERS_HPP 1

#include <stddef.h>

#define NS_PER_MS (1000000)

namespace drivers
{
namespace timers
{
enum class TimerMode
{
	TIMER_PERIODIC,
	TIMER_ONESHOT,
};

size_t get_time();
void sleep(size_t ms);

void set_oneshot_timer(size_t deadline);
void tick();
} // namespace timers
} // namespace drivers

#endif // DRIVERS_TIMERS_HPP