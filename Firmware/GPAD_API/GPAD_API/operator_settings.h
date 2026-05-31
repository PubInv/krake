#ifndef OPERATOR_SETTINGS_H
#define OPERATOR_SETTINGS_H

const int OPERATOR_VOLUME_MIN_PERCENT = 1;
const int OPERATOR_VOLUME_MAX_PERCENT = 100;
const int OPERATOR_MUTE_TIMEOUT_MIN_MINUTES = 1;
const int OPERATOR_MUTE_TIMEOUT_MAX_MINUTES = 60;

int loadVolumeSetting(int fallbackPercent);
int loadMuteTimeoutMinutesSetting(int fallbackMinutes);
bool saveVolumeSetting(int volumePercent);
bool saveMuteTimeoutMinutesSetting(int minutes);

#endif
