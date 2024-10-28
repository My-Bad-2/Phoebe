#ifndef DRIVERS_PIT_HPP
#define DRIVERS_PIT_HPP 1

#include <stdint.h>
#include <drivers/timers.hpp>

namespace drivers
{
namespace timers
{
void initialize_pit();
void pit_sleep(uint32_t __msec);
} // namespace timers
} // namespace drivers

#endif // DRIVERS_PIT_HPP