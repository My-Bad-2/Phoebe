#include <uacpi/types.h>
#include <uacpi_libc.h>
#include <uacpi/status.h>
#include <uacpi/kernel_api.h>

#include <logger.h>

void uacpi_kernel_log(uacpi_log_level level, const uacpi_char* fmt)
{
	switch(level)
	{
		case UACPI_LOG_ERROR:
			log_error("[UACPI] -> %s", fmt);
			break;
		case UACPI_LOG_WARN:
			log_warning("[UACPI] -> %s", fmt);
			break;
		case UACPI_LOG_INFO:
			log_info("[UACPI] -> %s", fmt);
			break;
		case UACPI_LOG_TRACE:
			log_trace("[UACPI] -> %s", fmt);
			break;
		case UACPI_LOG_DEBUG:
			log_debug("[UACPI] -> %s", fmt);
			break;
	}
}

uacpi_status uacpi_kernel_handle_firmware_request(uacpi_firmware_request* request)
{
	switch(request->type)
	{
		case UACPI_FIRMWARE_REQUEST_TYPE_BREAKPOINT:
			log_warning("[UACPI] -> Ignoring AML breakpoint");
			break;
		case UACPI_FIRMWARE_REQUEST_TYPE_FATAL:
			log_error("[UACPI] -> Fatal firmware error: type = %hhx, code = %x, arg: %lx",
					  request->fatal.type, request->fatal.code, request->fatal.arg);
            break;
	}

    return UACPI_STATUS_OK;
}