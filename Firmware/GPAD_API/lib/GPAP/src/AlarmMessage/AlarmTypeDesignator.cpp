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

#include "AlarmTypeDesignator.h"

#include <algorithm>

using namespace gpap_message::alarm;

AlarmTypeDesignator::AlarmTypeDesignator(const Buffer designator) : designator(std::move(designator))
{
    auto designatorIterator = designator.begin();

    const bool allDigits =
        std::all_of(this->designator.cbegin(), this->designator.cend(),
                    [](char inputCharacter)
                    {
                        return isdigit(inputCharacter);
                    });

    // if the characters are not all digits we want to throw
    if (!allDigits)
    {
        throw;
    }
}

const AlarmTypeDesignator::Buffer &AlarmTypeDesignator::getValue() const
{
    return this->designator;
}
