#include <protocol.h>

using namespace protocol;

AlarmCommand::AlarmCommand(AlarmLevel alarmLevel, std::array<char, 80> alarmMessage) : level(alarmLevel), message(alarmMessage) {}

AlarmCommand AlarmCommand::deserialize(const char *const messageBytes)
{
    AlarmLevel level = AlarmLevel::Level1;
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
    ProtocolMessage protocolMessage = ProtocolMessage();
    protocolMessage.info = InfoCommand();

    return protocolMessage;
}