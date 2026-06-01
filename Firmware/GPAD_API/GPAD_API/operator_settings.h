#ifndef OPERATOR_SETTINGS_H
#define OPERATOR_SETTINGS_H

const int OPERATOR_VOLUME_MIN_PERCENT = 1;
const int OPERATOR_VOLUME_MAX_PERCENT = 100;
const int OPERATOR_MUTE_TIMEOUT_MIN_MINUTES = 0;
const int OPERATOR_MUTE_TIMEOUT_MAX_MINUTES = 60;
const int OPERATOR_ALARM_REPEAT_MIN_SECONDS = 1;
const int OPERATOR_ALARM_REPEAT_MAX_SECONDS = 300;
const int OPERATOR_ALARM_REPEAT_DEFAULT_SECONDS = 10;

int loadVolumeSetting(int fallbackPercent);
int loadMuteTimeoutMinutesSetting(int fallbackMinutes);
int loadAlarmRepeatSecondsSetting(int fallbackSeconds);
bool saveVolumeSetting(int volumePercent);
bool saveMuteTimeoutMinutesSetting(int minutes);
bool saveAlarmRepeatSecondsSetting(int seconds);

#endif
