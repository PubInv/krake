#ifndef _POSSIBLE_PARAMETER_H
#define _POSSIBLE_PARAMETER_H

namespace gpap_message::alarm
{
    template <class T>
    struct PossibleParameter
    {
        const union
        {
            T contents;
            std::nullptr_t empty;
        };

        enum State
        {
            None,
            Some
        };

        State state;

        explicit PossibleParameter() : state(State::None), empty(nullptr) {}

        explicit PossibleParameter(T contents) : contents(std::move(contents)), state(State::Some) {}
        PossibleParameter(const PossibleParameter<T> &&other) : state(other.state)
        {
            switch (this->state)
            {
            case State::None:
                this->empty = nullptr;
                break;
            case State::Some:
                this->contents = std::move(other.contents);
                break;
            }
        }
        PossibleParameter(PossibleParameter<T> &other) = delete;
        PossibleParameter(const PossibleParameter<T> &other) = delete;

        ~PossibleParameter() {}
    };
}

#endif