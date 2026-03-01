#include <protocol.h>

using namespace protocol;

const AlarmLevel DEFAULT_ALARM_LEVEL = AlarmLevel::Level1;

AlarmCommand::AlarmCommand() : level(DEFAULT_ALARM_LEVEL)
{
}

AlarmCommand::AlarmCommand(AlarmLevel level) : level(level) {}

AlarmCommand AlarmCommand::deserialize()
{
    AlarmLevel level = AlarmLevel::Level1;
    return AlarmCommand(level);
}

InfoCommand::InfoCommand()
{
}

InfoCommand InfoCommand::deserialize()
{
    return InfoCommand();
}
