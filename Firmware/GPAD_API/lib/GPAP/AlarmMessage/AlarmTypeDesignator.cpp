#include "AlarmTypeDesignator.h"

#include <algorithm>

#include <Print.h>

using namespace gpap_message::alarm;

AlarmTypeDesignator::AlarmTypeDesignator(const Buffer designator) : designator(designator)
{
    auto designatorIterator = designator.begin();

    const bool allDigits = std::all_of(this->designator.cbegin(), this->designator.cend(),
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

AlarmTypeDesignator::~AlarmTypeDesignator() = default;

const char *const AlarmTypeDesignator::getValue() const
{
    return this->designator.data();
}

size_t AlarmTypeDesignator::printTo(Print &print) const
{
    auto bytesWritten = print.print(this->getValue());
    bytesWritten += print.print("\0");
    return bytesWritten;
}
