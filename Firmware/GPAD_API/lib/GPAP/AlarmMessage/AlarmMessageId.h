#ifndef _ALARM_MESSAGE_ID_H
#define _ALARM_MESSAGE_ID_H

#include <array>

namespace gpap_message::alarm
{
    class AlarmMessageId final
    {
    public:
        // TODO: Find out what this value is SUPPOSED to be.
        static const size_t MAX_LENGTH = 10;

        using Buffer = std::array<char, AlarmMessageId::MAX_LENGTH>;

    private:
        static const size_t TOTAL_MAX_LENGTH = AlarmMessageId::MAX_LENGTH + 1;

    public:
        size_t idLength;
        std::array<char, AlarmMessageId::TOTAL_MAX_LENGTH> id;

    public:
        explicit AlarmMessageId(const size_t idLength, const Buffer id);

        AlarmMessageId(const AlarmMessageId &&other) noexcept
            : id(std::move(other.id)), idLength(other.idLength) {}
        AlarmMessageId &operator=(const AlarmMessageId &&other) noexcept
        {
            if (this != &other)
            {
                this->idLength = other.idLength;
                this->id = std::move(other.id);
            }
            return *this;
        }

        AlarmMessageId() = delete;
        AlarmMessageId(AlarmMessageId &other) = delete;
        AlarmMessageId(const AlarmMessageId &other) = delete;

    private:
        static std::array<char, AlarmMessageId::TOTAL_MAX_LENGTH>
        validateId(const size_t idLength, const Buffer);
    };
}

#endif
