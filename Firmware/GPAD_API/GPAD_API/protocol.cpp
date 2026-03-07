#include <protocol.h>

using namespace protocol;

AlarmCommand::AlarmCommand(const AlarmCommand::Level alarmLevel, const std::array<char, 80> alarmMessage) : level(alarmLevel), message(alarmMessage) {}

AlarmCommand AlarmCommand::deserialize(const char *const messageBytes)
{
    // TODO: Throw error if there is no 0th element;
    const auto level = static_cast<AlarmCommand::Level>(messageBytes[0]);

    const auto level = AlarmCommand::Level::Level1;
    return AlarmCommand(level, std::array<char, 80>());
}

InfoCommand::InfoCommand()
{
}

InfoCommand InfoCommand::deserialize(const char *const messageBytes)
{
    return InfoCommand();
}

const ProtocolMessage ProtocolMessage::deserialize(const char *const messageBytes)
{
    // TODO: This should be wrapped in a try/catch if there isn't a 0th element
    auto commandType = static_cast<CommandType>(messageBytes[0]);

    auto remainingBytes = messageBytes + 1;

    switch (commandType)
    {
    case CommandType::ALARM:
        return ProtocolMessage(AlarmCommand::deserialize(remainingBytes));
    case CommandType::INFO:
        return ProtocolMessage(InfoCommand::deserialize(remainingBytes));
    }
}