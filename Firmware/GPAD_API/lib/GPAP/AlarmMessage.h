#ifndef _ALARM_MESSAGE_H
#define _ALARM_MESSAGE_H

#include "AlarmMessage/AlarmMessageId.h"
#include "AlarmMessage/AlarmTypeDesignator.h"
#include "AlarmMessage/AlarmContent.h"

namespace gpap_message::alarm
{
    class AlarmMessage final
    {
    public:
        enum class Level : char
        {
            Level0 = '0',
            Level1 = '1',
            Level2 = '2',
            Level3 = '3',
            Level4 = '4',
            Level5 = '5',
        };

        const Level level;
        const AlarmMessageId messageId;
        const AlarmTypeDesignator typeDesignator;
        const AlarmContent content;

    public:
        explicit AlarmMessage(const AlarmMessage::Level alarmLevel,
                              const AlarmContent alarmContent,
                              const AlarmMessageId messageId,
                              const AlarmTypeDesignator typeDesignator)
            : level(alarmLevel), content(std::move(alarmContent)),
              messageId(std::move(messageId)),
              typeDesignator(std::move(typeDesignator)) {}

        AlarmMessage(AlarmMessage &&other) = default;
        AlarmMessage(const AlarmMessage &&other)
            : level(other.level), content(std::move(other.content)),
              messageId(std::move(other.messageId)),
              typeDesignator(std::move(other.typeDesignator)) {}
        AlarmMessage operator=(AlarmMessage &&other) noexcept
        {
            return std::move(other);
        }
        AlarmMessage operator=(const AlarmMessage &&other)
        {
            return std::move(other);
        }

        ~AlarmMessage() {}

        AlarmMessage() = delete;
        AlarmMessage(AlarmMessage &other) = delete;
        AlarmMessage(const AlarmMessage &other) = delete;
    };

} // namespace alarm

#endif
