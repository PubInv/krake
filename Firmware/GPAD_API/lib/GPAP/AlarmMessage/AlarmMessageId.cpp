#include "AlarmMessageId.h"

#include <algorithm>

using namespace gpap_message::alarm;

AlarmMessageId::AlarmMessageId(
    const size_t idLength,
    const std::array<char, AlarmMessageId::MAX_LENGTH> id)
    : idLength(idLength), id(AlarmMessageId::validateId(idLength, id)) {}

std::array<char, AlarmMessageId::TOTAL_MAX_LENGTH> AlarmMessageId::validateId(
    const size_t idLength,
    const std::array<char, AlarmMessageId::MAX_LENGTH> id)
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

    bool allHex = std::all_of(
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
    return validatedId;
}
