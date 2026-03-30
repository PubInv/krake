#ifndef _GPAP_MESSAGE_H
#define _GPAP_MESSAGE_H

#include "AlarmMessage/AlarmMessage.h"

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
            // InfoCommand info;
            // MuteCommand mute;
            // UnmuteCommand unmute;
            // HelpCommand help;
        };

        const MessageType messageType;

        using GPAPBuffer = std::array<char, GPAPMessage::BUFFER_LENGTH>;

        // Constructors and operator overloads
    public:
        GPAPMessage(alarm::AlarmMessage alarmMessage) noexcept
            : messageType(MessageType::ALARM), alarm(std::move(alarmMessage)) {}
        // ProtocolMessage(InfoCommand infoCommand) noexcept
        //     : commandType(CommandType::INFO), info(std::move(infoCommand)) {}
        // ProtocolMessage(MuteCommand muteCommand) noexcept
        //     : commandType(CommandType::MUTE), mute(std::move(muteCommand)) {}
        // ProtocolMessage(UnmuteCommand unmuteCommand) noexcept
        //     : commandType(CommandType::UNMUTE), unmute(std::move(unmuteCommand)) {}
        // ProtocolMessage(HelpCommand helpCommand) noexcept
        //     : commandType(CommandType::HELP), help(std::move(helpCommand)) {}

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
                // case CommandType::INFO:
                //     this->info = std::move(other.info);
                //     break;
                // case CommandType::MUTE:
                //     this->mute = std::move(other.mute);
                //     break;
                // case CommandType::UNMUTE:
                //     this->unmute = std::move(other.unmute);
                //     break;
                // case CommandType::HELP:
                //     this->help = std::move(other.help);
                //     break;
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
