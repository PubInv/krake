#include <protocol.h>

using namespace protocol;

AlarmCommand::AlarmCommand(AlarmLevel alarmLevel, const char *const alarmMessage) : level(alarmLevel), alarm(alarmMessage) {}

AlarmCommand AlarmCommand::deserialize(const char *const messageBytes)
{
    AlarmLevel level = AlarmLevel::Level1;
    return AlarmCommand(level, messageBytes);
}

InfoCommand::InfoCommand()
{
}

InfoCommand InfoCommand::deserialize(const char *const messageBytes)
{
    return InfoCommand();
}

Command::Command(const InfoCommand infoCommand) : info(infoCommand) {}
Command::Command(const AlarmCommand alarmCommand) : alarm(alarmCommand) {}

ProtocolMessage::ProtocolMessage(CommandType commandType, Command command) : commandType(commandType), command(command) {}

ProtocolMessage ProtocolMessage::deserialize(const char *const messageBytes)
{
    CommandType commandType = static_cast<CommandType>(messageBytes[0]);

    const char *const remainingBytes = messageBytes + 1;
    Command command = [commandType, remainingBytes]()
    {
        switch (commandType)
        {
        case CommandType::ALARM:
            return Command(AlarmCommand::deserialize(remainingBytes));
        case CommandType::INFO:
            return Command(InfoCommand::deserialize(remainingBytes));
        }
    }();

    Command command = Command(InfoCommand());

    return ProtocolMessage(commandType, command);
}