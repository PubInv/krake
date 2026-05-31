#include "operator_settings.h"
#include <Preferences.h>

namespace
{
  const char *OPERATOR_PREF_NS = "operator";
  const char *OPERATOR_PREF_VOLUME = "volume";
  const char *OPERATOR_PREF_MUTE_MIN = "muteMinutes";

  int clampSetting(int value, int minimum, int maximum)
  {
    if (value < minimum) return minimum;
    if (value > maximum) return maximum;
    return value;
  }

  bool saveUChar(const char *key, int value)
  {
    Preferences prefs;
    if (!prefs.begin(OPERATOR_PREF_NS, false)) return false;
    const size_t written = prefs.putUChar(key, static_cast<uint8_t>(value));
    prefs.end();
    return written == sizeof(uint8_t);
  }
}

int loadVolumeSetting(int fallbackPercent)
{
  Preferences prefs;
  if (!prefs.begin(OPERATOR_PREF_NS, true)) return clampSetting(fallbackPercent, OPERATOR_VOLUME_MIN_PERCENT, OPERATOR_VOLUME_MAX_PERCENT);
  const int safeFallback = clampSetting(fallbackPercent, OPERATOR_VOLUME_MIN_PERCENT, OPERATOR_VOLUME_MAX_PERCENT);
  const int volume = prefs.getUChar(OPERATOR_PREF_VOLUME, static_cast<uint8_t>(safeFallback));
  prefs.end();
  return clampSetting(volume, OPERATOR_VOLUME_MIN_PERCENT, OPERATOR_VOLUME_MAX_PERCENT);
}

int loadMuteTimeoutMinutesSetting(int fallbackMinutes)
{
  Preferences prefs;
  if (!prefs.begin(OPERATOR_PREF_NS, true)) return clampSetting(fallbackMinutes, OPERATOR_MUTE_TIMEOUT_MIN_MINUTES, OPERATOR_MUTE_TIMEOUT_MAX_MINUTES);
  const int safeFallback = clampSetting(fallbackMinutes, OPERATOR_MUTE_TIMEOUT_MIN_MINUTES, OPERATOR_MUTE_TIMEOUT_MAX_MINUTES);
  const int minutes = prefs.getUChar(OPERATOR_PREF_MUTE_MIN, static_cast<uint8_t>(safeFallback));
  prefs.end();
  return clampSetting(minutes, OPERATOR_MUTE_TIMEOUT_MIN_MINUTES, OPERATOR_MUTE_TIMEOUT_MAX_MINUTES);
}

bool saveVolumeSetting(int volumePercent)
{
  return saveUChar(OPERATOR_PREF_VOLUME, clampSetting(volumePercent, OPERATOR_VOLUME_MIN_PERCENT, OPERATOR_VOLUME_MAX_PERCENT));
}

bool saveMuteTimeoutMinutesSetting(int minutes)
{
  return saveUChar(OPERATOR_PREF_MUTE_MIN, clampSetting(minutes, OPERATOR_MUTE_TIMEOUT_MIN_MINUTES, OPERATOR_MUTE_TIMEOUT_MAX_MINUTES));
}
