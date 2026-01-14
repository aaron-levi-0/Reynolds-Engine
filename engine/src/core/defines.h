#ifndef DEFINES_H
#define DEFINES_H

#define ASSERT(_e, ...) 	if(!(_e)) { fprintf(stderr, __VA_ARGS__); exit(EXIT_FAILURE); } 
#define VALIDATE(_e, ...) 	if(!(_e)) {__VA_OPT__( fprintf(stderr, __VA_ARGS__);) return;} 

#define NOT_USED		0
#define BIT(x) 			(1 << x)
#define MAX(x,y)		(x > y)? x:y
#define MIN(x,y)		(x < y)? x:y

#define CREATE_WIN 		create_window("Reynolds Engine", 970, 970)

// Platform detection
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) 
#define PLATFORM_WINDOWS 1
#ifndef _WIN64
#error "64-bit is required on Windows!"
#endif
#elif defined(__linux__) || defined(__gnu_linux__)
// Linux OS
#define PLATFORM_LINUX 1
#if defined(__ANDROID__)
#define PLATFORM_ANDROID 1
#endif
#elif defined(__unix__)
// Catch anything not caught by the above.
#define PLATFORM_UNIX 1
#elif defined(_POSIX_VERSION)
// Posix
#define PLATFORM_POSIX 1
#elif __APPLE__
// Apple platforms
#define PLATFORM_APPLE 1
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR
// iOS Simulator
#define PLATFORM_IOS 1
#define PLATFORM_IOS_SIMULATOR 1
#elif TARGET_OS_IPHONE
#define PLATFORM_IOS 1
// iOS device
#elif TARGET_OS_MAC
// Other kinds of Mac OS
#else
#error "Unknown Apple platform"
#endif
#else
#error "Unknown platform!"
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
  #ifdef REN_EXPORT
    #define REN_API __declspec(dllexport)
  #else
    #define REN_API __declspec(dllimport)
  #endif
#else
  #ifdef REN_EXPORT
    #define REN_API __attribute__((visibility("default")))
  #else
    #define REN_API
  #endif
#endif

#endif // DEFINES_H