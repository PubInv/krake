#ifndef _GPAP_MESSAGE_H
#define _GPAP_MESSAGE_H

#include <Arduino.h>

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

        const union
        {
            alarm::AlarmMessage alarm;
            InfoMessage info;
            MuteMessage mute;
            UnmuteMessage unmute;
            HelpMessage help;
        };

        const MessageType messageType;

        using GPAPBuffer = std::array<char, GPAPMessage::BUFFER_LENGTH>;

        // Constructors and operator overloads
    public:
        explicit GPAPMessage(const alarm::AlarmMessage alarmMessage) noexcept
            : messageType(MessageType::ALARM), alarm(std::move(alarmMessage))
        {
            auto data = this->alarm.typeDesignator.contents.getValue();
            for (auto iter = data.cbegin(); iter != data.cend(); ++iter)
            {
                Serial.printf("In GPAPMessage ctor: %c\n", *iter);
            }
        }
        explicit GPAPMessage(const InfoMessage infoMessage) noexcept
            : messageType(MessageType::INFO), info(std::move(infoMessage)) {}
        explicit GPAPMessage(const MuteMessage muteMessage) noexcept
            : messageType(MessageType::MUTE), mute(std::move(muteMessage)) {}
        explicit GPAPMessage(const UnmuteMessage unmuteCommand) noexcept
            : messageType(MessageType::UNMUTE), unmute(std::move(unmuteCommand)) {}
        explicit GPAPMessage(const HelpMessage helpCommand) noexcept
            : messageType(MessageType::HELP), help(std::move(helpCommand)) {}

        GPAPMessage(GPAPMessage &&other) noexcept
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
        GPAPMessage &operator=(GPAPMessage &&other)
        {
            return *this;
        }
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
        GPAPMessage &operator=(const GPAPMessage &&other)
        {
            return *this;
        }

        ~GPAPMessage() {}

        GPAPMessage() = delete;
        GPAPMessage(GPAPMessage &other) = delete;

        static GPAPMessage deserialize(const char *const buffer, const size_t numBytes);
    };
}

#endif
