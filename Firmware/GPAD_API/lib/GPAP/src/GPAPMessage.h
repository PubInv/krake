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

#ifndef _GPAP_MESSAGE_H
#define _GPAP_MESSAGE_H

#include "AlarmMessage.h"
#include "HelpMessage.h"
#include "InfoMessage.h"
#include "MuteMessage.h"
#include "UnmuteMessage.h"

namespace gpap_message
{

    enum class MessageType : char
    {
        MUTE = 's',
        UNMUTE = 'u',
        HELP = 'h',
        ALARM = 'a',
        INFO = 'i',
    };
    struct GPAPMessage final
    {
    public:
        static const size_t BUFFER_LENGTH = 131;

        union
        {
            alarm::AlarmMessage alarm;
            InfoMessage info;
            MuteMessage mute;
            UnmuteMessage unmute;
            HelpMessage help;
        };

        MessageType messageType;

        using GPAPBuffer = std::array<char, GPAPMessage::BUFFER_LENGTH>;

        // Constructors and operator overloads
    public:
        explicit GPAPMessage(const alarm::AlarmMessage alarmMessage) noexcept
            : messageType(MessageType::ALARM), alarm(std::move(alarmMessage)) {}
        explicit GPAPMessage(const InfoMessage infoMessage) noexcept
            : messageType(MessageType::INFO), info(std::move(infoMessage)) {}
        explicit GPAPMessage(const MuteMessage muteMessage) noexcept
            : messageType(MessageType::MUTE), mute(std::move(muteMessage)) {}
        explicit GPAPMessage(const UnmuteMessage unmuteCommand) noexcept
            : messageType(MessageType::UNMUTE), unmute(std::move(unmuteCommand)) {}
        explicit GPAPMessage(const HelpMessage helpCommand) noexcept
            : messageType(MessageType::HELP), help(std::move(helpCommand)) {}
        GPAPMessage(const GPAPMessage &&other) noexcept
            : messageType(other.messageType)
        {
            switch (this->messageType)
            {
            case MessageType::ALARM:
                this->alarm = std::move(other.alarm);
                break;
            case MessageType::INFO:
                this->info = std::move(other.info);
                break;
            case MessageType::MUTE:
                this->mute = std::move(other.mute);
                break;
            case MessageType::UNMUTE:
                this->unmute = std::move(other.unmute);
                break;
            case MessageType::HELP:
                this->help = std::move(other.help);
                break;
            }
        }
        GPAPMessage &operator=(const GPAPMessage &&other) noexcept
        {
            if (this != &other)
            {
                this->messageType = other.messageType;
                switch (this->messageType)
                {
                case MessageType::ALARM:
                    this->alarm = std::move(other.alarm);
                    break;
                case MessageType::INFO:
                    this->info = std::move(other.info);
                    break;
                case MessageType::MUTE:
                    this->mute = std::move(other.mute);
                    break;
                case MessageType::UNMUTE:
                    this->unmute = std::move(other.unmute);
                    break;
                case MessageType::HELP:
                    this->help = std::move(other.help);
                    break;
                }
            }
            return *this;
        }

        ~GPAPMessage() {}

        GPAPMessage() = delete;
        GPAPMessage(GPAPMessage &other) = delete;

        static GPAPMessage deserialize(const char *const buffer, const size_t numBytes);
    };
}

#endif
