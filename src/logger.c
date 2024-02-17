#include "logger.h"

char* to_string_level(log_level level) {
    static_assert(NB_LEVEL == 3, "Handle all values of log_level");
    switch (level)
    {
    case LOG_INFO:
        return "INFO";
    case LOG_WARN:
        return "WARN";
    case LOG_ERROR:
        return "ERROR";
    default:
        assert(0 && "Unreachable");
        break;
    }
}

void _log_message(log_level level, const char* file, int line, const char* format, ...) {
    va_list args;
    va_start(args, format);
    FILE* output = stdout;
    if(level >= ERROR_LEVEL) {
        output = stderr;
        fprintf(output, "\033[0;31m");
    }
    fprintf(output, "[%s] %s:%d: ", to_string_level(level), file, line);
    vfprintf(output, format, args);
    fprintf(output, "\n\033[0m");
    va_end(args);
}