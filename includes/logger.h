#ifndef __LOGGER__
#define __LOGGER__

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

typedef enum {
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    NB_LEVEL
} log_level;

#define ERROR_LEVEL LOG_ERROR

#define log_message(level, format, ...) _log_message((level), __FILE__, __LINE__, (format), ## __VA_ARGS__)

void _log_message(log_level level, const char* file, int line, const char* format, ...);

#endif // __LOGGER__