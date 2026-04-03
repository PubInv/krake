/*
  Copyright (C) 2026 Public Invention

  This program includes free software: you can redistribute it and/or modify
  it under the terms of the GNU Affero General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  See the GNU Affero General Public License for more details.
  You should have received a copy of the GNU Affero General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

/*
  In the AlarmMessage, a couple of the fields are optional. The "message ID" and the "types designator".
  Treating the concrete type of AlarmMessageId and AlarmTypeDesignator as both having a possible value
  and also being empty can lead to many headaches and mistakes. To address this, an "optional" type has
  been added to clearly indicate if a field in the AlarmMessage is present or not.

  Note: In C++17 there is a native "optional" type called `std::optional`. At the time of creation for
  this library, it was compiled using C++11 so an "optional" type had to be manually implemented.
  https://en.cppreference.com/w/cpp/utility/optional.html
*/

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