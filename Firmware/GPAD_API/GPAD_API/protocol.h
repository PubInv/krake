#ifndef _PROTOCOL_H
#define _PROTOCOL_H

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
        friend union Command;

    private:
        AlarmLevel level;
        const char const *alarm;

    public:
        explicit AlarmCommand(AlarmLevel level, const char *const alarmMessage);
        static AlarmCommand deserialize(const char *const messageBytes);
    };

    class InfoCommand
    {
    private:
        friend union Command;
        friend class ProtocolMessage;

    public:
        explicit InfoCommand();
        static InfoCommand deserialize(const char *const messageBytes);
    };

    union Command
    {
        AlarmCommand alarm;
        InfoCommand info;

    public:
        explicit Command(const InfoCommand infoCommand);
        explicit Command(const AlarmCommand alarmCommand);
    };

    class ProtocolMessage
    {
        // Methods
    private:
        explicit ProtocolMessage(CommandType commandType, Command command);

        // Data members
    private:
        CommandType commandType;
        Command command;

    public:
        static ProtocolMessage deserialize(const char *const messageBytes);
    };
}

#endif
