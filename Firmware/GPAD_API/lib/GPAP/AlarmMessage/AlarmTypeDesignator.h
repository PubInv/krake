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

#ifndef _ALARM_TYPE_DESIGNATOR_H
#define _ALARM_TYPE_DESIGNATOR_H

#include <array>

#include <Printable.h>

namespace gpap_message::alarm
{
    class AlarmTypeDesignator final : Printable
    {
    public:
        static const size_t DESIGNATOR_LENGTH = 3;
        using Buffer = std::array<char, AlarmTypeDesignator::DESIGNATOR_LENGTH>;

    private:
        std::array<char, AlarmTypeDesignator::DESIGNATOR_LENGTH> designator;

    public:
        explicit AlarmTypeDesignator(const Buffer designator);

        AlarmTypeDesignator(const AlarmTypeDesignator &&other) noexcept
            : designator(std::move(other.designator)) {}
        AlarmTypeDesignator &operator=(const AlarmTypeDesignator &&other) noexcept
        {
            if (this != &other)
            {
                this->designator = std::move(other.designator);
            }
            return *this;
        }

        AlarmTypeDesignator() = delete;
        AlarmTypeDesignator(AlarmTypeDesignator &other) = delete;
        AlarmTypeDesignator(const AlarmTypeDesignator &other) = delete;

        virtual ~AlarmTypeDesignator();
        // Methods
    public:
        const Buffer &getValue() const;

        size_t printTo(Print &print) const override;
    };
}

#endif
