#ifndef DRIVERS_APIC_TIMER_HPP
#define DRIVERS_APIC_TIMER_HPP 1

#include <drivers/timers.hpp>
#include <drivers/pit.hpp>

namespace drivers
{
namespace timers
{
void calibrate_apic_timer();
}
} // namespace drivers

#endif // DRIVERS_APIC_TIMER_HPP