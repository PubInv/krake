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

    enum class AlarmLevel : char
    {
        Level1 = 1,
        Level2 = 2,
        Level3 = 3,
        Level4 = 4,
        Level5 = 5,
    };

    class AlarmCommand
    {

    private:
        AlarmCommand();
        friend struct ProtocolMessage;

    private:
        AlarmLevel level;
        std::array<char, 80> message;

    public:
        explicit AlarmCommand(AlarmLevel level, std::array<char, 80> message);

        AlarmCommand(AlarmCommand &&other) = default;
        AlarmCommand operator=(AlarmCommand &&other)
        {
            return std::move(other);
        }
        AlarmCommand(AlarmCommand &other) = delete;

        static AlarmCommand deserialize(const char *const messageBytes);
    };

    class InfoCommand
    {
    private:
        InfoCommand();
        friend struct ProtocolMessage;

    public:
        InfoCommand(InfoCommand &&other) = default;
        InfoCommand operator=(InfoCommand &&other)
        {
            return std::move(other);
        }
        InfoCommand(InfoCommand &other) = delete;

        static InfoCommand deserialize(const char *const messageBytes);
    };

    struct ProtocolMessage final
    {
    private:
        enum
        {
            ALARM,
            INFO
        } command;

        union
        {
            AlarmCommand alarm;
            InfoCommand info;
        };

    public:
        ProtocolMessage(ProtocolMessage &&other) = default;

        ProtocolMessage(ProtocolMessage &other) = delete;
        static const ProtocolMessage deserialize(const char *const messageBytes);

    private:
        ProtocolMessage();
    };
}

#endif
