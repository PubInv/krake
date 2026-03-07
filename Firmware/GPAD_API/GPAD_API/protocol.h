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
        const std::array<char, 80> message;

    private:
        friend struct ProtocolMessage;

        // TODO: Throw error if the message cannot be deserialized from the bytes
        static AlarmCommand deserialize(const char *const messageBytes);

    public:
        explicit AlarmCommand(const Level level, const std::array<char, 80> message);

        AlarmCommand(AlarmCommand &&other) = default;
        AlarmCommand operator=(AlarmCommand &&other) noexcept
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
        static InfoCommand deserialize(const char *const messageBytes);

    public:
        InfoCommand(InfoCommand &&other) = default;
        InfoCommand operator=(InfoCommand &&other) noexcept
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
        ProtocolMessage(InfoCommand infoCommand) : commandType(CommandType::INFO), info(std::move(infoCommand)) {}

        ProtocolMessage(ProtocolMessage &&other) = default;
        ProtocolMessage operator=(ProtocolMessage &&other) noexcept
        {
            return std::move(other);
        }

        ProtocolMessage(ProtocolMessage &other) = delete;
        // TODO: This needs to through an error if the message could not be deserialized
        static const ProtocolMessage deserialize(const char *const messageBytes);

    private:
        ProtocolMessage();
    };
}

#endif
