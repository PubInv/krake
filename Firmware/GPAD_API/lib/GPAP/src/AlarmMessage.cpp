#include "AlarmMessage.h"

using namespace gpap_message::alarm;

const AlarmContent &AlarmMessage::getAlarmContent() const noexcept
{
    return this->content;
}