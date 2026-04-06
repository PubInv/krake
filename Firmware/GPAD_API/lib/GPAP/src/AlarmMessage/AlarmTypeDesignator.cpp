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
#include <cctype>

#ifndef PIO_UNIT_TESTING
#include <Print.h>
#else
#include <MockPrint.h>
using Print = MockPrint;
#endif

using namespace gpap_message::alarm;

AlarmTypeDesignator::AlarmTypeDesignator(const Buffer designator) : designator(std::move(designator))
{
    auto designatorIterator = designator.begin();

    const bool allDigits =
        std::all_of(this->designator.cbegin(), this->designator.cend(),
                    [](char inputCharacter)
                    {
                        return std::isdigit(static_cast<unsigned char>(inputCharacter));
                    });

    // if the characters are not all digits we want to throw
    if (!allDigits)
    {
        throw;
    }
}

AlarmTypeDesignator::~AlarmTypeDesignator() {}

const AlarmTypeDesignator::Buffer &AlarmTypeDesignator::getValue() const
{
    return this->designator;
}

std::size_t AlarmTypeDesignator::printTo(Print &print) const
{
    auto beginIterator = this->designator.cbegin();
    auto const endIterator = this->designator.cend();

    auto bytesWritten = 0;

    std::for_each(
        beginIterator,
        endIterator,
        [&bytesWritten, &print](const char &currElement)
        {
            bytesWritten += print.print(currElement);
        });

    bytesWritten += print.print('\0');
    return bytesWritten;
}
