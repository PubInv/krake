#include "AlarmTypeDesignator.h"

#include <algorithm>

#include <Print.h>

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

AlarmTypeDesignator::~AlarmTypeDesignator() {}

const AlarmTypeDesignator::Buffer &AlarmTypeDesignator::getValue() const
{
    return this->designator;
}

size_t AlarmTypeDesignator::printTo(Print &print) const
{
    auto bytesWritten = 0;
    for (auto iter = this->getValue().cbegin(); iter != this->getValue().cend(); ++iter)
    {
        bytesWritten += print.print(*iter);
    }

    bytesWritten += print.print("\0");
    return bytesWritten;
}
