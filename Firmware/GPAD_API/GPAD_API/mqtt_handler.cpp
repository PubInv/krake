#include "mqtt_handler.h"

extern char publish_Ack_Topic[];

bool publishAck(PubSubClient *client, const char *message)
{
  if (client == nullptr || message == nullptr)
  {
    return false;
  }

  return client->publish(publish_Ack_Topic, message);
}
