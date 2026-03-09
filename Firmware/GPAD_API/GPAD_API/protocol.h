#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include <array>

namespace protocol
{
    union Command;
    class ProtocolMessage;

    enum class CommandType : char
    {
        MUTE = 's',
        UNMUTE = 'u',
        HELP = 'h',
        ALARM = 'a',
        INFO = 'i',
    };

    class AlarmMessage final
    {
    public:
        static const size_t MAX_SIZE = 80;

    private:
        std::array<char, AlarmMessage::MAX_SIZE> messageBytes;

    public:
        AlarmMessage(const char *const messageBytes, const size_t numBytes);

        AlarmMessage(AlarmMessage &&other) = default;
        AlarmMessage operator=(AlarmMessage &&other)
        {
            return std::move(other);
        }
        AlarmMessage(const AlarmMessage &&other) : messageBytes(std::move(other.messageBytes)) {}
        const AlarmMessage operator=(const AlarmMessage &&other)
        {
            return std::move(other);
        }

        AlarmMessage() = delete;
        AlarmMessage(AlarmMessage &other) = delete;
        AlarmMessage(const AlarmMessage &other) = delete;
    };

    class AlarmCommand
    {

    public:
        enum class Level : char
        {
            Level1 = 1,
            Level2 = 2,
            Level3 = 3,
            Level4 = 4,
            Level5 = 5,
        };

    private:
        const Level level;
        const AlarmMessage message;

    private:
        friend struct ProtocolMessage;

        // TODO: Throw error if the message cannot be deserialized from the bytes
        static AlarmCommand deserialize(const char *const messageBytes, const size_t numBytes);

    public:
        explicit AlarmCommand(const Level level, const AlarmMessage message);

        AlarmCommand(AlarmCommand &&other) = default;
        AlarmCommand(const AlarmCommand &&other) : level(other.level), message(std::move(other.message)) {}
        AlarmCommand operator=(AlarmCommand &&other) noexcept
        {
            return std::move(other);
        }
        const AlarmCommand operator=(const AlarmCommand &&other)
        {
            return std::move(other);
        }
        AlarmCommand() = delete;
        AlarmCommand(AlarmCommand &other) = delete;
    };

    class InfoCommand
    {
    private:
        friend struct ProtocolMessage;

        InfoCommand();
        // TODO: Throw error if the message cannot be deserialized from the bytes
        static InfoCommand deserialize(const char *const messageBytes, const size_t numBytes);

    public:
        InfoCommand(InfoCommand &&other) = default;
        InfoCommand operator=(InfoCommand &&other) noexcept
        {
            return std::move(other);
        }

        InfoCommand(const InfoCommand &&other)
        {
        }
        const InfoCommand operator=(const InfoCommand &&other)
        {
            return std::move(other);
        }

        InfoCommand(InfoCommand &other) = delete;
    };

    struct ProtocolMessage final
    {
    private:
        const union
        {
            AlarmCommand alarm;
            InfoCommand info;
        };

        const CommandType commandType;

    public:
        ProtocolMessage(AlarmCommand alarmCommand) : commandType(CommandType::ALARM), alarm(std::move(alarmCommand)) {}
        ProtocolMessage(InfoCommand infoCommand) : commandType(CommandType::INFO), info(std::move(infoCommand))
        {
        }

        ProtocolMessage(ProtocolMessage &&other) = default;
        ProtocolMessage operator=(ProtocolMessage &&other) noexcept
        {
            return std::move(other);
        }
        ProtocolMessage(const ProtocolMessage &&other) : commandType(other.commandType)
        {
            switch (this->commandType)
            {
            case CommandType::ALARM:
                this->alarm = std::move(other.alarm);
                break;
            case CommandType::INFO:
                this->info = std::move(other.info);
                break;
            }
        }
        const ProtocolMessage operator=(const ProtocolMessage &&other)
        {
            return std::move(other);
        }

        ProtocolMessage(ProtocolMessage &other) = delete;

        // TODO: This needs to through an error if the message could not be deserialized
        static const ProtocolMessage deserialize(const char *const messageBytes, const size_t numBytes);

    private:
        ProtocolMessage();
    };
}

#endif
