#ifndef _POSSIBLE_PARAMETER_H
#define _POSSIBLE_PARAMETER_H

#include <Arduino.h>

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

        const union
        {
            T contents;
            std::nullptr_t empty;
        };

        const State state;

        explicit PossibleParameter() : state(State::None), empty(nullptr) {}
        explicit PossibleParameter(T contents) : state(State::Some), contents(std::move(contents)) {}
        PossibleParameter(PossibleParameter<T> &&other) : state(other.state)
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
        PossibleParameter &operator=(PossibleParameter<T> &&other)
        {
            return *this;
        }
        PossibleParameter(const PossibleParameter<T> &&other) : state(other.state)
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
        PossibleParameter<T> &operator=(const PossibleParameter<T> &&other)
        {
            return *this;
        }

        PossibleParameter(PossibleParameter<T> &other) = delete;
        PossibleParameter(const PossibleParameter<T> &other) = delete;

        ~PossibleParameter() {}
    };
}

#endif