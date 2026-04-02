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
