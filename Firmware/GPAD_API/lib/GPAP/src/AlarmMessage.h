/*
  Copyright (C) 2026 Public Invention

  This program includes free software: you can redistribute it and/or modify
  it under the terms of the GNU Affero General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  See the GNU Affero General Public License for more details.
  You should have received a copy of the GNU Affero General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#ifndef _ALARM_MESSAGE_H
#define _ALARM_MESSAGE_H

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
