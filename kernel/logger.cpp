#include <logger.h>
#include <arch.hpp>
#include <stdio.h>

namespace logger
{
LogStyle style = {};

const char* get_log_color(int log_level)
{
	return style.colors[log_level];
}

const char* get_level_tag(int log_level)
{
	return style.tags[log_level];
}

inline void vlog_msg(const char* color, const char* tag, const char* format, va_list args)
{
	fprintf(stderr, "%s%s -> ", color, tag);
	vfprintf(stderr, format, args);
	fputs(RESET_TEXT "\n", stderr);
}
} // namespace logger

void log_set_style(struct LogStyle style)
{
	logger::style = style;
}

void log_msg(int log_level, const char* __restrict format, ...)
{
	const char* color = logger::get_log_color(log_level);
	const char* tag = logger::get_level_tag(log_level);

	va_list args = {};
	va_start(args, format);

	logger::vlog_msg(color, tag, format, args);

	va_end(args);
}

void log_panik(const char* restrict format, ...)
{
	const char* color = logger::get_log_color(LOG_LEVEL_PANIC);
	const char* tag = logger::get_level_tag(LOG_LEVEL_PANIC);

	fprintf(stderr, "%s---------------- FATAL ERROR ----------------" RESET_TEXT "\n", color);

	va_list args = {};
	va_start(args, format);

	logger::vlog_msg(color, tag, format, args);

	va_end(args);

	fprintf(stderr, "%s-------------- STOPPING SYSTEM --------------" RESET_TEXT "\n", color);

	arch::halt(false);
}