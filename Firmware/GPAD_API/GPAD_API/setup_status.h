#ifndef SETUP_STATUS_H
#define SETUP_STATUS_H

#include <Arduino.h>

enum SetupErrorFlag : uint32_t
{
  SETUP_ERROR_NONE = 0,
  SETUP_ERROR_LITTLEFS = 1UL << 0,
  SETUP_ERROR_DFPLAYER = 1UL << 1,
  SETUP_ERROR_DFPLAYER_FILES = 1UL << 2,
  SETUP_ERROR_ROTARY_ENCODER = 1UL << 3,
  SETUP_ERROR_COM_PREFERENCES = 1UL << 4,
  SETUP_ERROR_OPERATOR_PREFERENCES = 1UL << 5,
  SETUP_ERROR_MQTT_PREFERENCES = 1UL << 6,
};

void setSetupError(SetupErrorFlag error);
uint32_t setupErrorFlags();
void formatSetupErrors(char *dest, size_t destLen);

#endif
