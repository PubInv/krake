#ifndef _ALARM_TYPE_DESIGNATOR_H
#define _ALARM_TYPE_DESIGNATOR_H

#include <array>

#include <Printable.h>

namespace gpap_message::alarm
{
    class AlarmTypeDesignator final : Printable
    {
    public:
        static const size_t DESIGNATOR_LENGTH = 3;
        using Buffer = std::array<char, AlarmTypeDesignator::DESIGNATOR_LENGTH>;

    private:
        const std::array<char, AlarmTypeDesignator::DESIGNATOR_LENGTH> designator;
        /// @brief The `empty` field was a temporary solution to the situation where no Type Designator
        /// was provided. With the addition of the `PossibleField` type then this field can be removed

    public:
        explicit AlarmTypeDesignator(const Buffer designator);

        AlarmTypeDesignator(const AlarmTypeDesignator &&other) noexcept
            : designator(std::move(other.designator)) {}
        AlarmTypeDesignator operator=(const AlarmTypeDesignator &&source) noexcept
        {
            return std::move(source);
        }

        AlarmTypeDesignator() = delete;
        AlarmTypeDesignator(AlarmTypeDesignator &other) = delete;
        AlarmTypeDesignator(const AlarmTypeDesignator &other) = delete;

        virtual ~AlarmTypeDesignator();
        // Methods
    public:
        const char *const getValue() const;

        size_t printTo(Print &print) const override;
    };
}

#endif
