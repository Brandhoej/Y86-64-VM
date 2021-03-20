#ifndef cero_logger_h
#define cero_logger_h

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"

#define ANSI_COLOR_BLACK          "\x1b[30m"
#define ANSI_COLOR_RED            "\x1b[31m"
#define ANSI_COLOR_GREEN          "\x1b[32m"
#define ANSI_COLOR_YELLOW         "\x1b[33m"
#define ANSI_COLOR_BLUE           "\x1b[34m"
#define ANSI_COLOR_MAGENTA        "\x1b[35m"
#define ANSI_COLOR_CYAN           "\x1b[36m"
#define ANSI_COLOR_WHITE          "\x1b[37m"
#define ANSI_COLOR_BRIGHT_BLACK   "\x1b[30;1m"
#define ANSI_COLOR_BRIGHT_RED     "\x1b[31;1m"
#define ANSI_COLOR_BRIGHT_GREEN   "\x1b[32;1m"
#define ANSI_COLOR_BRIGHT_YELLOW  "\x1b[33;1m"
#define ANSI_COLOR_BRIGHT_BLUE    "\x1b[34;1m"
#define ANSI_COLOR_BRIGHT_MAGENTA "\x1b[35;1m"
#define ANSI_COLOR_BRIGHT_CYAN    "\x1b[36;1m"
#define ANSI_COLOR_BRIGHT_WHITE   "\x1b[37;1m"
#define ANSI_COLOR_RESET          "\x1b[0m"

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,
#define FOREACH_LOGGER_LEVEL(wrapper)\
        wrapper(LOG_TRACE)\
        wrapper(LOG_DEBUG)\
        wrapper(LOG_INFO)\
        wrapper(LOG_WARN)\
        wrapper(LOG_ERROR)\
        wrapper(LOG_FATAL)\

typedef enum {
    FOREACH_LOGGER_LEVEL(GENERATE_ENUM)
} LoggerLevel;

static const char* loggerLevelStrings[6] = {
    FOREACH_LOGGER_LEVEL(GENERATE_STRING)
};

static const char* loggerLevelAnsiColorStrings[6] = {
    ANSI_COLOR_CYAN,       /* LOG_TRACE */
    ANSI_COLOR_MAGENTA,    /* LOG_DEBUG */
    ANSI_COLOR_BLUE,       /* LOG_INFO  */
    ANSI_COLOR_YELLOW,     /* LOG_WARN  */
    ANSI_COLOR_RED,        /* LOG_ERROR */
    ANSI_COLOR_BRIGHT_RED, /* LOG_FATAL */
};
#undef GENERATE_ENUM
#undef GENERATE_STRING

#define LOGGER_LOG_TIME
#define LOGGER_LOG_FILE

#define CERO_PRINT(...) (printf(__VA_ARGS__))
#define CERO_LEVEL_LOG(level, fileName, lineNumber)                           \
    do {                                                                      \
        time_t t = time(NULL);                                                \
        struct tm* tm = localtime(&t);                                        \
        CERO_PRINT(ANSI_COLOR_BRIGHT_BLACK"%02i:%02i:%02i"ANSI_COLOR_RESET    \
                   " "                                                        \
                   "%s%-5s"ANSI_COLOR_RESET                                   \
                   " "                                                        \
                   ANSI_COLOR_BRIGHT_BLACK"%10s:%03u:"ANSI_COLOR_RESET,       \
            tm->tm_hour, tm->tm_min, tm->tm_sec,                              \
            loggerLevelAnsiColorStrings[level], loggerLevelStrings[level] + 4,\
            fileName, lineNumber);                                            \
    } while (false)

#define CERO_LOG(level, fileName, lineNumber, ...)  \
    do {                                            \
        CERO_LEVEL_LOG(level, fileName, lineNumber);\
        printf(" "__VA_ARGS__);                     \
    } while (false)

#define CERO_TRACE(...) CERO_LOG(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define CERO_DEBUG(...) CERO_LOG(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define CERO_INFO(...)  CERO_LOG(LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define CERO_WARN(...)  CERO_LOG(LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define CERO_ERROR(...) CERO_LOG(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define CERO_FATAL(...) CERO_LOG(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

#endif