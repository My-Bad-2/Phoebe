#include <drivers/drivers.hpp>
#include <arch.hpp>
#include <logger.h>

__CDECLS_BEGIN

__NO_RETURN void kernel_main()
{
	drivers::initialize();

	LogStyle style = {
		DEFAULT_TEXT_COLOR,
		DEFAULT_LEVEL_TAG,
	};

	log_set_style(style);

	log_trace("Hello, World!");
	log_info("Hello, World!");
	log_debug("Hello, World!");
	log_warning("Hello, World!");
	log_error("Hello, World!");
	log_panic("Hello, World!");

	// Halt the kernel
	log_panik("Hello, World!");

	arch::halt(true);
}

__CDECLS_END