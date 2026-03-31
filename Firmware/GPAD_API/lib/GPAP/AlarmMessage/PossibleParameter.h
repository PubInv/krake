#ifndef _POSSIBLE_PARAMETER_H
#define _POSSIBLE_PARAMETER_H

namespace gpap_message::alarm
{
    template <class T>
    struct PossibleParameter
    {
        enum State
        {
            None,
            Some
        };

        const State state;
        const T contents;

        PossibleParameter()
        {
            this->state = State::None;
        }

        PossibleParameter(T contents) : contents(std::move(contents)), state(State::Some) {}
        PossibleParameter(const PossibleParameter<T> &&other) : state(other.state), contents(std::move(other.contents)) {}
        PossibleParameter(PossibleParameter<T> &other) = delete;
        PossibleParameter(const PossibleParameter<T> &other) = delete;
    };
}

#endif