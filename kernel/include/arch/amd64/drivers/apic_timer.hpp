#ifndef DRIVERS_APIC_TIMER_HPP
#define DRIVERS_APIC_TIMER_HPP 1

#include <drivers/timers.hpp>
#include <drivers/pit.hpp>

#include <cpu/lapic.hpp>

namespace drivers
{
namespace timers
{
void calibrate_apic_timer();
void apic_timer(uint8_t vector, size_t ms, cpu::apic::TimerModes mode);
} // namespace timers
} // namespace drivers

#endif // DRIVERS_APIC_TIMER_HPP