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
        std::array<char, AlarmTypeDesignator::DESIGNATOR_LENGTH> designator;

    public:
        explicit AlarmTypeDesignator(const Buffer designator);

        AlarmTypeDesignator(const AlarmTypeDesignator &&other) noexcept
            : designator(std::move(other.designator)) {}
        AlarmTypeDesignator &operator=(const AlarmTypeDesignator &&other) noexcept
        {
            if (this != &other)
            {
                this->designator = std::move(other.designator);
            }
            return *this;
        }

        AlarmTypeDesignator() = delete;
        AlarmTypeDesignator(AlarmTypeDesignator &other) = delete;
        AlarmTypeDesignator(const AlarmTypeDesignator &other) = delete;

        virtual ~AlarmTypeDesignator();
        // Methods
    public:
        const Buffer &getValue() const;

        size_t printTo(Print &print) const override;
    };
}

#endif
