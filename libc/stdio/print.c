#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>

#define INTEGER_BUFFER_SIZE 32
#define MAX_POSSIBLE_BUFFER_SIZE INT_MAX
#define STDIO_CONCATENATE(s1, s2) __STRINGIFY(s1, s2)

#define FLAGS_ZEROPAD (1U << 0U)
#define FLAGS_LEFT (1U << 1U)
#define FLAGS_PLUS (1U << 2U)
#define FLAGS_SPACE (1U << 3U)
#define FLAGS_HASH (1U << 4U)
#define FLAGS_UPPERCASE (1U << 5U)
#define FLAGS_CHAR (1U << 6U)
#define FLAGS_SHORT (1U << 7U)
#define FLAGS_INT (1U << 8U)

#define FLAGS_LONG (1U << 9U)
#define FLAGS_LONG_LONG (1U << 10U)
#define FLAGS_PRECISION (1U << 11U)
#define FLAGS_ADAPT_EXP (1U << 12U)
#define FLAGS_POINTER (1U << 13U)

#define FLAGS_SIGNED (1U << 14U)
#define FLAGS_LONG_DOUBLE (1U << 15U)

#define FLAGS_INT8 FLAGS_CHAR
#define FLAGS_INT16 FLAGS_SHORT
#define FLAGS_INT32 FLAGS_INT
#define FLAGS_INT64 FLAGS_LONG

typedef unsigned int stdio_flags_t;

#define BASE_BINARY 2
#define BASE_OCTAL 8
#define BASE_DECIMAL 10
#define BASE_HEX 16

typedef uint8_t stdio_base_t;

#define SIGN(neg, x) ((neg) ? -(x) : (x))
#define STDIO_ABS(x) ((unsigned long)((x) > 0 ? (x) : -((unsigned long)x)))

// Internal ASCII string to size_t conversion
static inline size_t stdio_atou(const char** str)
{
	size_t val = 0U;

	while(isdigit(**str))
	{
		val = val * 10U + (size_t)(*((*str)++) - '0');
	}

	return val;
}

static inline void append_termination(FILE* fp)
{
	if((fp->write_func != NULL) || (fp->buffer_size == 0))
	{
		return;
	}

	if(fp->buffer == NULL)
	{
		return;
	}

	size_t null_pos = (fp->position < fp->buffer_size) ? fp->position : fp->buffer_size - 1;
	fp->buffer[null_pos] = '\0';
}

static error_t output_reverse(FILE* fp, const char* buffer, size_t len, size_t width,
							  stdio_flags_t flags)
{
	const size_t start_pos = fp->position;

	// pad spaces up to giver width
	if(!(flags & FLAGS_LEFT) && !(flags & FLAGS_ZEROPAD))
	{
		for(size_t i = len; i < width; i++)
		{
			putc(' ', fp);
		}
	}

	// reverse string
	while(len)
	{
		putc(buffer[--len], fp);
	}

	// append pad spaces up to given width
	if(flags & FLAGS_LEFT)
	{
		while((fp->position - start_pos) < width)
		{
			putc(' ', fp);
		}
	}

	return SYSTEM_OK;
}

static error_t finalize_integer(FILE* fp, char* buffer, size_t len, bool negative,
								stdio_base_t base, size_t precision, size_t width,
								stdio_flags_t flags)
{
	size_t unpadded_len = len;

	// pad with leading zeros
	{
		if(!(flags & FLAGS_LEFT))
		{
			if(width && (flags & FLAGS_ZEROPAD) &&
			   (negative || (flags & (FLAGS_PLUS | FLAGS_SPACE))))
			{
				width--;
			}

			while((flags & FLAGS_ZEROPAD) && (len < width) && (len < INTEGER_BUFFER_SIZE))
			{
				buffer[len++] = '0';
			}
		}

		while((len < precision) && (len < INTEGER_BUFFER_SIZE))
		{
			buffer[len++] = '0';
		}

		if((base == BASE_OCTAL) && (len > unpadded_len))
		{
			// Since we've written some zeros, we've satisfied
			// the alternative format leading space requirement
			flags &= ~FLAGS_HASH;
		}
	}

	// Handle alternative format (hash)
	if(flags & (FLAGS_HASH | FLAGS_POINTER))
	{
		if(!(flags & FLAGS_PRECISION) && len && ((len == precision) || (len == width)))
		{
			// Take back some padding digits to fit in what will be the format-specific prefix
			if(unpadded_len < len)
			{
				// this should suffice for BASE_OCTAL
				len--;
			}

			if(len && (base == BASE_HEX || base == BASE_BINARY) && (unpadded_len < len))
			{
				// and an extra for 0x or 0b
				len--;
			}
		}

		if((base == BASE_HEX) && !(flags & FLAGS_UPPERCASE) && (len < INTEGER_BUFFER_SIZE))
		{
			buffer[len++] = 'x';
		}
		else if((base == BASE_HEX) && (flags & FLAGS_UPPERCASE) && (len < INTEGER_BUFFER_SIZE))
		{
			buffer[len++] = 'X';
		}
		else if((base == BASE_BINARY) && (len < INTEGER_BUFFER_SIZE))
		{
			buffer[len++] = 'b';
		}

		if(len < INTEGER_BUFFER_SIZE)
		{
			buffer[len++] = '0';
		}
	}

	if(len < INTEGER_BUFFER_SIZE)
	{
		if(negative)
		{
			buffer[len++] = '-';
		}
		else if(flags & FLAGS_PLUS)
		{
			// Ignore the space if the `+` exists
			buffer[len++] = '+';
		}
		else if(flags & FLAGS_SPACE)
		{
			buffer[len++] = ' ';
		}
	}

	return output_reverse(fp, buffer, len, width, flags);
}

static error_t print_integer(FILE* fp, unsigned long value, bool negative, stdio_base_t base,
							 size_t precision, size_t width, stdio_flags_t flags)
{
	char buffer[INTEGER_BUFFER_SIZE];
	size_t len = 0;

	if(!value)
	{
		if(!(flags & FLAGS_PRECISION))
		{
			buffer[len++] = '0';

			// Drop this flag since either the alternative and regular modes
			// of the specifier don't differ on 0 values, or we've already provided
			// the special handling for this mode.
			flags &= ~FLAGS_HASH;
		}
		else if(base == BASE_HEX)
		{
			// Drop this flags since both alternative and regular modes of the specifier
			// don't differ on 0 values
			flags &= ~FLAGS_HASH;
		}
	}
	else
	{
		do
		{
			const char digit = (char)(value % base);

			if(digit < 10)
			{
				buffer[len++] = (char)('0' + digit);
			}
			else
			{
				buffer[len++] = (char)((flags & FLAGS_UPPERCASE ? 'A' : 'a') + digit - 10);
			}

			value /= base;
		} while(value && (len < INTEGER_BUFFER_SIZE));
	}

	return finalize_integer(fp, buffer, len, negative, base, precision, width, flags);
}

// Advances the format pointer past the flags, and returns the parsed flags
// due to the characters passed
static stdio_flags_t parse_flags(const char** format)
{
	stdio_flags_t flags = 0U;

	do
	{
		switch(**format)
		{
			case '0':
				flags |= FLAGS_ZEROPAD;
				(*format)++;
				break;
			case '-':
				flags |= FLAGS_LEFT;
				(*format)++;
				break;
			case '+':
				flags |= FLAGS_PLUS;
				(*format)++;
				break;
			case ' ':
				flags |= FLAGS_SPACE;
				(*format)++;
				break;
			case '#':
				flags |= FLAGS_HASH;
				(*format)++;
				break;
			default:
				return flags;
		}
	} while(true);
}

static inline error_t format_string(FILE* fp, const char* format, va_list args)
{
#define ADVANCE_IN_FORMAT_STRING(ptr)   \
	do                                  \
	{                                   \
		(ptr)++;                        \
		if(!*(ptr))                     \
		{                               \
			return SYSTEM_ERR_CANCELED; \
		}                               \
	} while(0)

	while(*format)
	{
		if(*format != '%')
		{
			// A regular content character
			putc(*format, fp);
			format++;
			continue;
		}

		// We're parsing a format specifier: %[flags][width][.precision][length]
		ADVANCE_IN_FORMAT_STRING(format);

		stdio_flags_t flags = parse_flags(&format);

		// Evalue width field
		size_t width = 0U;

		if(isdigit(*format))
		{
			width = (size_t)stdio_atou(&format);
		}
		else if(*format == '*')
		{
			const int wide = va_arg(args, int);

			if(wide < 0)
			{
				flags |= FLAGS_LEFT; // reverse padding
				width = (size_t)-wide;
			}
			else
			{
				width = (size_t)wide;
			}

			ADVANCE_IN_FORMAT_STRING(format);
		}

		// evaluate precision field
		size_t precision = 0U;

		if(*format == '.')
		{
			flags |= FLAGS_PRECISION;
			ADVANCE_IN_FORMAT_STRING(format);

			if(*format == '-')
			{
				do
				{
					ADVANCE_IN_FORMAT_STRING(format);
				} while(isdigit(*format));

				flags &= ~FLAGS_PRECISION;
			}
			else if(isdigit(*format))
			{
				precision = stdio_atou(&format);
			}
			else if(*format == '*')
			{
				const int precision_ = va_arg(args, int);

				if(precision_ < 0)
				{
					flags &= ~FLAGS_PRECISION;
				}
				else
				{
					precision = precision_ > 0 ? (size_t)precision_ : 0U;
				}

				ADVANCE_IN_FORMAT_STRING(format);
			}
		}

		switch(*format)
		{
			case 'I':
			{
				ADVANCE_IN_FORMAT_STRING(format);
				// Greedily parse for size in bits: 8, 16, 32 or 64
				switch(*format)
				{
					case '8':
						flags |= FLAGS_INT8;
						ADVANCE_IN_FORMAT_STRING(format);

						break;
					case '1':
						ADVANCE_IN_FORMAT_STRING(format);

						if(*format == '6')
						{
							format++;
							flags |= FLAGS_INT16;
						}

						break;
					case '3':
						ADVANCE_IN_FORMAT_STRING(format);

						if(*format == '2')
						{
							ADVANCE_IN_FORMAT_STRING(format);
							flags |= FLAGS_INT32;
						}

						break;
					case '6':
						ADVANCE_IN_FORMAT_STRING(format);

						if(*format == '4')
						{
							ADVANCE_IN_FORMAT_STRING(format);
							flags |= FLAGS_INT64;
						}

						break;
					default:
						break;
				}
				break;
			}
			case 'l':
			{
				flags |= FLAGS_LONG;
				ADVANCE_IN_FORMAT_STRING(format);

				if(*format == 'l')
				{
					flags |= FLAGS_LONG_LONG;
					ADVANCE_IN_FORMAT_STRING(format);
				}

				break;
			}
			case 'h':
			{
				flags |= FLAGS_SHORT;
				ADVANCE_IN_FORMAT_STRING(format);

				if(*format == 'h')
				{
					flags |= FLAGS_CHAR;
					ADVANCE_IN_FORMAT_STRING(format);
				}

				break;
			}
			case 't':
			{
				flags |= (sizeof(ptrdiff_t) <= sizeof(int))	 ? FLAGS_INT :
						 (sizeof(ptrdiff_t) == sizeof(long)) ? FLAGS_LONG :
															   FLAGS_LONG_LONG;
				ADVANCE_IN_FORMAT_STRING(format);
				break;
			}
			case 'j':
			{
				flags |= (sizeof(intmax_t) == sizeof(long) ? FLAGS_LONG : FLAGS_LONG_LONG);
				ADVANCE_IN_FORMAT_STRING(format);
				break;
			}
			case 'z':
			{
				flags |= (sizeof(size_t) <= sizeof(int))  ? FLAGS_INT :
						 (sizeof(size_t) == sizeof(long)) ? FLAGS_LONG :
															FLAGS_LONG_LONG;
				ADVANCE_IN_FORMAT_STRING(format);
				break;
			}
			default:
				break;
		}

		// evaluate specifier
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
					flags |= FLAGS_SIGNED;
				}

				stdio_base_t base = 0;

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

					// decimal integers have no alternative presentation
					flags &= ~FLAGS_HASH;
				}

				if(*format == 'X')
				{
					flags |= FLAGS_UPPERCASE;
				}

				format++;

				// ignore '0' flag when precision is given
				if(flags & FLAGS_PRECISION)
				{
					flags &= ~FLAGS_ZEROPAD;
				}

				if(flags & FLAGS_SIGNED)
				{
					// A signed specifier: d, i or possibly I + bit size if enabled
					if(flags & FLAGS_LONG_LONG)
					{
						const long long value = va_arg(args, long long);
						print_integer(fp, STDIO_ABS(value), value < 0, base, precision, width,
									  flags);
					}
					else if(flags & FLAGS_LONG)
					{
						const long value = va_arg(args, long);
						print_integer(fp, STDIO_ABS(value), value < 0, base, precision, width,
									  flags);
					}
					else
					{
						// Never try to interpret the argument as something potentially-smaller than
						// int, due to integer promotion rules: Even if the user passed a short int,
						// short unsigned etc. - these will come in after promotion, as int's (or
						// unsigned for the case of short unsigned when it has the same size as int)
						const int value = (flags & FLAGS_CHAR)	? (signed char)va_arg(args, int) :
										  (flags & FLAGS_SHORT) ? (short int)va_arg(args, int) :
																  va_arg(args, int);

						print_integer(fp, STDIO_ABS(value), value < 0, base, precision, width,
									  flags);
					}
				}
				else
				{
					// An unsigned specifier: u, x, X, o, b
					flags &= ~(FLAGS_PLUS | FLAGS_SPACE);

					if(flags & FLAGS_LONG_LONG)
					{
						print_integer(fp, (unsigned long)va_arg(args, unsigned long long), false,
									  base, precision, width, flags);
					}
					else if(flags & FLAGS_LONG)
					{
						print_integer(fp, (unsigned long)va_arg(args, unsigned long), false, base,
									  precision, width, flags);
					}
					else
					{
						const unsigned int value =
							(flags & FLAGS_CHAR)  ? (unsigned char)va_arg(args, unsigned int) :
							(flags & FLAGS_SHORT) ? (unsigned short int)va_arg(args, unsigned int) :
													va_arg(args, int);

						print_integer(fp, STDIO_ABS(value), false, base, precision, width, flags);
					}
				}
				break;
			}

			case 'c':
			{
				size_t len = 1U;

				// pre-padding
				if(!(flags & FLAGS_LEFT))
				{
					while(len++ < width)
					{
						putc(' ', fp);
					}
				}

				// char output
				putc((char)va_arg(args, int), fp);

				// post padding
				if(flags & FLAGS_LEFT)
				{
					while(len++ < width)
					{
						putc(' ', fp);
					}
				}

				format++;
				break;
			}

			case 's':
			{
				const char* str = va_arg(args, char*);

				if(str == NULL)
				{
					output_reverse(fp, ")llun(", 0, width, flags);
				}
				else
				{
					size_t len = strnlen(str, precision ? precision : MAX_POSSIBLE_BUFFER_SIZE);

					// pre-padding
					if(flags & FLAGS_PRECISION)
					{
						len = (len < precision) ? len : precision;
					}

					if(!(flags & FLAGS_LEFT))
					{
						while(len++ < width)
						{
							putc(' ', fp);
						}
					}

					// string-output
					while((*str != '\0') && (!(flags & FLAGS_PRECISION) || precision))
					{
						putc(*(str++), fp);
						--precision;
					}

					// post padding
					if(flags & FLAGS_LEFT)
					{
						while(len++ < width)
						{
							putc(' ', fp);
						}
					}
				}
				format++;
				break;
			}

			case 'p':
			{
				width = sizeof(void*) * 2U + 2; // 2 hex chars per byte + the '0x' prefix
				flags |= FLAGS_ZEROPAD | FLAGS_POINTER;
				uintptr_t value = (uintptr_t)va_arg(args, void*);

				if(value == (uintptr_t)NULL)
				{
					output_reverse(fp, ")lin(", 5, width, flags);
				}
				else
				{
					print_integer(fp, (unsigned long)value, false, BASE_HEX, precision, width,
								  flags);
				}

				format++;
				break;
			}

			case '%':
			{
				putc('%', fp);
				format++;
				break;
			}

			case 'n':
			{
				// Do we want to implement brainfuck interpreter in the kernel? Hell yeah
				if(flags & FLAGS_CHAR)
				{
					*(va_arg(args, char*)) = (char)fp->position;
				}
				else if(flags & FLAGS_SHORT)
				{
					*(va_arg(args, short*)) = (short)fp->position;
				}
				else if(flags & FLAGS_LONG)
				{
					*(va_arg(args, long*)) = (long)fp->position;
				}
				else if(flags & FLAGS_LONG_LONG)
				{
					*(va_arg(args, long long*)) = (long long)fp->position;
				}
				else
				{
					*(va_arg(args, int*)) = (int)fp->position;
				}

				format++;
				break;
			}

			default:
			{
				putc(*format, fp);
				format++;
				break;
			}
		}
	}

	return SYSTEM_OK;
}

int vsnprintf_internal(FILE* fp, const char* format, va_list args)
{
	error_t err = format_string(fp, format, args);
	append_termination(fp);

	return err;
}