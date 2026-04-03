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
        alarm::AlarmContent::Buffer messageBuffer;

    private:
        explicit AlarmMessageBuilder();

        size_t deserializeLevel(const char *const buffer, size_t numBytes);
        size_t deserializeId(const char *const buffer, const size_t numBytes);
        size_t deserializeTypeDesignator(const char *const buffer, const size_t numBytes);
        size_t deserializeMessage(const char *const buffer, const size_t numBytes);

    public:
        static alarm::AlarmMessage buildAlarmMessage(const char *const buffer, const size_t numBytes);
        static bool isReservedCharacter(const char character);

        AlarmMessageBuilder(const AlarmMessageBuilder &&other) noexcept
            : level(other.level),
              idLength(other.idLength),
              idBuffer(std::move(other.idBuffer)),
              designatorLength(other.designatorLength),
              designatorBuffer(std::move(other.designatorBuffer)),
              messageLength(other.messageLength),
              messageBuffer(std::move(other.messageBuffer)) {};
        AlarmMessageBuilder &operator=(const AlarmMessageBuilder &&other) noexcept
        {
            if (this != &other)
            {
                this->level = other.level;
                this->idLength = other.idLength;
                this->idBuffer = std::move(other.idBuffer);
                this->designatorLength = other.designatorLength;
                this->designatorBuffer = std::move(other.designatorBuffer);
                this->messageLength = other.messageLength;
                this->messageBuffer = std::move(other.messageBuffer);
            }
            return *this;
        }

        AlarmMessageBuilder(AlarmMessageBuilder &other) = delete;
        AlarmMessageBuilder operator=(AlarmMessageBuilder &other) = delete;
    };
}

#endif
