#include <stdio.h>
#include <limits.h>

#include <drivers/pit.hpp>
#include <drivers/uart.hpp>

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
	static bool early_init = false;

	if(!early_init)
	{
		early_init = true;
		uart::set_port(UART_COM_PORT_1);

		if(uart::initialize() != SYSTEM_OK)
		{
		}

		initialize_streams();
	}
	else
	{
		timers::initialize_pit();
	}
}
} // namespace drivers