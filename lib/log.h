#ifndef LOG_H
#define LOG_H

#include <stdio.h>

// LOG_LEVEL meaning:
// 0 = NONE   (no logs at all)
// 1 = ERROR  (only errors)
// 2 = WARN   (warnings + errors)
// 3 = INFO   (info + warnings + errors)
// 4 = DEBUG  (debug + info + warnings + errors)
#ifndef LOG_LEVEL
#define LOG_LEVEL 3   // default: INFO
#endif

// Optional: prefix for your library/module, e.g. "ARRAY" or "HASHMAP".
// You can -DLOG_NAME="ARRAY" from compiler if you want.
#ifndef LOG_NAME
#define LOG_NAME "LOG"
#endif

// Base guarded printf: all levels go through this.
// We always compile this, the guarding happens in LOG_* macros.
#define PRINTF_GUARDED(level_str, fmt, ...)                                       \
    fprintf(                                                                      \
        stderr,                                                                   \
         level_str ":" LOG_NAME ":%s:%s:%d: " fmt "\n",                           \
        __func__, __FILE__, __LINE__, ##__VA_ARGS__)

// ----- Per-level macros, guarded by LOG_LEVEL -----

// DEBUG (4)
#if LOG_LEVEL >= 4
    #define LOG_DEBUG(fmt, ...)  PRINTF_GUARDED("DEBUG", fmt, ##__VA_ARGS__)
#else
    #define LOG_DEBUG(fmt, ...)  do { } while (0)
#endif

// INFO (3)
#if LOG_LEVEL >= 3
    #define LOG_INFO(fmt, ...)   PRINTF_GUARDED("INFO", fmt, ##__VA_ARGS__)
#else
    #define LOG_INFO(fmt, ...)   do { } while (0)
#endif

// WARN (2)
#if LOG_LEVEL >= 2
    #define LOG_WARN(fmt, ...)   PRINTF_GUARDED("WARN", fmt, ##__VA_ARGS__)
#else
    #define LOG_WARN(fmt, ...)   do { } while (0)
#endif

// ERROR (1)
#if LOG_LEVEL >= 1
    #define LOG_ERROR(fmt, ...)  PRINTF_GUARDED("ERROR", fmt, ##__VA_ARGS__)
#else
    #define LOG_ERROR(fmt, ...)  do { } while (0)
#endif

#endif // LOG_H
