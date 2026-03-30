#include "AlarmTypeDesignator.h"

#include <algorithm>

#include <Print.h>

using namespace gpap_message::alarm;

AlarmTypeDesignator::AlarmTypeDesignator(
    const std::array<char, AlarmTypeDesignator::DESIGNATOR_LENGTH> designator,
    const bool empty)
    : designator(AlarmTypeDesignator::validateDesignator(designator, empty)),
      empty(empty) {}

AlarmTypeDesignator::~AlarmTypeDesignator() = default;

const char *const AlarmTypeDesignator::getValue() const
{
    return this->designator.data();
}

size_t AlarmTypeDesignator::printTo(Print &print) const
{
    return print.print(this->getValue());
}

std::array<char, AlarmTypeDesignator::TOTAL_DESIGNATOR_LENGTH>
AlarmTypeDesignator::validateDesignator(
    const std::array<char, AlarmTypeDesignator::DESIGNATOR_LENGTH>
        inputDesignator,
    const bool empty)
{
    std::array<char, AlarmTypeDesignator::TOTAL_DESIGNATOR_LENGTH> designator =
        {};
    if (empty)
    {
        return designator;
    }

    auto designatorIterator = designator.begin();

    bool allDigits = std::all_of(inputDesignator.cbegin(), inputDesignator.cend(),
                                 [&](char inputCharacter)
                                 {
                                     *designatorIterator = inputCharacter;
                                     designatorIterator =
                                         std::next(designatorIterator, 1);
                                     return isdigit(inputCharacter);
                                 });

    // if the characters are not all digits we want to throw
    if (!allDigits)
    {
        throw;
    }

    *designatorIterator = '\0';

    return designator;
}
