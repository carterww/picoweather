#ifndef _PICOWEATHER_LOG_H
#define _PICOWEATHER_LOG_H

#include "pw_cc.h"
#include "pw_cfg.h"

enum pw_log_level {
	LOG_LEVEL_TRACE = 0,
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARN,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_FATAL,
	LOG_LEVEL_NONE
};
typedef enum pw_log_level pw_log_level_t;

#if BUILD_TYPE == BUILD_DEBUG
void pw_log_level_set(pw_log_level_t level);

PW_ATTR_FORMAT(2, 3)
void pw_log(pw_log_level_t level, const char *fmt, ...);
#else
#define pw_log_level(level) ((void)0)
#define pw_log(level, fmt, ...) ((void)0)
#endif

#endif /* _PICOWEATHER_LOG_H */
