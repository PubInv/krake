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

#include "GPAPMessage.h"
#include "Deserialize/AlarmMessageBuilder/AlarmMessageBuilder.h"

using namespace gpap_message;
GPAPMessage GPAPMessage::deserialize(const char *const buffer, const std::size_t numBytes)
{
    // Can't determined the message type if there are no bytes
    if (numBytes == 0)
    {
        throw;
    }

    // TODO: This should be wrapped in a try/catch if there isn't a 0th element
    const auto messageType = static_cast<MessageType>(buffer[0]);

    switch (messageType)
    {
    case MessageType::ALARM:
        if ((numBytes - 1) == 0)
        {
            throw;
        }

        return GPAPMessage(std::move(deserialize::AlarmMessageBuilder::buildAlarmMessage(buffer + 1, numBytes - 1)));

    case MessageType::INFO:
        return GPAPMessage(InfoMessage());

    case MessageType::MUTE:
        return GPAPMessage(MuteMessage());

    case MessageType::UNMUTE:
        return GPAPMessage(UnmuteMessage());

    case MessageType::HELP:
        return GPAPMessage(HelpMessage());

    default:
        throw;
    }
}

MessageType GPAPMessage::getMessageType() const noexcept
{
    return this->messageType;
}

const alarm::AlarmMessage &GPAPMessage::getAlarmMessage() const
{
    // Cannot access a field of the union when it's the incorrect type
    if (this->messageType != MessageType::ALARM)
    {
        throw;
    }

    return this->alarm;
}
