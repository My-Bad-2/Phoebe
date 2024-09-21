#include <errno.h>
#include <stdio.h>

int putc(int c, FILE* stream)
{
	int ret = 0;
	size_t write_pos = stream->position++;

	if(write_pos >= stream->buffer_size)
	{
		ret = SYSTEM_ERR_BUFFER_TOO_SMALL;
	}

	if(stream->write_func != NULL)
	{
		ret = stream->write_func(c);
	}
	else
	{
		stream->buffer[write_pos] = c;
		ret = SYSTEM_OK;
	}

	return ret;
}

int putchar(int c)
{
	return putc(c, stdout);
}