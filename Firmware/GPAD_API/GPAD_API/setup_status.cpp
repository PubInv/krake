#include "setup_status.h"
#include <stdio.h>
#include <string.h>

namespace
{
uint32_t errors = SETUP_ERROR_NONE;

void appendError(char *dest, size_t destLen, const char *text)
{
  if (destLen == 0 || text == nullptr)
  {
    return;
  }
  const size_t used = strlen(dest);
  if (used >= destLen - 1)
  {
    return;
  }
  snprintf(dest + used, destLen - used, "%s%s", used == 0 ? "" : ", ", text);
}
}

void setSetupError(SetupErrorFlag error)
{
  errors |= static_cast<uint32_t>(error);
}

uint32_t setupErrorFlags()
{
  return errors;
}

void formatSetupErrors(char *dest, size_t destLen)
{
  if (destLen == 0)
  {
    return;
  }
  dest[0] = '\0';
  if (errors == SETUP_ERROR_NONE)
  {
    snprintf(dest, destLen, "none");
    return;
  }
  if (errors & SETUP_ERROR_LITTLEFS) appendError(dest, destLen, "LittleFS mount");
  if (errors & SETUP_ERROR_DFPLAYER) appendError(dest, destLen, "DFPlayer unavailable");
  if (errors & SETUP_ERROR_DFPLAYER_FILES) appendError(dest, destLen, "DFPlayer audio files");
  if (errors & SETUP_ERROR_ROTARY_ENCODER) appendError(dest, destLen, "rotary encoder allocation");
  if (errors & SETUP_ERROR_COM_PREFERENCES) appendError(dest, destLen, "COM preferences");
  if (errors & SETUP_ERROR_OPERATOR_PREFERENCES) appendError(dest, destLen, "operator preferences");
  if (errors & SETUP_ERROR_MQTT_PREFERENCES) appendError(dest, destLen, "MQTT preferences");
}
