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

#ifndef _ALARM_MESSAGE_ID_H
#define _ALARM_MESSAGE_ID_H

#include <array>

#ifndef PIO_UNIT_TESTING
#include <Printable.h>
#else
#include "MockPrintable.h"
#include "MockPrint.h"
using Printable = MockPrintable;
using Print = MockPrint;
#endif

namespace gpap_message::alarm
{
    class AlarmMessageId final : public Printable
    {
    public:
        static const std::size_t MAX_LENGTH = 10;

        using Buffer = std::array<char, AlarmMessageId::MAX_LENGTH>;

    private:
        static const std::size_t TOTAL_MAX_LENGTH = AlarmMessageId::MAX_LENGTH + 1;

        std::size_t idLength;
        std::array<char, AlarmMessageId::TOTAL_MAX_LENGTH> id;

    public:
        explicit AlarmMessageId(const std::size_t idLength, const Buffer id);

        AlarmMessageId(const AlarmMessageId &&other) noexcept
            : id(std::move(other.id)), idLength(other.idLength) {}
        AlarmMessageId &operator=(const AlarmMessageId &&other) noexcept
        {
            if (this != &other)
            {
                this->idLength = other.idLength;
                this->id = std::move(other.id);
            }
            return *this;
        }

        AlarmMessageId() = delete;
        AlarmMessageId(AlarmMessageId &other) = delete;
        AlarmMessageId(const AlarmMessageId &other) = delete;

        virtual ~AlarmMessageId();

    private:
        static std::array<char, AlarmMessageId::TOTAL_MAX_LENGTH>
        validateId(const std::size_t idLength, const Buffer);

    public:
        std::size_t printTo(Print &print) const;
    };
}

#endif
