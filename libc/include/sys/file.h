#ifndef _SYS_FILE_H
#define _SYS_FILE_H 1

#include <stdint.h>

struct __FILE
{
	int flags;

	unsigned long buffer_size;
	unsigned long position;
	char* buffer;

	int (*write_func)(int __c);
};

typedef struct __FILE FILE;

#endif // _SYS_FILE_H