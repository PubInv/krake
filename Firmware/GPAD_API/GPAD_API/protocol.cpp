#include <protocol.h>

using namespace protocol;

AlarmMessage::AlarmMessage(const char *const messageBytes, const size_t numBytes)
{
    auto index = 0;
    for (; index < numBytes; ++index)
    {
        this->messageBytes.at(index) = messageBytes[index];
    }
}

AlarmCommand::AlarmCommand(const AlarmCommand::Level alarmLevel, const AlarmMessage alarmMessage) : level(alarmLevel), message(std::move(alarmMessage)) {}

AlarmCommand AlarmCommand::deserialize(const char *const messageBytes, const size_t numBytes)
{
    // TODO: This will throw if either there is no 0th element or the static cast fails
    // Those should be differentiated into 2 exceptions for traceability
    const auto level = static_cast<AlarmCommand::Level>(messageBytes[0]);

    const auto alarmMessage = AlarmMessage(messageBytes + 1, numBytes - 1);

    return AlarmCommand(level, std::move(alarmMessage));
}

InfoCommand::InfoCommand()
{
}

InfoCommand InfoCommand::deserialize(const char *const messageBytes, const size_t numBytes)
{
    return InfoCommand();
}

const ProtocolMessage ProtocolMessage::deserialize(const char *const messageBytes, const size_t numBytes)
{
    // TODO: This should be wrapped in a try/catch if there isn't a 0th element
    auto commandType = static_cast<CommandType>(messageBytes[0]);

    auto remainingBytes = messageBytes + 1;

    switch (commandType)
    {
    case CommandType::ALARM:
        return ProtocolMessage(AlarmCommand::deserialize(remainingBytes, numBytes));
    case CommandType::INFO:
        return ProtocolMessage(InfoCommand::deserialize(remainingBytes, numBytes));
    }
    return ProtocolMessage();
}