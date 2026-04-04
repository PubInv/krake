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

#ifndef _ALARM_CONTENT_H
#define _ALARM_CONTENT_H

#include <array>

namespace gpap_message::alarm
{

    class AlarmContent final
    {
    public:
        static const std::size_t MAX_LENGTH = 80;

        using Buffer = std::array<char, AlarmContent::MAX_LENGTH>;

    private:
        std::size_t messageLength;
        std::array<char, AlarmContent::MAX_LENGTH> message;

    public:
        explicit AlarmContent(const std::size_t messageLength, const Buffer message) noexcept
            : messageLength(messageLength), message(std::move(message)) {};

        AlarmContent(const AlarmContent &&other) noexcept
            : messageLength(other.messageLength), message(std::move(other.message)) {}
        AlarmContent &operator=(const AlarmContent &&other) noexcept
        {
            if (this != &other)
            {
                this->messageLength = other.messageLength;
                this->message = std::move(other.message);
            }
            return *this;
        }

        AlarmContent() = delete;
        AlarmContent(AlarmContent &other) = delete;
        AlarmContent(const AlarmContent &other) = delete;
    };
}

#endif
