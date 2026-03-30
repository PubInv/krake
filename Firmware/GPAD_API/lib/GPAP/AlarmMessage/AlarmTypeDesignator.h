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
        static const size_t TOTAL_DESIGNATOR_LENGTH =
            AlarmTypeDesignator::DESIGNATOR_LENGTH + 1;
        const std::array<char, AlarmTypeDesignator::DESIGNATOR_LENGTH + 1> designator;
        const bool empty;

    public:
        AlarmTypeDesignator(const Buffer designator, const bool empty);

        AlarmTypeDesignator(AlarmTypeDesignator &&other) = default;
        AlarmTypeDesignator(const AlarmTypeDesignator &&other)
            : designator(std::move(other.designator)), empty(other.empty) {}
        AlarmTypeDesignator operator=(AlarmTypeDesignator &&source)
        {
            return std::move(source);
        }
        AlarmTypeDesignator operator=(const AlarmTypeDesignator &&source)
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

    private:
        static std::array<char, AlarmTypeDesignator::TOTAL_DESIGNATOR_LENGTH>
        validateDesignator(const Buffer buffer, const bool empty);
    };
}

#endif
