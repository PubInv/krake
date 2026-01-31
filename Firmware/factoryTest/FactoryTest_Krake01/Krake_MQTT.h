#ifndef KRAKE_MQTT_H
#define KRAKE_MQTT_H

#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

void Krake_MQTT_setup();
void Krake_MQTT_loop();

#ifdef __cplusplus
}
#endif

#endif // KRAKE_MQTT_H
