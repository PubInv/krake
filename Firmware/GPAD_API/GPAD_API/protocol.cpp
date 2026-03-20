#include <protocol.h>

#include <algorithm>

using namespace protocol;

AlarmMessageId::AlarmMessageId(const size_t idLength, const std::array<char, AlarmMessageId::MAX_LENGTH> id)
    : idLength(idLength), id(std::move(id))
{
    // The recorded real-length of the message ID cannot me more than the max number of elements
    if (this->idLength > this->id.size())
    {
        throw;
    }

    auto startIterator = this->id.cbegin();
    auto endIterator = this->id.cbegin() + this->idLength;

    bool allHex = std::all_of(
        startIterator,
        endIterator,
        [](char hexChar)
        {
            return isxdigit(hexChar);
        });

    // If all the characters are NOT hex characters we need to throw an error and cancel the creation of
    // this instance.
    if (!allHex)
    {
        throw;
    }
}

AlarmTypeDesignator::AlarmTypeDesignator(const std::array<char, AlarmTypeDesignator::DESIGNATOR_LENGTH> designator) : designator(designator)
{
    bool allDigits = std::all_of(
        this->designator.cbegin(),
        this->designator.cend(),
        [](char c)
        {
            return isdigit(c);
        });

    // if the characters are not all digits we want to throw
    if (!allDigits)
    {
        throw;
    }
}

AlarmMessageId ProtocolMessage::deserializeAlarmMessageId(const char *const bytes, const size_t numBytes)
{
    auto idLength = 0;
    std::array<char, AlarmMessageId::MAX_LENGTH> id = {};

    auto foundStart = false;
    auto foundEnd = false;

    for (auto index = 0; index < numBytes; ++index)
    {
        if (foundEnd || idLength > AlarmMessageId::MAX_LENGTH)
        {
            break;
        }
        else if (foundStart)
        {
            id.at(idLength) = bytes[index];
            idLength += 1;
        }
        else if (bytes[index] == AlarmMessageId::ID_START_CHARACTER)
        {
            foundStart = true;
        }
        else if (bytes[index] == AlarmMessageId::ID_END_CHARACTER)
        {
            foundEnd = true;
        }
    }

    return AlarmMessageId(idLength, id);
}

AlarmTypeDesignator ProtocolMessage::deserializeAlarmTypeDesignator(const char *const bytes, const size_t numBytes)
{
    // The bytes length is 2 more than the designator length due to do the '[' and ']'
    // characters
    static const size_t BYTES_LENGTH = AlarmTypeDesignator::DESIGNATOR_LENGTH + 2;

    // If the number of bytes is not equal to the exact length of the designator we throw an error
    if (numBytes != BYTES_LENGTH)
    {
        throw;
    }

    auto idLength = 0;
    std::array<char, AlarmTypeDesignator::DESIGNATOR_LENGTH> id = {};

    auto foundStart = false;
    auto foundEnd = false;

    for (auto index = 0; index < numBytes; ++index)
    {
        if (foundEnd || idLength > AlarmTypeDesignator::DESIGNATOR_LENGTH)
        {
            break;
        }
        else if (foundStart)
        {
            id.at(idLength) = bytes[index];
            idLength += 1;
        }
        else if (bytes[index] == AlarmTypeDesignator::DESIGNATOR_START_CHARACTER)
        {
            foundStart = true;
        }
        else if (bytes[index] == AlarmTypeDesignator::DESIGNATOR_END_CHARACTER)
        {
            foundEnd = true;
        }
    }

    return AlarmTypeDesignator(id);
}

AlarmCommand ProtocolMessage::deserializeAlarmCommand(const char *const bytes, const size_t numBytes)
{
    // TODO: This will throw if either there is no 0th element or the static cast fails
    // Those should be differentiated into 2 exceptions for traceability
    const auto level = static_cast<AlarmCommand::Level>(bytes[0]);

    // Parse message id. >= 1 characters indicating a hex value.
    // Parse alarm type designator. == 3 digits

    // Characters a + b + c + d + 1 + 2 + 3 = 7 charactersf
    const auto idLength = 7;
    std::array<char, AlarmMessageId::MAX_LENGTH> id = {'a', 'b', 'c', 'd', 1, 2, 3};
    const std::array<char, AlarmMessage::MAX_LENGTH> message = {
        'T', 'h', 'i', 's', ' ',
        'i', 's', ' ',
        't', 'h', 'e', ' ',
        'm', 'e', 's', 's', 'a', 'g', 'e', '.'};
    const std::array<char, AlarmTypeDesignator::DESIGNATOR_LENGTH> designator = {};

    return AlarmCommand(level, AlarmMessage(message), AlarmMessageId(idLength, id), AlarmTypeDesignator(designator));
}

ProtocolMessage ProtocolMessage::deserialize(const char *const messageBytes, const size_t numBytes)
{
    // TODO: This should be wrapped in a try/catch if there isn't a 0th element
    auto commandType = static_cast<CommandType>(messageBytes[0]);

    auto remainingBytes = messageBytes + 1;

    switch (commandType)
    {
    case CommandType::ALARM:
        return ProtocolMessage(ProtocolMessage::deserializeAlarmCommand(remainingBytes, numBytes));
    case CommandType::INFO:
        return ProtocolMessage(InfoCommand());
    }

    // In the event the switch falls through, throw an error
    throw;
}
