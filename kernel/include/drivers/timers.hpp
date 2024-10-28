#ifndef DRIVERS_TIMERS_HPP
#define DRIVERS_TIMERS_HPP 1

#include <stddef.h>

namespace drivers
{
namespace timers
{
size_t get_time();
void sleep(size_t ms);
} // namespace timers
} // namespace drivers

#endif // DRIVERS_TIMERS_HPP