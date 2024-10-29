#include <arch.hpp>
#include <cpu/idt.hpp>
#include <drivers/interrupts.hpp>
#include <drivers/pit.hpp>

#define CHANNEL0 (0x40)
#define CHANNEL1 (0x41)
#define CHANNEL2 (0x42)
#define COMMAND_REGISTER (0x43)

// Read back status bytes
#define CHANNEL_0 (0 << 6)
#define CHANNEL_1 (1 << 6)
#define CHANNEL_2 (2 << 6)
#define READ_BACK_CMD (3 << 6)

#define ACCESS_LATCH (0 << 4)
#define ACCESS_LO_BYTE (1 << 4)
#define ACCESS_HI_BYTE (2 << 4)
#define ACCESS_LO_HI_BYTE (3 << 4)

#define MODE0 (0 << 1) // Interrupt on terminal count
#define MODE1 (1 << 1) // Hardware re-triggerable one-shot
#define MODE2 (2 << 1) // rate generator
#define MODE3 (3 << 1) // square wave generator
#define MODE4 (4 << 1) // software triggered strobe
#define MODE5 (5 << 1) // hardware triggered strobe

#define BINARY (0)
#define BCD (1)

#define INTERNAL_FREQ 1193182U
#define INTERNAL_FREQ_3X 3579546U

#define INTERNAL_FREQ_TICKS_PER_MS (INTERNAL_FREQ / 1000)

namespace drivers
{
namespace timers
{
static volatile uint64_t pit_ticks = 0;
static uint16_t pit_divisor;

inline void pit_tick()
{
	pit_ticks += 1;
}

void set_pit_freq(uint32_t freq)
{
	uint32_t count, remainder;

	if(freq <= 18)
	{
		count = 0xffff;
	}
	else if(freq >= INTERNAL_FREQ)
	{
		count = 1;
	}
	else
	{
		count = INTERNAL_FREQ_3X / freq;
		remainder = INTERNAL_FREQ_3X % freq;

		if(remainder >= INTERNAL_FREQ_3X / 2)
		{
			count++;
		}

		count /= 3;
		remainder = count % 3;

		if(remainder >= 1)
		{
			count++;
		}
	}

	pit_divisor = count & 0xffff;

	arch::outp<uint8_t>(COMMAND_REGISTER, MODE2 | ACCESS_LO_HI_BYTE);
	arch::outp<uint8_t>(CHANNEL0, static_cast<uint8_t>(pit_divisor));
	arch::outp<uint8_t>(CHANNEL0, static_cast<uint8_t>(pit_divisor >> 8));
}

void pit_calibration_precycle(uint16_t ms)
{
	const uint16_t init_pic_count = static_cast<uint16_t>(INTERNAL_FREQ_TICKS_PER_MS * ms);

	arch::outp<uint8_t>(COMMAND_REGISTER, MODE0 | ACCESS_LO_HI_BYTE);
	arch::outp<uint8_t>(CHANNEL0, static_cast<uint8_t>(init_pic_count));
}

void pit_calibration_cycle(uint16_t ms)
{
	const uint16_t init_pic_count = static_cast<uint16_t>(INTERNAL_FREQ_TICKS_PER_MS * ms);
	arch::outp<uint8_t>(CHANNEL0, static_cast<uint8_t>(init_pic_count >> 8));

	uint8_t status = 0;
	do
	{
		arch::outp<uint8_t>(COMMAND_REGISTER, ACCESS_HI_BYTE | MODE1 | READ_BACK_CMD);
		status = arch::inp<uint8_t>(CHANNEL0);
	} while((status & 0xc0) != 0x80);
}

void pit_calibration_cycle_cleanup()
{
	arch::outp<uint8_t>(COMMAND_REGISTER, MODE4 | ACCESS_LO_HI_BYTE);
}

void initialize_pit()
{
	const uint32_t desired_freq = 1000;

	log_begin_intialization("Programmable Interval Timer");

	set_pit_freq(desired_freq);
	auto [handler, vector] = drivers::interrupts::allocate_handler(IRQ_SYSTEM_TIMER);

	handler.set([](auto) {
		pit_tick();
	});

	log_end_intialization();
}

void pit_sleep(uint32_t msec)
{
	const uint64_t target_ticks = pit_ticks + msec;

	while(pit_ticks < target_ticks)
	{
		pause();
	}
}

size_t get_time()
{
	return pit_ticks;
}

void tick()
{
	pit_tick();
}

void sleep(size_t ms)
{
	return pit_sleep(ms);
}
} // namespace timers
} // namespace drivers