# Universal Asynchronous Receiver-Transmitter (UART)

It is a peripheral device for asynchronous serial communication in which the data format and transmission speeds are configurable. It sends data bits one by one, from the least significant to the most significant, framed by start and stop bits so that precise timing is handled by the communication channel.

## Port Addresses

| COM Port | IO Base |
| -------- | ------- |
| COM1     | 0x3f8   |
| COM2     | 0x2f8   |
| COM3     | 0x3e8   |
| COM4     | 0x2e8   |

| IO Port Offset | Setting for DLAB | I/O Access | Register Mapped to this port                                                                          |
| -------------- | ---------------- | ---------- | ----------------------------------------------------------------------------------------------------- |
| +0             | 0                | Read       | Recieve Buffer                                                                                        |
| +0             | 0                | Write      | Transmit Buffer                                                                                       |
| +1             | 0                | Read/Write | Interrupt Enable Register                                                                             |
| +0             | 1                | Read/Write | With DLAB set to 1, this is the least significant byte of the divisor value for setting the baud rate |
| +1             | 1                | Read/Write | With DLAB set to 1, this is the most significant byte of the divisor value                            |
| +2             | -                | Read       | Interrupt identification                                                                              |
| +2             | -                | Write      | FIFO Control registers                                                                                |
| +3             | -                | Read/Write | Line Control Register                                                                                 |
| +4             | -                | Read/Write | Modem Control Register                                                                                |
| +5             | -                | Read       | Line Status Register                                                                                  |
| +6             | -                | Read       | Modem Status Register                                                                                 |
| +7             | -                | Read/Write | Scratch Register                                                                                      |

## Line Protocol

The serial data transmitted across the wire can have a number of different parameters set. The sending device and the recieving device require the same protocol parameters values written to each serial controller in order for the communication to be successful.

## Baud Rate

The serial controller has an internal clock which runs at 115200 ticks per second (Baud) and a clock divisor which is used to control the baud rate.

In order to set the speed of the port, calculate the divisor required for the given baud rate and program that in to the divisor register. For example, a divisor of 3 will give 38400 baud.

## Line Control Register

| Bit 7                    | Bit 6            | Bit 5-3     | Bit 2     | Bit 1-0   |
| ------------------------ | ---------------- | ----------- | --------- | --------- |
| Divisor Latch Access Bit | Break Enable Bit | Parity Bits | Stop Bits | Data Bits |

### Data Bits

The number of bits in a character is variable. Set this value by writing to the two least significant bits of the Line Control Register.

| Bit 1 | Bit 0 | Character length (bits) |
| ----- | ----- | ----------------------- |
| 0     | 0     | 5                       |
| 0     | 1     | 6                       |
| 1     | 0     | 7                       |
| 1     | 1     | 8                       |

### Stop Bits

The serial controller can be configured to send a number of bits after each character of data. These reliable bits can be used by the controller to verify that the sending and recieving devices are in phase.

| Bit 2 | Stop Bits |
| ----- | --------- |
| 0     | 1         |
| 1     | 1.5 or 2  |

### Parity Bits

The controller can be made to add or expect a parity bit at the end of each character of data transmitted. With this parity bit, if a single bit of data is inverted by interference, a parity error can be raised.

If parity is set to NONE, no parity bit will be added and none will be expected. If one is sent by the transmitter and not expected by the receiver, it will likely cause an error.

If the parity is MARK or SPACE, the parity bit will be expected to be always set to 1 or 0 respectively.

If the parity is set to EVEN or ODD, the controller calculates the accuracy of the parity by adding together the values of all the data bits and the parity bit. If the port is set to have EVEN parity, the result must be even. If it is set to have ODD parity, the result must be odd.

| Bit 5 | Bit 4 | Bit 3 | Parity |
| ----- | ----- | ----- | ------ |
| 0     | 0     | 0     | NONE   |
| 0     | 0     | 1     | ODD    |
| 0     | 1     | 1     | EVEN   |
| 1     | 0     | 0     | MARK   |
| 1     | 1     | 1     | SPACE  |

## Interrupt Enable Register

To communicate with a serial port in interrupt mode, the interrupt-enable-register must be set correctly.

| Bits 7-4 | Bit 3        | Bit 2                | Bit 1                              | Bit 0                   |
| -------- | ------------ | -------------------- | ---------------------------------- | ----------------------- |
| Reserved | Modem Status | Reciever Line Status | Transmitter Holding Register Empty | Recieved Data Available |

## First In First Out Control Register

The First In/First Out Control Register(FCR) is for controlling the FIFO buffers.

| Bits 7-6                | Bits 5-4 | Bit 3           | Bit 2               | Bit 1              | Bit 0         |
| ----------------------- | -------- | --------------- | ------------------- | ------------------ | ------------- |
| Interrupt Trigger Level | Reserved | DMA Mode Select | Clear Transmit FIFO | Clear Receive FIFO | Enable FIFO's |

### Interrupt Trigger Level

The Interrupt Trigger Level is used to configure how much data must be received in the FIFO Receive buffer before triggering a Received Data Available Interrupt.

| Bit 7 | Bit 6 | Trigger Level |
| ----- | ----- | ------------- |
| 0     | 0     | 1 Byte        |
| 0     | 1     | 4 Bytes       |
| 1     | 0     | 8 Bytes       |
| 1     | 1     | 14 Bytes      |

## Interrupt Identification Register

The Interrupt Identification Register (IIR) is for identifying pending interrupts.

| Bits 7-6          | Bits 5-4 | Bit 3                                              | Bits 2-1        | Bit 0             |
| ----------------- | -------- | -------------------------------------------------- | --------------- | ----------------- |
| FIFO Buffer State | Reserved | Timeout Interrupt Pending (UART 16550) or Reserved | Interrupt State | Interrupt Pending |

### Interrupt State

After Interrupt Pending is set, the Interrupt State shows the interrupt that has occurred.

| Bit 2 | Bit 1 | Interrupt                          | Priority    |
| ----- | ----- | ---------------------------------- | ----------- |
| 0     | 0     | Modem Status                       | 4 (Lowest)  |
| 0     | 1     | Transmitter Holding Register Empty | 3           |
| 1     | 0     | Received Data Available            | 2           |
| 1     | 1     | Receiver Line Status               | 1 (Highest) |

### FIFO Buffer State

| Bit 7 | Bit 6 | State                     |
| ----- | ----- | ------------------------- |
| 0     | 0     | No FIFO                   |
| 0     | 1     | FIFO Enabled but Unusable |
| 1     | 0     | FIFO Enabled              |

## Modem Control Register

| Bit | Name                      | Meaning                                                                              |
| --- | ------------------------- | ------------------------------------------------------------------------------------ |
| 0   | Data Terminal Ready (DTR) | Controls the Data Terminal Ready Pin                                                 |
| 1   | Request to Send (RTS)     | Controls the Request to Send Pin                                                     |
| 2   | Out 1                     | Controls a hardware pin (OUT1) which is unused in PC implementations                 |
| 3   | Out 2                     | Controls a hardware pin (OUT2) which is used to enable the IRQ in PC implementations |
| 4   | Loop                      | Provides a local loopback feature for diagnostic testing of the UART                 |
| 5   | 0                         | Unused                                                                               |
| 6   | 0                         | Unused                                                                               |
| 7   | 0                         | Unused                                                                               |

## Line Status

| Bit | Name                                      | Meaning                                                             |
| --- | ----------------------------------------- | ------------------------------------------------------------------- |
| 0   | Data ready (DR)                           | Set if there is data that can be read                               |
| 1   | Overrun error (OE)                        | Set if there has been data lost                                     |
| 2   | Parity error (PE)                         | Set if there was an error in the transmission as detected by parity |
| 3   | Framing error (FE)                        | Set if a stop bit was missing                                       |
| 4   | Break indicator (BI)                      | Set if there is a break in data input                               |
| 5   | Transmitter holding register empty (THRE) | Set if the transmission buffer is empty (i.e. data can be sent)     |
| 6   | Transmitter empty (TEMT)                  | Set if the transmitter is not doing anything                        |
| 7   | Impending Error                           | Set if there is an error with a word in the input buffer            |

## Modem Status Register

| Bit | Name                                   | Meaning                                                                     |
| --- | -------------------------------------- | --------------------------------------------------------------------------- |
| 0   | Delta Clear to Send (DCTS)             | Indicates that CTS input has changed state since the last time it was read  |
| 1   | Delta Data Set Ready (DDSR)            | Indicates that DSR input has changed state since the last time it was read  |
| 2   | Trailing Edge of Ring Indicator (TERI) | Indicates that RI input to the chip has changed from a low to a high state  |
| 3   | Delta Data Carrier Detect (DDCD)       | Indicates that DCD input has changed state since the last time it ware read |
| 4   | Clear to Send (CTS)                    | Inverted CTS Signal                                                         |
| 5   | Data Set Ready (DSR)                   | Inverted DSR Signal                                                         |
| 6   | Ring Indicator (RI)                    | Inverted RI Signal                                                          |
| 7   | Data Carrier Detect (DCD)              | Inverted DCD Signal                                                         |

## Printing to Serial

We use `inb` and `outb` instructions to communicate with the serial port. But first, the kernel needs to initialize the serial device before sending/recieving data.

### Writing to UART Register

We employ `write_register` to write value, `val`, to a UART register with IO port base `uart_port` and offset `reg`.

```c++
inline void write_register(uint16_t reg, uint8_t val)
{
	arch::outp(uart_port + reg, val);
}
```

We employ `read_register` to read value from a UART register with IO port base `uart_port` and offset `reg`.

```c++
inline uint8_t read_register(uint16_t reg)
{
	return arch::inp<uint8_t>(uart_port + reg);
}
```

### Sending data to UART

To send a register to UART, we read from Line Status Register [IO Base + 5], and check whether the `Transmitter Holding Register Empty (THE)` is set. If unset, loop until the bit is set. To send a data to UART, transmit the data to `Transmit Buffer`.

```c++
int putc(int c)
{
	while(!(read_register(UART_LINE_STATUS) & UART_LINE_TRANSMITTER_BUF_EMPTY))
	{
		pause();
	}

	write_register(UART_DATA, c);
	return SYSTEM_OK;
}
```

## References

1. [Serial Ports - OSDEV wiki](https://wiki.osdev.org/Serial_Ports)
2. [Serial Programming - Wikibooks](https://en.wikibooks.org/wiki/Serial_Programming)