#include "AlarmContent.h"

#include <iterator>

#ifndef PIO_UNIT_TESTING
#include <Print.h>
#else
#include <MockPrint.h>
using Print = MockPrint;
#endif

using namespace gpap_message::alarm;

AlarmContent::AlarmContent(const std::size_t messageLength, const Buffer message)
    : messageLength(messageLength), message(std::move(message))
{
    if (this->messageLength > AlarmContent::MAX_LENGTH)
    {
        throw;
    }
}

AlarmContent::~AlarmContent() {}

std::size_t AlarmContent::printTo(Print &print) const
{
    auto beginIterator = this->message.cbegin();
    const auto endIterator = std::next(beginIterator, this->messageLength);

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
