#include <assert.h>
#include <libs/fixed_point.h>
#include <cpu/features.h>

#include <cpu/lapic.hpp>
#include <drivers/pit.hpp>
#include <drivers/apic_timer.hpp>

#include <algorithm>

namespace drivers
{
namespace timers
{
uint32_t apic_ticks_per_ms = 0;
uint8_t apic_divisor = 0;
fp_32_64 apic_ticks_per_ns;

void calibrate_apic_timer()
{
	const uint64_t apic_freq = cpu::apic::get_apic_freq();

	if(apic_freq != 0)
	{
		apic_ticks_per_ms = static_cast<uint32_t>(apic_freq / 1000);
		apic_divisor = 1;

		fp_32_64_div_32_32(&apic_ticks_per_ns, apic_ticks_per_ms, 1000 * 1000);

		log_debug("APIC frequency = %u ticks/ms", apic_ticks_per_ms);
		return;
	}

	apic_divisor = 1;
outer:
	while(apic_divisor != 0)
	{
		uint32_t best_time[2] = {UINT32_MAX, UINT32_MAX};
		const uint16_t duration_ms[2] = {2, 4};

		for(int trial = 0; trial < 2; trial++)
		{
			for(int tries = 0; tries < 3; tries++)
			{
				pit_calibration_precycle(duration_ms[trial]);

				error_t status = cpu::apic::timer_set_oneshot(UINT32_MAX, apic_divisor, true);
				assert(status == SYSTEM_OK);

				pit_calibration_cycle(duration_ms[trial]);

				uint32_t apic_ticks = UINT32_MAX - cpu::apic::timer_current_count();

				if(apic_ticks < best_time[trial])
				{
					best_time[trial] = apic_ticks;
				}

				log_debug("Calibration trial %d found %u ticks/ms", tries, apic_ticks);

				pit_calibration_cycle_cleanup();
			}

			if(best_time[trial] == UINT32_MAX)
			{
				apic_divisor = static_cast<uint8_t>(apic_divisor * 2);
				goto outer;
			}
		}

		apic_ticks_per_ms = (best_time[1] - best_time[0]) / (duration_ms[1] - duration_ms[0]);
		fp_32_64_div_32_32(&apic_ticks_per_ns, apic_ticks_per_ms, 1000 * 1000);
		break;
	}

	assert(apic_divisor != 0);

	log_debug("APIC timer calibrated = %u ticks/ms, divisor %d", apic_ticks_per_ms, apic_divisor);
}

void set_oneshot_timer(size_t deadline)
{
	deadline = std::max(deadline, size_t(1));
	bool use_tsc_deadline = test_feature(FEATURE_TSC_DEADLINE);

	if(use_tsc_deadline)
	{
		cpu::apic::timer_set_tsc_deadline(deadline);
		return;
	}

	const size_t now = get_time();

	if(now >= deadline)
	{
		cpu::apic::timer_set_oneshot(1, 1, false);
		return;
	}

	const size_t interval = deadline - now;
	const uint64_t apic_ticks_needed = interval * apic_ticks_per_ms;
	size_t ticks = apic_ticks_needed;

	uint32_t highest_set_bit = 0;

	while(ticks >>= 1)
	{
		highest_set_bit++;
	}

	uint8_t extra_shift = (highest_set_bit <= 31) ? 0 : static_cast<uint8_t>(highest_set_bit - 31);

	if(extra_shift > 8)
	{
		extra_shift = 8;
	}

	uint32_t divisor = apic_divisor << extra_shift;
	uint32_t count = 0;

	if(divisor <= 128)
	{
		count = static_cast<uint32_t>(apic_ticks_needed >> extra_shift);
	}
	else
	{
		divisor = 128;
		count = UINT32_MAX;
	}

	if(count == 0)
	{
		count = 1;
	}

	cpu::apic::timer_set_oneshot(count, static_cast<uint8_t>(divisor), false);
}

void stop_timer()
{
	bool use_tsc_deadline = test_feature(FEATURE_TSC_DEADLINE);

	if(use_tsc_deadline)
	{
		cpu::apic::timer_set_tsc_deadline(0);
	}
	else
	{
		cpu::apic::timer_stop();
	}
}

void apic_timer(uint8_t vector, size_t ms, cpu::apic::TimerModes mode)
{
	using namespace cpu;

	if(apic_ticks_per_ms == 0)
	{
		calibrate_apic_timer();
	}

	size_t ticks = apic_ticks_per_ms * ms;
	apic::set_timer(vector, ticks, mode);
}
} // namespace timers
} // namespace drivers