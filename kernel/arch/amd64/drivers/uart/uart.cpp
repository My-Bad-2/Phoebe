#include <arch.hpp>

#include <drivers/uart.hpp>
#include "uart_defs.h"

namespace drivers
{
namespace uart
{
uint16_t uart_port = 0;

inline void write_register(uint16_t reg, uint8_t val)
{
	arch::outp(uart_port + reg, val);
}

inline uint8_t read_register(uint16_t reg)
{
	return arch::inp<uint8_t>(uart_port + reg);
}

int putc(int c)
{
	while(!(read_register(UART_LINE_STATUS) & UART_LINE_TRANSMITTER_BUF_EMPTY))
	{
		pause();
	}

	write_register(UART_DATA, c);
	return SYSTEM_OK;
}

void set_port(uint16_t port)
{
	uart_port = port;
}

error_t initialize()
{
	write_register(UART_INTERRUPT_IDENTIFACTOR, 0);

	// Enable DLAB and set baud rate divisor
	write_register(UART_LINE_CONTROL, UART_LINE_DLAB_STATUS);
	write_register(UART_BAUD_RATE_LOW, 3);
	write_register(UART_BAUD_RATE_HIGH, 0);

	// Configure for 8 bits, no parity, and one stop bit
	write_register(UART_LINE_CONTROL, UART_LINE_DS_8);

	// Enable FIFO, clear them with 14-byte threshold
	write_register(UART_FIFO_CONTROLLER, UART_ENABLE_FIFO | UART_FIFO_CLEAR_RECEIVE |
											 UART_FIFO_CLEAR_TRANSMIT | UART_FIFO_TRIGGER_LEVEL4);

	// Enable IRQs, set RTS/DSR, and set in loopback mode
	write_register(UART_MODEM_CONTROL,
				   UART_MODEM_RTS | UART_MODEM_DTR | UART_MODEM_OUT2 | UART_MODEM_LOOPBACK);

	// Test serial chip by seding one byte
	write_register(UART_DATA, 0xae);

	if(read_register(UART_DATA) != 0xae)
	{
		return SYSTEM_ERR_IO_INVALID;
	}

	write_register(UART_MODEM_CONTROL,
				   UART_MODEM_RTS | UART_MODEM_DTR | UART_MODEM_OUT1 | UART_MODEM_OUT2);

	return SYSTEM_OK;
}
} // namespace uart
} // namespace drivers