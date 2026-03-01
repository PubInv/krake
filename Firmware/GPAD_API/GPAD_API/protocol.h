#ifndef _PROTOCOL_H
#define _PROTOCOL_H

namespace protocol
{
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
        AlarmLevel level;

        explicit AlarmCommand();
        explicit AlarmCommand(AlarmLevel level);

    public:
        static AlarmCommand deserialize();
    };

    class InfoCommand
    {
    private:
        explicit InfoCommand();

    public:
        static InfoCommand deserialize();
    };
}

#endif
