#include <protocol.h>

using namespace protocol;

AlarmMessage::AlarmMessage(const char *const messageBytes, const size_t numBytes)
{
    const auto messageLength = std::min(numBytes, AlarmMessage::MAX_SIZE);

    auto index = 0;
    for (; index < messageLength; ++index)
    {
        this->message.at(index) = messageBytes[index];
    }
}

AlarmMessageId AlarmMessageId::deserialize(const char *const bytes, const size_t numBytes)
{
    std::array<char, AlarmMessageId::MAX_LENGTH> id = {};

    auto idLength = [bytes, numBytes, &id]()
    {
        auto idLength = 0;

        auto foundStart = false;
        auto foundEnd = false;
        for (auto i = 0; i < numBytes; ++i)
        {
            if (foundEnd || idLength > AlarmMessageId::MAX_LENGTH)
            {
                break;
            }
            else if ((foundStart) && isxdigit(bytes[i]))
            {
                id.at(idLength) = bytes[i];
                idLength += 1;
            }
            else if (bytes[i] == AlarmMessageId::ID_START_CHARACTER)
            {
                foundStart = true;
            }
            else if (bytes[i] == AlarmMessageId::ID_END_CHARACTER)
            {
                foundEnd = true;
            }
            // If a character isn't hex and isn't a marker character, it is an error and should be thrown
            else if (!isxdigit(bytes[i]))
            {
                throw;
            }
        }

        // If the start and end characters were not found then we are unable to deserialize the ID.
        // This will result in throwing an error.
        if (!(foundStart && foundEnd))
        {
            throw;
        }

        return idLength;
    }();

    return AlarmMessageId(idLength, id);
}

AlarmCommand AlarmCommand::deserialize(const char *const messageBytes, const size_t numBytes)
{
    // TODO: This will throw if either there is no 0th element or the static cast fails
    // Those should be differentiated into 2 exceptions for traceability
    const auto level = static_cast<AlarmCommand::Level>(messageBytes[0]);

    // Parse message id. >= 1 characters indicating a hex value.
    // Parse alarm type designator. == 3 digits

    const auto messageId = AlarmMessageId::deserialize(messageBytes + 1, numBytes - 1);

    const auto alarmMessage = AlarmMessage(messageBytes + 1, numBytes - 1);

    return AlarmCommand(level, std::move(alarmMessage), std::move(messageId));
}

InfoCommand::InfoCommand()
{
}

InfoCommand InfoCommand::deserialize(const char *const messageBytes, const size_t numBytes)
{
    return InfoCommand();
}

ProtocolMessage ProtocolMessage::deserialize(const char *const messageBytes, const size_t numBytes)
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