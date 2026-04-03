#ifndef _ALARM_MESSAGE_H
#define _ALARM_MESSAGE_H

#include <Arduino.h>

#include "AlarmMessage/AlarmMessageId.h"
#include "AlarmMessage/AlarmTypeDesignator.h"
#include "AlarmMessage/AlarmContent.h"
#include "AlarmMessage/PossibleParameter.h"

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

        using PossibleMessageId = PossibleParameter<AlarmMessageId>;
        using PossibleTypeDesignator = PossibleParameter<AlarmTypeDesignator>;

        Level level;
        AlarmContent content;
        PossibleMessageId messageId;
        PossibleTypeDesignator typeDesignator;

    public:
        explicit AlarmMessage(const AlarmMessage::Level alarmLevel,
                              const AlarmContent alarmContent,
                              const PossibleMessageId messageId,
                              const PossibleTypeDesignator typeDesignator) noexcept
            : level(alarmLevel),
              content(std::move(alarmContent)),
              messageId(std::move(messageId)),
              typeDesignator(std::move(typeDesignator))
        {
        }
        AlarmMessage(const AlarmMessage &&other) noexcept
            : level(other.level),
              content(std::move(other.content)),
              messageId(std::move(other.messageId)),
              typeDesignator(std::move(other.typeDesignator))
        {
        }
        AlarmMessage &operator=(const AlarmMessage &&other) noexcept
        {
            if (this != &other)
            {
                this->level = other.level;
                this->content = std::move(other.content);
                this->messageId = std::move(other.messageId);
                this->typeDesignator = std::move(other.typeDesignator);
            }
            return *this;
        }

        ~AlarmMessage() {}

        AlarmMessage() = delete;
        AlarmMessage(AlarmMessage &other) = delete;
        AlarmMessage(const AlarmMessage &other) = delete;
    };

} // namespace alarm

#endif
