#include "AlarmContent.h"

using namespace gpap_message::alarm;

AlarmContent::AlarmContent(const std::size_t messageLength, const Buffer message)
    : messageLength(messageLength), message(std::move(message))
{
    if (this->messageLength > AlarmContent::MAX_LENGTH)
    {
        throw;
    }
}
