#ifndef DEBUG_MACROS_H
#define DEBUG_MACROS_H

#include <Arduino.h>

// Serial-monitor verbosity: 0 = errors only, 1 = operational messages,
// 2 = verbose diagnostics.  Override with -DDEBUG_LEVEL=<0|1|2>.
#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL 0
#endif
#if DEBUG_LEVEL < 0 || DEBUG_LEVEL > 2
#error "DEBUG_LEVEL must be 0, 1, or 2"
#endif
#ifndef DEBUG
#define DEBUG DEBUG_LEVEL
#endif
#ifndef GPAD_DEBUG
#define GPAD_DEBUG DEBUG_LEVEL
#endif

#ifndef ENABLE_LCD_UI
#define ENABLE_LCD_UI 1
#endif
#ifndef ENABLE_DFPLAYER
#define ENABLE_DFPLAYER 1
#endif
#ifndef ENABLE_COM_SETUP
#define ENABLE_COM_SETUP 1
#endif
#ifndef ENABLE_OTA
#define ENABLE_OTA 1
#endif

#define DBG_AT(level, statement) do { if (DEBUG_LEVEL >= (level)) { statement; } } while (0)
#define DBG_PRINT(x) DBG_AT(1, Serial.print(x))
#define DBG_PRINTLN(x) DBG_AT(1, Serial.println(x))
#define DBG_PRINTF(...) DBG_AT(1, Serial.printf(__VA_ARGS__))
#define DBG_VERBOSE_PRINT(x) DBG_AT(2, Serial.print(x))
#define DBG_VERBOSE_PRINTLN(x) DBG_AT(2, Serial.println(x))
#define DBG_VERBOSE_PRINTF(...) DBG_AT(2, Serial.printf(__VA_ARGS__))

#endif
