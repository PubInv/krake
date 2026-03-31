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
        const bool empty;

    public:
        AlarmTypeDesignator(const Buffer designator, const bool empty);

        AlarmTypeDesignator(const AlarmTypeDesignator &&other) noexcept
            : designator(std::move(other.designator)), empty(other.empty) {}
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
