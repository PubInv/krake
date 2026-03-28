#include <protocol.h>

#include <algorithm>

#include <Print.h>
#include <Arduino.h>

using namespace protocol;

AlarmMessageId::AlarmMessageId(const size_t idLength, const std::array<char, AlarmMessageId::MAX_LENGTH> id)
    : idLength(idLength), id(AlarmMessageId::validateId(idLength, id))
{
}

std::array<char, AlarmMessageId::TOTAL_MAX_LENGTH>
AlarmMessageId::validateId(const size_t idLength, const std::array<char, AlarmMessageId::MAX_LENGTH> id)
{
    std::array<char, AlarmMessageId::TOTAL_MAX_LENGTH> validatedId = {};
    auto validatedIdIterator = validatedId.begin();

    // The recorded real-length of the message ID cannot me more than the max number of elements
    if (idLength > id.size())
    {
        throw;
    }

    auto startIterator = id.cbegin();
    auto endIterator = id.cbegin() + idLength;

    bool allHex = std::all_of(
        startIterator,
        endIterator,
        [&validatedIdIterator](char hexChar)
        {
            *validatedIdIterator = hexChar;
            validatedIdIterator = std::next(validatedIdIterator, 1);
            return isxdigit(hexChar);
        });

    // If all the characters are NOT hex characters we need to throw an error and cancel the creation of
    // this instance.
    if (!allHex)
    {
        throw;
    }

    *validatedIdIterator = '\0';
    return validatedId;
}

AlarmTypeDesignator::AlarmTypeDesignator(const std::array<char, AlarmTypeDesignator::DESIGNATOR_LENGTH> designator, const bool empty)
    : designator(AlarmTypeDesignator::validateDesignator(designator, empty)), empty(empty)
{
}

AlarmTypeDesignator::~AlarmTypeDesignator() = default;

const char *const AlarmTypeDesignator::getValue() const
{
    return this->designator.data();
}

size_t AlarmTypeDesignator::printTo(Print &print) const
{
    return print.print(this->getValue());
}

std::array<char, AlarmTypeDesignator::TOTAL_DESIGNATOR_LENGTH>
AlarmTypeDesignator::validateDesignator(const std::array<char, AlarmTypeDesignator::DESIGNATOR_LENGTH> inputDesignator, const bool empty)
{
    std::array<char, AlarmTypeDesignator::TOTAL_DESIGNATOR_LENGTH> designator = {};
    if (empty)
    {
        return designator;
    }

    auto designatorIterator = designator.begin();

    bool allDigits = std::all_of(
        inputDesignator.cbegin(),
        inputDesignator.cend(),
        [&](char inputCharacter)
        {
            *designatorIterator = inputCharacter;
            designatorIterator = std::next(designatorIterator, 1);
            return isdigit(inputCharacter);
        });

    // if the characters are not all digits we want to throw
    if (!allDigits)
    {
        throw;
    }

    *designatorIterator = '\0';

    return designator;
}

char AlarmCommand::alarmLevelIntoChar() const
{
    switch (this->level)
    {
    case AlarmCommand::Level::Level1:
        return '1';
    case AlarmCommand::Level::Level2:
        return '2';
    case AlarmCommand::Level::Level3:
        return '3';
    case AlarmCommand::Level::Level4:
        return '4';
    case AlarmCommand::Level::Level5:
        return '5';
    }

    Serial.println("End of the level to char method");
    return '0';
}

ProtocolMessage ProtocolMessage::deserialize(const char *const buffer, const size_t numBytes)
{
    // Can't determined the message type if there are no bytes
    if (numBytes == 0)
    {
        throw;
    }

    // TODO: This should be wrapped in a try/catch if there isn't a 0th element
    auto commandType = static_cast<CommandType>(buffer[0]);

    switch (commandType)
    {
    case CommandType::ALARM:
        if ((numBytes - 1) == 0)
        {
            throw;
        }
        return ProtocolMessage(AlarmCommandBuilder::buildAlarmCommand(buffer + 1, numBytes - 1));

    case CommandType::INFO:
        return ProtocolMessage(InfoCommand());

    case CommandType::MUTE:
        return ProtocolMessage(MuteCommand());

    case CommandType::UNMUTE:
        return ProtocolMessage(UnmuteCommand());

    case CommandType::HELP:
        return ProtocolMessage(HelpCommand());
    }
}

AlarmCommandBuilder::AlarmCommandBuilder()
    : level(AlarmCommand::Level::Level1),
      idLength(0),
      idBuffer({}),
      designatorLength(0),
      designatorBuffer({}),
      messageLength(0),
      messageBuffer({}) {}

size_t AlarmCommandBuilder::deserializeLevelBytes(const char *const buffer, const size_t numBytes)
{
    if (numBytes == 0)
    {
        throw;
    }

    this->level = static_cast<AlarmCommand::Level>(buffer[0]);

    return 1;
}

size_t AlarmCommandBuilder::deserializeIdBytes(const char *const buffer, const size_t numBytes)
{
    if (numBytes == 0 || (buffer[0] != AlarmCommandBuilder::ID_START_CHARACTER))
    {
        return 0;
    }

    auto idLength = 0;
    auto foundEnd = false;

    // Going 1 more than the max since the end character, ], can be after the max length of the ID
    while ((idLength < (AlarmMessageId::MAX_LENGTH + 1)) && (idLength < numBytes) && !foundEnd)
    {
        // Need to offset the index by 1 since we have to account for the starting character, [
        auto bufferIndex = idLength + 1;
        if (buffer[bufferIndex] == AlarmCommandBuilder::ID_END_CHARACTER)
        {
            foundEnd = true;
        }
        else if (idLength < AlarmMessageId::MAX_LENGTH)
        {
            this->idBuffer.at(idLength) = buffer[bufferIndex];
            ++idLength;
        }
    }

    // If the terminating character, }, was not found then it is an invalid string
    if (!foundEnd)
    {
        throw;
    }
    else
    {
        this->idLength = idLength;

        // Add 2 to the index value since that will account for the deliminating characters, '[' and ']'
        return idLength + 2;
    }
}

size_t AlarmCommandBuilder::deserializeTypeDesignatorBytes(const char *const buffer, const size_t numBytes)
{
    if ((numBytes == 0) || buffer[0] != AlarmCommandBuilder::DESIGNATOR_START_CHARACTER)
    {
        return 0;
    }

    auto designatorLength = 0;
    auto foundEnd = false;
    while ((designatorLength < (AlarmTypeDesignator::DESIGNATOR_LENGTH + 1)) && (designatorLength < numBytes) && !foundEnd)
    {
        // Need to offset the index by 1 since we have to account for the starting character, {
        auto bufferIndex = designatorLength + 1;
        if (buffer[bufferIndex] == AlarmCommandBuilder::DESIGNATOR_END_CHARACTER)
        {
            foundEnd = true;
        }
        else if (designatorLength < AlarmTypeDesignator::DESIGNATOR_LENGTH)
        {
            this->designatorBuffer.at(designatorLength) = buffer[bufferIndex];
            ++designatorLength;
        }
    }

    if (!foundEnd || (designatorLength != AlarmTypeDesignator::DESIGNATOR_LENGTH && designatorLength != 0))
    {
        throw;
    }
    else
    {
        this->designatorLength = designatorLength;

        // Add 2 to the index value since that will account for the deliminating characters, '{' and '}'
        return designatorLength + 2;
    }
}

size_t AlarmCommandBuilder::deserializeMessageBytes(const char *const buffer, const size_t numBytes)
{
    auto messageLength = 0;
    for (; messageLength < numBytes; ++messageLength)
    {
        this->messageBuffer.at(messageLength) = buffer[messageLength];
    }

    return messageLength;
}

AlarmCommand AlarmCommandBuilder::buildAlarmCommand(const char *const buffer, const size_t numBytes)
{
    AlarmCommandBuilder builder = AlarmCommandBuilder();

    auto totalBytes = builder.deserializeLevelBytes(buffer, numBytes);

    if ((numBytes - totalBytes) > 0)
    {
        totalBytes += builder.deserializeIdBytes(buffer + totalBytes, numBytes - totalBytes);
    }

    if ((numBytes - totalBytes) > 0)
    {
        totalBytes += builder.deserializeTypeDesignatorBytes(buffer + totalBytes, numBytes - totalBytes);
    }

    if ((numBytes - totalBytes) > 0)
    {

        totalBytes += builder.deserializeMessageBytes(buffer + totalBytes, numBytes - totalBytes);
    }

    const auto messageId = AlarmMessageId(builder.idLength, std::move(builder.idBuffer));
    const auto typeDesignator = AlarmTypeDesignator(std::move(builder.designatorBuffer), (builder.designatorLength == 0));
    const auto message = AlarmMessage(builder.messageLength, std::move(builder.messageBuffer));

    return AlarmCommand(builder.level, std::move(message), std::move(messageId), std::move(typeDesignator));
}
