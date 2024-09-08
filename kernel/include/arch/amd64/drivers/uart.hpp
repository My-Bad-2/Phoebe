#ifndef DRIVERS_UART_HPP
#define DRIVERS_UART_HPP 1

#include <stdint.h>
#include <errno.h>

#define UART_COM_PORT_1 0x3f8
#define UART_COM_PORT_2 0x2f8
#define UART_COM_PORT_3 0x3e8
#define UART_COM_PORT_4 0x2e8

namespace drivers
{
namespace uart
{
error_t initialize();
void set_port(uint16_t port);

int putc(int c);
} // namespace uart
} // namespace drivers

#endif // DRIVERS_UART_HPP