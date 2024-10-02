#include <drivers/drivers.hpp>
#include <arch.hpp>
#include <logger.h>
#include <cpu/gdt.hpp>
#include <memory/physical.hpp>

__CDECLS_BEGIN

__NO_RETURN void kernel_main()
{
	drivers::initialize();

	LogStyle style = {
		DEFAULT_TEXT_COLOR,
		DEFAULT_LEVEL_TAG,
	};

	log_set_style(style);

	memory::physical_initialize();

	arch::initialize();

	log_info("Hello, World!");

	arch::halt(true);
}

__CDECLS_END