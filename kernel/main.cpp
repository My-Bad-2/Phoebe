#include <drivers/drivers.hpp>
#include <arch.hpp>
#include <stdio.h>

__CDECLS_BEGIN

__NO_RETURN void kernel_main()
{
	drivers::initialize();
	printf("Hello, World!\n");

	arch::halt(true);
}

__CDECLS_END