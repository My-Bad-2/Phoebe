#include <drivers/uart.hpp>
#include <stdio.h>
#include <limits.h>

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
} // namespace drivers