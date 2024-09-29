#ifndef LOGGER_H
#define LOGGER_H 1

#include <stddef.h>
#include <stdint.h>
#include <sys/defs.h>
#include <libs/styling.h>

#define LOG_LEVEL_TRACE (0)
#define LOG_LEVEL_INFO (1)
#define LOG_LEVEL_DEBUG (2)
#define LOG_LEVEL_WARNING (3)
#define LOG_LEVEL_ERROR (4)
#define LOG_LEVEL_PANIC (5)

struct LogStyle
{
	const char* colors[6];
	const char* tags[6];
};

#ifndef DEFAULT_TEXT_COLOR
	// Works for my terminal background; Will change when switching to virtual terminal
	// Generate using ChatGPT so :/
	#define DEFAULT_TEXT_COLOR                                    \
		{                                                         \
			TEXT_COLOR(152, 251, 152), /* Page Green */           \
			TEXT_COLOR(102, 255, 102), /* Light Green */          \
			TEXT_COLOR(173, 216, 230), /* Light Blue */           \
			TEXT_COLOR(255, 255, 102), /* Bright, Light Yellow */ \
			TEXT_COLOR(255, 165, 0), /* Bright Orange */          \
			TEXT_COLOR(255, 77, 77), /* Light Red */              \
		}
#endif // DEFAULT_TEXT_COLOR

#ifndef DEFAULT_LEVEL_TAG
	#define DEFAULT_LEVEL_TAG                         \
		{                                             \
			"[T]", "[I]", "[D]", "[W]", "[E]", "[P]", \
		}
#endif

__CDECLS_BEGIN

void log_set_style(struct LogStyle __style);
void log_msg(int __log_level, const char* __restrict __format, ...) __PRINTFLIKE(2, 3);

// Note: Use this at the end of panic messages to halt the kernel
void log_panik(const char* __restrict __format, ...) __PRINTFLIKE(1, 2);

// To be used at the start of a component to be initialized,
// to be used in pair with `log_end_initialization`
void log_begin_intialization(const char* __restrict __component);
void log_end_intialization(void);

__CDECLS_END

#define log_trace(__format, ...) log_msg(LOG_LEVEL_TRACE, __format, ##__VA_ARGS__)
#define log_info(__format, ...) log_msg(LOG_LEVEL_INFO, __format, ##__VA_ARGS__)
#define log_debug(__format, ...) log_msg(LOG_LEVEL_DEBUG, __format, ##__VA_ARGS__)
#define log_warning(__format, ...) log_msg(LOG_LEVEL_WARNING, __format, ##__VA_ARGS__)
#define log_error(__format, ...) log_msg(LOG_LEVEL_ERROR, __format, ##__VA_ARGS__)
#define log_panic(__format, ...) log_msg(LOG_LEVEL_PANIC, __format, ##__VA_ARGS__)

#endif // LOGGER_H