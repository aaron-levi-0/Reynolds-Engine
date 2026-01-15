#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include "../include/logging.h"

#define LOG_BUFSIZ 32000

// ANSI escape codes for colors
#define COLOUR_RESET "\033[0m"
#define COLOUR_TIMESTAMP "\033[38;2;145;130;141m" // TrueColor for #91828D

#define COLOUR_FATAL "\033[0;31m"    // Bold Red    #AA0000
#define COLOUR_ERROR "\033[1;31m"    // Red         #FF5555
#define COLOUR_WARN  "\033[0;33m"    // Yellow      #AAAA00
#define COLOUR_INFO  "\033[0;32m"    // Green       #00AA00
#define COLOUR_DEBUG "\033[0;34m"    // Blue        #0000AA
#define COLOUR_VERBOSE "\033[0;37m"  // White/Gray  #AAAAAA

uint8_t LOG_LEVEL = LOG_LEVEL_INFO;

void set_log_level(log_level level)
{
    LOG_LEVEL = level;
}

// Helper function to get the log level string with color
static const char* get_coloured_log_level(log_level level) {
    switch (level) {
        case LOG_LEVEL_FATAL: return COLOUR_FATAL "[FATAL]" COLOUR_RESET;
        case LOG_LEVEL_ERROR: return COLOUR_ERROR "[ERROR]" COLOUR_RESET;
        case LOG_LEVEL_WARN:  return COLOUR_WARN  "[WARNING]" COLOUR_RESET;
        case LOG_LEVEL_INFO:  return COLOUR_INFO  "[INFO]" COLOUR_RESET;
        case LOG_LEVEL_DEBUG: return COLOUR_DEBUG "[DEBUG]" COLOUR_RESET;
        case LOG_LEVEL_VERBOSE: return COLOUR_VERBOSE "[EXTRA]" COLOUR_RESET;
        default: return "";
    }
}

// Helper function: Get a formatted timestamp
void get_timestamp(char* buffer, size_t size) {
    time_t rawtime;
    struct tm* time_info;

    time(&rawtime);                         // Get current time
    time_info = localtime(&rawtime);        // Convert to local time
    strftime(buffer, size, "%H:%M:%S", time_info); // Format timestamp
}

// Helper function: Output the log message to the appropriate stream
static void output_log_message(log_level level, const char* final_message) {
    if (level <= LOG_LEVEL_WARN) 
        fprintf(stderr, "%s", final_message);
    else 
        printf("%s", final_message);
}

void log_message(log_level level, const char* format, ...)
{
    if (level > LOG_LEVEL) return;
    
    const char* colored_log_level = get_coloured_log_level(level);

    // Buffer for the formatted log message
    char out_message[LOG_BUFSIZ - 500];
    memset(out_message, 0, sizeof(out_message));

    // Format the original message
    __builtin_va_list arg_ptr;
    va_start(arg_ptr, format);
    vsnprintf(out_message, sizeof(out_message), format, arg_ptr);
    va_end(arg_ptr);
    
    char time_stamp[64];
    get_timestamp(time_stamp, sizeof(time_stamp));

    // Combine timestamp, log level, and the message; add colour
    char final_message[LOG_BUFSIZ];
    snprintf(final_message, sizeof(final_message),"%s[%s]%s REYNOLDS %s: %s\n", 
                COLOUR_TIMESTAMP, time_stamp, COLOUR_RESET, colored_log_level, out_message);
    
    output_log_message(level, final_message);
}

