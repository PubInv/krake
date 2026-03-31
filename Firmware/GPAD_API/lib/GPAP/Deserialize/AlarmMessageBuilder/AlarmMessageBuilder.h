#ifndef _ALARM_MESSAGE_BUILDER_H
#define _ALARM_MESSAGE_BUILDER_H

#include "AlarmMessage.h"

namespace gpap_message::deserialize
{

    class AlarmMessageBuilder final
    {

    private:
        static const char ID_START_CHARACTER = '{';
        static const char ID_END_CHARACTER = '}';

        static const char DESIGNATOR_START_CHARACTER = '[';
        static const char DESIGNATOR_END_CHARACTER = ']';

    private:
        alarm::AlarmMessage::Level level;

        size_t idLength;
        alarm::AlarmMessageId::Buffer idBuffer;

        size_t designatorLength;
        alarm::AlarmTypeDesignator::Buffer designatorBuffer;

        size_t messageLength;
        AlarmContent::Buffer messageBuffer;

    private:
        AlarmMessageBuilder();

        size_t deserializeLevel(const char *const buffer, size_t numBytes);
        size_t deserializeId(const char *const buffer, const size_t numBytes);
        size_t deserializeTypeDesignator(const char *const buffer, const size_t numBytes);
        size_t deserializeMessage(const char *const buffer, const size_t numBytes);

    public:
        static alarm::AlarmMessage buildAlarmMessage(const char *const buffer, const size_t numBytes);

        AlarmMessageBuilder(AlarmMessageBuilder &&other) = default;
        AlarmMessageBuilder operator=(AlarmMessageBuilder &&other)
        {
            return std::move(other);
        }

        AlarmMessageBuilder(AlarmMessageBuilder &other) = delete;
        AlarmMessageBuilder operator=(AlarmMessageBuilder &other) = delete;
    };
}

#endif
