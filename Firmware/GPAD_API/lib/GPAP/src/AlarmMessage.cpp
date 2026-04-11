#include "AlarmMessage.h"

using namespace gpap_message::alarm;

const AlarmContent &AlarmMessage::getAlarmContent() const noexcept
{
    return this->content;
}

AlarmMessage::Level AlarmMessage::getAlarmLevel() const noexcept
{
    return this->level;
}

const AlarmMessage::PossibleTypeDesignator &AlarmMessage::getTypeDesignator() const noexcept
{
    return this->typeDesignator;
}

const AlarmMessage::PossibleMessageId &AlarmMessage::getMessageId() const noexcept
{
    return this->messageId;
}
