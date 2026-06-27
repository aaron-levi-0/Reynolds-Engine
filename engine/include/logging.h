#ifndef REYNOLDS_LOGGING_H
#define REYNOLDS_LOGGING_H

#include "defines.h"
#include <stdlib.h>
#include <stdint.h>

typedef enum log_level {
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_VERBOSE 
} log_level;

REN_API void set_log_level(log_level level);
REN_API uint8_t get_log_level();
REN_API void log_message(log_level level, const char* format, ...);

#define LOG_VERBOSE_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_WARN_ENABLED 1

//user can declare the filepath to the workspace as SRC_FOLDER during complilation otherwise the absolute
//path is used
#ifdef ENABLE_RELATIVE_PATHS
#define RELATIVE_FILE (__FILE__ + (sizeof(SRC_FOLDER) - 2))
#else
#define RELATIVE_FILE (__FILE__)
#endif

#define ASSERT_FATAL(_e, ...)                                                          \
    if (!(_e)) {                                                                     \
        log_message(LOG_LEVEL_FATAL,                                                 \
                    "in file %s (line %d): " __VA_ARGS__, RELATIVE_FILE, __LINE__);  \
        exit(EXIT_FAILURE);                                                          \
    }

#ifdef RELEASE_VERSION
#define LOG_VERBOSE_ENABLED 0
#define LOG_DEBUG_ENABLED 0
#define LOG_INFO_ENABLED 0
#endif

#if LOG_VERBOSE_ENABLED == 1
#define REYNOLDS_VERBOSE(format, ...) log_message(LOG_LEVEL_VERBOSE, format, ##__VA_ARGS__);
#else
#define REYNOLDS_VERBOSE(format, ...)
#endif

#if LOG_DEBUG_ENABLED == 1
#define REYNOLDS_DEBUG(format, ...) log_message(LOG_LEVEL_DEBUG, format, ##__VA_ARGS__);
#else
#define REYNOLDS_DEBUG(format, ...)
#endif

#if LOG_INFO_ENABLED == 1
#define REYNOLDS_INFO(format, ...) log_message(LOG_LEVEL_INFO, format, ##__VA_ARGS__);
#else
#define REYNOLDS_INFO(format, ...)
#endif

#if LOG_WARN_ENABLED == 1
#define REYNOLDS_WARN(format, ...) log_message(LOG_LEVEL_WARN, format, ##__VA_ARGS__);
#define VALIDATE_LOG(_e, ...)                                                        \
    if (!(_e)) {                                                                     \
        log_message(LOG_LEVEL_ERROR,                                                 \
                    "in file %s (line %d): " __VA_ARGS__, RELATIVE_FILE, __LINE__);  \
        return;                                                         \
    }
#else
#define REYNOLDS_WARN(format, ...)
#define VALIDATE_LOG(_e, ...) 	if(!(_e)) { return;}
#endif

#define REYNOLDS_ERROR(format, ...) log_message(LOG_LEVEL_ERROR, format, ##__VA_ARGS__);

#endif // LOGGING_H