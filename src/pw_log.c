#include <stdarg.h>
#include <stdio.h>

#include <pico/time.h>

#include "pw_log.h"

#define PW_LOG_EOL ("\n")

static pw_log_level_t log_level_global = LOG_LEVEL_NONE;

void pw_log_level_set(pw_log_level_t level)
{
	/* TODO: When I start using multi threading I should do an atomic
	 * store (int should be atomic but just to be safe) and possibly a
	 * memory barrier if I care that much.
	 */
	log_level_global = level;
}

void pw_log(pw_log_level_t level, const char *fmt, ...)
{
	va_list args;
	uint32_t ms;

	if (level < log_level_global || level == LOG_LEVEL_NONE) {
		return;
	}
	ms = to_ms_since_boot(get_absolute_time());
	printf("[%ums] ", ms);
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	puts(PW_LOG_EOL);
}
