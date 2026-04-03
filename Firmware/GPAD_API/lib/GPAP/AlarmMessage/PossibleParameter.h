#ifndef _POSSIBLE_PARAMETER_H
#define _POSSIBLE_PARAMETER_H

namespace gpap_message::alarm
{
    template <class T>
    struct PossibleParameter
    {
        enum class State
        {
            Some,
            None,
        };

        union
        {
            T contents;
            std::nullptr_t empty;
        };

        State state;

        explicit PossibleParameter() : state(State::None), empty(nullptr) {}
        explicit PossibleParameter(T contents) : state(State::Some), contents(std::move(contents)) {}

        PossibleParameter(const PossibleParameter<T> &&other) noexcept
            : state(other.state)
        {
            switch (this->state)
            {
            case State::Some:
                this->contents = std::move(other.contents);
                break;
            case State::None:
                this->empty = nullptr;
                break;
            }
        }
        PossibleParameter &operator=(const PossibleParameter<T> &&other) noexcept
        {
            if (this != &other)
            {
                this->state = other.state;
                switch (this->state)
                {
                case State::Some:
                    this->contents = std::move(other.contents);
                    break;
                case State::None:
                    this->empty = nullptr;
                    break;
                }
            }
            return *this;
        }

        PossibleParameter(PossibleParameter<T> &other) = delete;
        PossibleParameter(const PossibleParameter<T> &other) = delete;

        ~PossibleParameter() {}
    };
}

#endif