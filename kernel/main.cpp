#include <drivers/drivers.hpp>
#include <arch.hpp>
#include <logger.h>
#include <memory/memory.hpp>

__CDECLS_BEGIN

__NO_RETURN void kernel_main()
{
	drivers::early_initialize();

	LogStyle style = {
		DEFAULT_TEXT_COLOR,
		DEFAULT_LEVEL_TAG,
	};

	log_set_style(style);

	memory::initialize();
	arch::initialize();
	drivers::initialize();
	
	arch::late_initialize();
	drivers::late_initialize();

	log_info("Hello, World!");

	arch::halt(true);
}

__CDECLS_END