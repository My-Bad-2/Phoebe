#include "string.h"
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#define FLAG_ZERO_PAD (1 << 0)
#define FLAG_LEFT (1 << 1)
#define FLAG_PLUS (1 << 2)
#define FLAG_SPACE (1 << 6)
#define FLAG_HASH (1 << 4)
#define FLAG_UPPERCASE (1 << 5)
#define FLAG_CHAR (1 << 6)
#define FLAG_SHORT (1 << 7)
#define FLAG_INT (1 << 8)
#define FLAG_LONG (1 << 9)
#define FLAG_LONG_LONG (1 << 10)
#define FLAG_PRECISION (1 << 11)
#define FLAG_ADAPT_EXP (1 << 12)
#define FLAG_POINTER (1 << 13)
#define FLAG_SIGNED (1 << 14)

#define BASE_BINARY 2
#define BASE_OCTAL 8
#define BASE_DECIMAL 10
#define BASE_HEX 16

#define INT_BUFSIZE 32

static inline void append_termination_internal(FILE* stream)
{
	if((stream->write_func != NULL) || (stream->buffer_size == 0))
	{
		return;
	}

	if(stream->buffer == NULL)
	{
		return;
	}

	const size_t null_pos =
		(stream->position < stream->buffer_size) ? stream->position : stream->buffer_size - 1;
	stream->buffer[null_pos] = '\0';
}

uint32_t parse_flags_internal(const char* format)
{
	uint32_t flags = 0;

	while(true)
	{
		switch(*format)
		{
			case '0':
				flags |= FLAG_ZERO_PAD;
				format++;
				break;
			case '-':
				flags |= FLAG_LEFT;
				format++;
				break;
			case '+':
				flags |= FLAG_PLUS;
				format++;
				break;
			case ' ':
				flags |= FLAG_SPACE;
				format++;
				break;
			case '#':
				flags |= FLAG_HASH;
				format++;
				break;
			default:
				return flags;
		}
	}
}

void reverse_output(FILE* stream, const char* string, size_t length, size_t width, uint32_t flags)
{
	const size_t start_pos = stream->position;

	if(!(flags & FLAG_LEFT) && !(flags & FLAG_ZERO_PAD))
	{
		for(size_t i = length; i < width; i++)
		{
			putc(' ', stream);
		}
	}

	while(length)
	{
		putc(string[--length], stream);
	}

	if(flags & FLAG_LEFT)
	{
		while((stream->position - start_pos) < width)
		{
			putc(' ', stream);
		}
	}
}

void print_integer(FILE* stream, char* buffer, size_t len, bool negative, uint8_t base,
				   size_t precision, size_t width, uint32_t flags)
{
	size_t unpadded_length = len;

	if(!(flags & FLAG_LEFT))
	{
		if(width && (flags & FLAG_ZERO_PAD) && (negative || (flags & (FLAG_PLUS | FLAG_SPACE))))
		{
			width--;
		}

		while((flags & FLAG_ZERO_PAD) && (len < width) && (len < INT_BUFSIZE))
		{
			buffer[len++] = '0';
		}
	}

	while((len < precision) && (len < INT_BUFSIZE))
	{
		buffer[len++] = '0';
	}

	if((base == BASE_OCTAL) && (len > unpadded_length))
	{
		flags &= ~FLAG_HASH;
	}

	if(flags & (FLAG_HASH | FLAG_POINTER))
	{
		if(!(flags & FLAG_PRECISION) && len && ((len == precision) || (len == width)))
		{
			if(unpadded_length < len)
			{
				len--;
			}

			if(len && ((base == BASE_HEX) || (base == BASE_BINARY)) && (unpadded_length < len))
			{
				len--;
			}
		}

		if((base == BASE_HEX) && !(flags & FLAG_UPPERCASE) && (len < INT_BUFSIZE))
		{
			buffer[len++] = 'x';
		}
		else if((base == BASE_HEX) && (flags & FLAG_UPPERCASE) && (len < INT_BUFSIZE))
		{
			buffer[len++] = 'X';
		}
		else if((base == BASE_BINARY) && (len < INT_BUFSIZE))
		{
			buffer[len++] = 'b';
		}

		if(len < INT_BUFSIZE)
		{
			buffer[len++] = '0';
		}
	}

	if(len < INT_BUFSIZE)
	{
		if(negative)
		{
			buffer[len++] = '-';
		}
		else if(flags & FLAG_PLUS)
		{
			buffer[len++] = '+';
		}
		else if(flags & FLAG_SPACE)
		{
			buffer[len++] = ' ';
		}
	}

	reverse_output(stream, buffer, len, width, flags);
}

void print_int(FILE* stream, size_t value, bool negative, uint8_t base, size_t precision,
			   size_t width, uint32_t flags)
{
	char buffer[INT_BUFSIZE] = {};
	size_t len = 0;

	if(!value)
	{
		if(!(flags & FLAG_PRECISION))
		{
			buffer[len++] = '0';
			flags &= ~FLAG_HASH;
		}
		else if(base == BASE_HEX)
		{
			flags &= ~FLAG_HASH;
		}
	}
	else
	{
		do
		{
			const char digit = (char)(value % base);

			buffer[len++] =
				(char)((digit < 10) ? '0' + digit :
									  (flags & FLAG_UPPERCASE ? 'A' : 'a') + digit - 10);
			value /= base;
		} while(value && (len < INT_BUFSIZE));
	}

	print_integer(stream, buffer, len, negative, base, precision, width, flags);
}

void format_string(FILE* stream, const char* format, va_list args)
{
#define ADVANCE_IN_FORMAT_STRING(ptr) \
	do                                \
	{                                 \
		(ptr)++;                      \
		if(!*(ptr))                   \
		{                             \
			return;                   \
		}                             \
	} while(0)

	while(*format)
	{
		if(*format != '%')
		{
			putc(*format, stream);
			format++;
			continue;
		}

		ADVANCE_IN_FORMAT_STRING(format);

		uint32_t flags = parse_flags_internal(format);
		size_t width = 0;

		if(isdigit(*format))
		{
			width = (size_t)atou(format);
		}
		else if(*format == '*')
		{
			const int val = va_arg(args, int);

			if(val < 0)
			{
				flags |= FLAG_LEFT;
				width = (size_t)(-val);
			}
			else
			{
				width = (size_t)val;
			}

			ADVANCE_IN_FORMAT_STRING(format);
		}

		size_t precision = 0;

		if(*format == '.')
		{
			flags |= FLAG_PRECISION;
			ADVANCE_IN_FORMAT_STRING(format);

			if(*format == '-')
			{
				do
				{
					ADVANCE_IN_FORMAT_STRING(format);
				} while(isdigit(*format));

				flags &= ~FLAG_PRECISION;
			}
			else if(isdigit(*format))
			{
				precision = atou(format);
			}
			else if(*format == '*')
			{
				const int precise = va_arg(args, int);

				if(precise < 0)
				{
					flags &= ~FLAG_PRECISION;
				}
				else
				{
					precision = (precise > 0) ? (size_t)precise : 0;
				}

				ADVANCE_IN_FORMAT_STRING(format);
			}
		}

		switch(*format)
		{
			case 'l':
			{
				flags |= FLAG_LONG;
				ADVANCE_IN_FORMAT_STRING(format);

				if(*format == 'l')
				{
					flags |= FLAG_LONG_LONG;
					ADVANCE_IN_FORMAT_STRING(format);
				}

				break;
			}
			case 'h':
			{
				flags |= FLAG_SHORT;
				ADVANCE_IN_FORMAT_STRING(format);

				if(*format == 'h')
				{
					flags |= FLAG_CHAR;
					ADVANCE_IN_FORMAT_STRING(format);
				}

				break;
			}
			case 't':
			{
				if(sizeof(ptrdiff_t) <= sizeof(int))
				{
					flags = FLAG_INT;
				}
				else if(sizeof(ptrdiff_t) == sizeof(long))
				{
					flags = FLAG_LONG;
				}
				else
				{
					flags = FLAG_LONG_LONG;
				}

				ADVANCE_IN_FORMAT_STRING(format);
				break;
			}
			case 'j':
			{
				if(sizeof(intmax_t) == sizeof(long))
				{
					flags |= FLAG_LONG;
				}
				else
				{
					flags |= FLAG_LONG_LONG;
				}

				ADVANCE_IN_FORMAT_STRING(format);
				break;
			}
			case 'z':
			{
				if(sizeof(size_t) <= sizeof(int))
				{
					flags = FLAG_INT;
				}
				else if(sizeof(size_t) == sizeof(long))
				{
					flags = FLAG_LONG;
				}
				else
				{
					flags = FLAG_LONG_LONG;
				}

				ADVANCE_IN_FORMAT_STRING(format);
				break;
			}
			default:
				break;
		}

		switch(*format)
		{
			case 'd':
			case 'i':
			case 'u':
			case 'x':
			case 'X':
			case 'o':
			case 'b':
			{
				if(*format == 'd' || *format == 'i')
				{
					flags |= FLAG_SIGNED;
				}

				uint8_t base;
				if(*format == 'x' || *format == 'X')
				{
					base = BASE_HEX;
				}
				else if(*format == 'o')
				{
					base = BASE_OCTAL;
				}
				else if(*format == 'b')
				{
					base = BASE_BINARY;
				}
				else
				{
					base = BASE_DECIMAL;
					flags &= ~FLAG_HASH;
				}

				if(*format == 'X')
				{
					flags |= FLAG_UPPERCASE;
				}

				format++;
				if(flags & FLAG_PRECISION)
				{
					flags &= ~FLAG_ZERO_PAD;
				}

				if(flags & FLAG_SIGNED)
				{
					if(flags & FLAG_LONG_LONG)
					{
						const long long value = va_arg(args, long long);
						print_int(stream, llabs(value), value < 0, base, precision, width, flags);
					}
					else if(flags & FLAG_LONG)
					{
						const long value = va_arg(args, long);
						print_int(stream, llabs(value), value < 0, base, precision, width, flags);
					}
					else
					{
						int value = 0;

						if(flags & FLAG_CHAR)
						{
							value = (signed char)va_arg(args, int);
						}
						else if(flags & FLAG_CHAR)
						{
							value = (signed char)va_arg(args, int);
						}
						else if(flags & FLAG_SHORT)
						{
							value = (short int)va_arg(args, int);
						}
						else
						{
							value = va_arg(args, int);
						}

						print_int(stream, llabs(value), value < 0, base, precision, width, flags);
					}
				}
				else
				{
					flags &= ~(FLAG_PLUS | FLAG_SPACE);

					if(flags & FLAG_LONG_LONG)
					{
						print_int(stream, va_arg(args, unsigned long long), false, base, precision,
								  width, flags);
					}
					else if(flags & FLAG_LONG)
					{
						print_int(stream, va_arg(args, unsigned long), false, base, precision,
								  width, flags);
					}
					else
					{
						unsigned int value = 0;

						if(flags & FLAG_CHAR)
						{
							value = (unsigned char)va_arg(args, unsigned int);
						}
						else if(flags & FLAG_SHORT)
						{
							value = (unsigned short int)va_arg(args, unsigned int);
						}
						else
						{
							value = va_arg(args, unsigned int);
						}

						print_int(stream, value, false, base, precision, width, flags);
					}
				}
				break;
			}
			case 'c':
			{
				size_t l = 1U;

				if(!(flags & FLAG_LEFT))
				{
					while(l++ < width)
					{
						putc(' ', stream);
					}
				}

				putc((char)va_arg(args, int), stream);

				if(flags & FLAG_LEFT)
				{
					while(l++ < width)
					{
						putc(' ', stream);
					}
				}

				format++;
				break;
			}

			case 's':
			{
				const char* p = va_arg(args, char*);

				if(p == NULL)
				{
					reverse_output(stream, ")llun(", 6, width, flags);
				}
				else
				{
					size_t l = strnlen(p, precision ? precision : BUFSIZE);

					if(flags & FLAG_PRECISION)
					{
						l = (l < precision ? l : precision);
					}

					if(!(flags & FLAG_LEFT))
					{
						while(l++ < width)
						{
							putc(' ', stream);
						}
					}

					while((*p != 0) && (!(flags & FLAG_PRECISION) || precision))
					{
						putc(*(p++), stream);
						--precision;
					}

					if(flags & FLAG_LEFT)
					{
						while(l++ < width)
						{
							putc(' ', stream);
						}
					}
				}

				format++;
				break;
			}

			case 'p':
			{
				width = sizeof(void*) * 2U + 2;
				flags |= FLAG_ZERO_PAD | FLAG_POINTER;
				uintptr_t value = (uintptr_t)va_arg(args, void*);

				if(value == (uintptr_t)NULL)
				{
					reverse_output(stream, ")lin(", 5, width, flags);
				}
				else
				{
					print_int(stream, (size_t)value, false, BASE_HEX, precision, width, flags);
				}

				format++;
				break;
			}

			case '%':
			{
				putc('%', stream);
				format++;
				break;
			}

			default:
			{
				putc(*format, stream);
				format++;
				break;
			}
		}
	}
}

int vsnprintf_internal(FILE* s, const char* format, va_list args)
{
	format_string(s, format, args);
	append_termination_internal(s);

	return s->position;
}