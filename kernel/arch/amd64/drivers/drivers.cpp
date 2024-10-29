#include <stdio.h>
#include <limits.h>

#include <drivers/pit.hpp>
#include <drivers/uart.hpp>
#include <drivers/apic_timer.hpp>

#include <cpu/features.h>
#include <cpu/lapic.hpp>

FILE* stdin = nullptr;
FILE* stdout = nullptr;
FILE* stderr = nullptr;

namespace drivers
{
void initialize_streams()
{
	static FILE simple_file = {
		.flags = 0,
		.buffer_size = INT_MAX,
		.position = 0,
		.buffer = nullptr,
		.write_func = [](int c) -> int {
			return uart::putc(c);
		},
	};

	stdin = &simple_file;
	stdout = &simple_file;
	stderr = &simple_file;
}

void arch_initialize()
{
	uart::set_port(UART_COM_PORT_1);

	if(uart::initialize() != SYSTEM_OK)
	{
	}

	initialize_streams();
}

void late_initialize()
{
	bool use_tsc_deadline = test_feature(FEATURE_TSC_DEADLINE);

	timers::initialize_pit();

	if(use_tsc_deadline)
	{
		cpu::apic::initialize_timer_tsc_deadline();
	}
	else
	{
		timers::calibrate_apic_timer();
	}
}
} // namespace drivers