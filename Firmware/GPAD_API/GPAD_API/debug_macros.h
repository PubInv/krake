#ifndef DEBUG_MACROS_H
#define DEBUG_MACROS_H

#include <Arduino.h>

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL 0
#endif

#ifndef ENABLE_LCD_UI
#define ENABLE_LCD_UI 1
#endif

#ifndef ENABLE_DEBUG_LOGS
#define ENABLE_DEBUG_LOGS 0
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

#ifndef GPAD_DEBUG
#define GPAD_DEBUG DEBUG_LEVEL
#endif

#define DBG_PRINT(x) do { if (ENABLE_DEBUG_LOGS || (GPAD_DEBUG > 0)) { Serial.print(x); } } while (0)
#define DBG_PRINTLN(x) do { if (ENABLE_DEBUG_LOGS || (GPAD_DEBUG > 0)) { Serial.println(x); } } while (0)
#define DBG_PRINTF(...) do { if (ENABLE_DEBUG_LOGS || (GPAD_DEBUG > 0)) { Serial.printf(__VA_ARGS__); } } while (0)

#endif
