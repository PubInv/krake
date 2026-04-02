#ifndef _ALARM_CONTENT_H
#define _ALARM_CONTENT_H

#include <array>

namespace gpap_message::alarm
{

    class AlarmContent final
    {
    public:
        static const size_t MAX_LENGTH = 80;

        using Buffer = std::array<char, AlarmContent::MAX_LENGTH>;

    private:
        const size_t messageLength;
        const std::array<char, AlarmContent::MAX_LENGTH> message;

    public:
        explicit AlarmContent(const size_t messageLength, const Buffer message)
            : messageLength(messageLength), message(std::move(message)) {};

        AlarmContent(AlarmContent &&other)
            : messageLength(other.messageLength), message(std::move(other.message)) {}
        AlarmContent &operator=(AlarmContent &&other)
        {
            return *this;
        }
        AlarmContent(const AlarmContent &&other)
            : messageLength(other.messageLength), message(std::move(other.message)) {}
        AlarmContent &operator=(const AlarmContent &&other)
        {
            return *this;
        }

        AlarmContent() = delete;
        AlarmContent(AlarmContent &other) = delete;
        AlarmContent(const AlarmContent &other) = delete;
    };
}

#endif
