#include "operator_settings.h"
#include <Preferences.h>
#include "setup_status.h"

namespace
{
  const char *OPERATOR_PREF_NS = "operator";
  const char *OPERATOR_PREF_VOLUME = "volume";
  const char *OPERATOR_PREF_MUTE_MIN = "muteMinutes";
  const char *OPERATOR_PREF_REPEAT_SEC = "repeatSec";

  int clampSetting(int value, int minimum, int maximum)
  {
    if (value < minimum) return minimum;
    if (value > maximum) return maximum;
    return value;
  }

  bool saveUChar(const char *key, int value)
  {
    Preferences prefs;
    if (!prefs.begin(OPERATOR_PREF_NS, false)) { setSetupError(SETUP_ERROR_OPERATOR_PREFERENCES); return false; }
    const size_t written = prefs.putUChar(key, static_cast<uint8_t>(value));
    prefs.end();
    return written == sizeof(uint8_t);
  }

  bool saveUShort(const char *key, int value)
  {
    Preferences prefs;
    if (!prefs.begin(OPERATOR_PREF_NS, false)) { setSetupError(SETUP_ERROR_OPERATOR_PREFERENCES); return false; }
    const size_t written = prefs.putUShort(key, static_cast<uint16_t>(value));
    prefs.end();
    return written == sizeof(uint16_t);
  }
}

int loadVolumeSetting(int fallbackPercent)
{
  Preferences prefs;
  if (!prefs.begin(OPERATOR_PREF_NS, true)) { setSetupError(SETUP_ERROR_OPERATOR_PREFERENCES); return clampSetting(fallbackPercent, OPERATOR_VOLUME_MIN_PERCENT, OPERATOR_VOLUME_MAX_PERCENT); }
  const int safeFallback = clampSetting(fallbackPercent, OPERATOR_VOLUME_MIN_PERCENT, OPERATOR_VOLUME_MAX_PERCENT);
  const int volume = prefs.getUChar(OPERATOR_PREF_VOLUME, static_cast<uint8_t>(safeFallback));
  prefs.end();
  return clampSetting(volume, OPERATOR_VOLUME_MIN_PERCENT, OPERATOR_VOLUME_MAX_PERCENT);
}

int loadMuteTimeoutMinutesSetting(int fallbackMinutes)
{
  Preferences prefs;
  if (!prefs.begin(OPERATOR_PREF_NS, true)) { setSetupError(SETUP_ERROR_OPERATOR_PREFERENCES); return clampSetting(fallbackMinutes, OPERATOR_MUTE_TIMEOUT_MIN_MINUTES, OPERATOR_MUTE_TIMEOUT_MAX_MINUTES); }
  const int safeFallback = clampSetting(fallbackMinutes, OPERATOR_MUTE_TIMEOUT_MIN_MINUTES, OPERATOR_MUTE_TIMEOUT_MAX_MINUTES);
  const int minutes = prefs.getUChar(OPERATOR_PREF_MUTE_MIN, static_cast<uint8_t>(safeFallback));
  prefs.end();
  return clampSetting(minutes, OPERATOR_MUTE_TIMEOUT_MIN_MINUTES, OPERATOR_MUTE_TIMEOUT_MAX_MINUTES);
}

int loadAlarmRepeatSecondsSetting(int fallbackSeconds)
{
  Preferences prefs;
  if (!prefs.begin(OPERATOR_PREF_NS, true)) { setSetupError(SETUP_ERROR_OPERATOR_PREFERENCES); return clampSetting(fallbackSeconds, OPERATOR_ALARM_REPEAT_MIN_SECONDS, OPERATOR_ALARM_REPEAT_MAX_SECONDS); }
  const int safeFallback = clampSetting(fallbackSeconds, OPERATOR_ALARM_REPEAT_MIN_SECONDS, OPERATOR_ALARM_REPEAT_MAX_SECONDS);
  const int seconds = prefs.getUShort(OPERATOR_PREF_REPEAT_SEC, static_cast<uint16_t>(safeFallback));
  prefs.end();
  return clampSetting(seconds, OPERATOR_ALARM_REPEAT_MIN_SECONDS, OPERATOR_ALARM_REPEAT_MAX_SECONDS);
}

bool saveVolumeSetting(int volumePercent)
{
  return saveUChar(OPERATOR_PREF_VOLUME, clampSetting(volumePercent, OPERATOR_VOLUME_MIN_PERCENT, OPERATOR_VOLUME_MAX_PERCENT));
}

bool saveMuteTimeoutMinutesSetting(int minutes)
{
  return saveUChar(OPERATOR_PREF_MUTE_MIN, clampSetting(minutes, OPERATOR_MUTE_TIMEOUT_MIN_MINUTES, OPERATOR_MUTE_TIMEOUT_MAX_MINUTES));
}

bool saveAlarmRepeatSecondsSetting(int seconds)
{
  return saveUShort(OPERATOR_PREF_REPEAT_SEC, clampSetting(seconds, OPERATOR_ALARM_REPEAT_MIN_SECONDS, OPERATOR_ALARM_REPEAT_MAX_SECONDS));
}
