#include <stdio.h>

int putc(int c, FILE* stream)
{
	return stream->write_func(c);
}

int putchar(int c)
{
	return putc(c, stdout);
}