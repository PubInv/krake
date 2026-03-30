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
        GPAPMessage(alarm::AlarmMessage alarmMessage) noexcept
            : messageType(MessageType::ALARM), alarm(std::move(alarmMessage)) {}
        GPAPMessage(InfoMessage infoMessage) noexcept
            : messageType(MessageType::INFO), info(std::move(infoMessage)) {}
        GPAPMessage(MuteMessage muteMessage) noexcept
            : messageType(MessageType::MUTE), mute(std::move(muteMessage)) {}
        GPAPMessage(UnmuteMessage unmuteCommand) noexcept
            : messageType(MessageType::UNMUTE), unmute(std::move(unmuteCommand)) {}
        GPAPMessage(HelpMessage helpCommand) noexcept
            : messageType(MessageType::HELP), help(std::move(helpCommand)) {}

        GPAPMessage operator=(GPAPMessage &&other) noexcept
        {
            return std::move(other);
        }
        GPAPMessage(const GPAPMessage &&other)
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
        const GPAPMessage operator=(const GPAPMessage &&other)
        {
            return std::move(other);
        }

        ~GPAPMessage() {}

        GPAPMessage() = delete;
        GPAPMessage(GPAPMessage &other) = delete;

        static GPAPMessage deserialize(const char *const buffer, const size_t numBytes);
    };
}

#endif
