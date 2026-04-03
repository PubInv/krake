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

#include "AlarmMessageId.h"

#include <algorithm>

using namespace gpap_message::alarm;

AlarmMessageId::AlarmMessageId(
    const size_t idLength,
    const Buffer id)
    : idLength(idLength), id(std::move(AlarmMessageId::validateId(idLength, id))) {}

std::array<char, AlarmMessageId::TOTAL_MAX_LENGTH> AlarmMessageId::validateId(
    const size_t idLength,
    const Buffer id)
{
    std::array<char, AlarmMessageId::TOTAL_MAX_LENGTH> validatedId = {};
    auto validatedIdIterator = validatedId.begin();

    // The recorded real-length of the message ID cannot me more than the max
    // number of elements
    if (idLength > id.size())
    {
        throw;
    }

    auto startIterator = id.cbegin();
    auto endIterator = id.cbegin() + idLength;

    const bool allHex = std::all_of(
        startIterator, endIterator, [&validatedIdIterator](char hexChar)
        {
        *validatedIdIterator = hexChar;
        validatedIdIterator = std::next(validatedIdIterator, 1);
        return isxdigit(hexChar); });

    // If all the characters are NOT hex characters we need to throw an error and
    // cancel the creation of this instance.
    if (!allHex)
    {
        throw;
    }

    *validatedIdIterator = '\0';
    return std::move(validatedId);
}
