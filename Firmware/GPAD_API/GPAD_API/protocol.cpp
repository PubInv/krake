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

AlarmTypeDesignator::AlarmTypeDesignator(const std::array<char, AlarmTypeDesignator::DESIGNATOR_LENGTH> designator)
    : designator(AlarmTypeDesignator::validateDesignator(designator))
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
AlarmTypeDesignator::validateDesignator(const std::array<char, AlarmTypeDesignator::DESIGNATOR_LENGTH> inputDesignator)
{
    std::array<char, AlarmTypeDesignator::TOTAL_DESIGNATOR_LENGTH> designator = {};
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

AlarmMessageId ProtocolMessage::deserializeAlarmMessageId(ProtocolBuffer::const_iterator start, const ProtocolBuffer::const_iterator end)
{
    if (*start != AlarmMessageId::ID_START_CHARACTER)
    {
        throw;
    }

    auto idLength = 0;
    std::array<char, AlarmMessageId::MAX_LENGTH> id = {};

    ++start;
    while ((idLength < AlarmMessageId::MAX_LENGTH) && (start != end) && (*start != AlarmMessageId::ID_END_CHARACTER))
    {
        Serial.printf("Current Message Id char: %c\n", *start);
        id.at(idLength) = *start;
        idLength += 1;

        ++start;
    }

    return AlarmMessageId(idLength, id);
}

AlarmTypeDesignator ProtocolMessage::deserializeAlarmTypeDesignator(ProtocolBuffer::const_iterator start, const ProtocolBuffer::const_iterator end)
{
    // // The bytes length is 2 more than the designator length due to do the '[' and ']'
    // // characters
    // static const size_t BYTES_LENGTH = AlarmTypeDesignator::DESIGNATOR_LENGTH + 2;

    // // If the number of bytes is not equal to the exact length of the designator we throw an error
    // if (numBytes != BYTES_LENGTH || bytes[0] != AlarmTypeDesignator::DESIGNATOR_START_CHARACTER)
    // {
    //     throw;
    // }

    // auto idLength = 0;
    // std::array<char, AlarmTypeDesignator::DESIGNATOR_LENGTH> id = {};

    // auto foundEnd = false;

    // for (auto index = 0; index < numBytes; ++index)
    // {
    //     if (foundEnd || idLength >= AlarmTypeDesignator::DESIGNATOR_LENGTH)
    //     {
    //         break;
    //     }
    //     else if (bytes[index] == AlarmTypeDesignator::DESIGNATOR_END_CHARACTER)
    //     {
    //         foundEnd = true;
    //     }
    //     else
    //     {
    //         id.at(idLength) = bytes[index];
    //         idLength += 1;
    //     }
    // }

    // if (!foundEnd)
    // {
    //     throw;
    // }

    return AlarmTypeDesignator({'1', '2', '3'});
}

AlarmCommand ProtocolMessage::deserializeAlarmCommand(ProtocolBuffer::const_iterator start, const ProtocolBuffer::const_iterator end)
{
    // TODO: This will throw if either there is no 0th element or the static cast fails
    // Those should be differentiated into 2 exceptions for traceability
    const auto level = static_cast<AlarmCommand::Level>(*start);
    ++start;

    std::array<char, AlarmMessageId::MAX_LENGTH> id = {};

    // Parse message id. >= 1 characters indicating a hex value.
    // Parse alarm type designator. == 3 digits

    // Characters a + b + c + d + 1 + 2 + 3 = 7 charactersf
    const auto idLength = 7;
    const std::array<char, AlarmMessage::MAX_LENGTH> message = {
        'T', 'h', 'i', 's', ' ',
        'i', 's', ' ',
        't', 'h', 'e', ' ',
        'm', 'e', 's', 's', 'a', 'g', 'e', '.'};
    const std::array<char, AlarmTypeDesignator::DESIGNATOR_LENGTH> designator = {'1', '2', '3'};

    // TODO: Catch error from message ID
    const auto messageId = ProtocolMessage::deserializeAlarmMessageId(start, end);

    // TODO: Catch error from type designator
    const auto typeDesignator = ProtocolMessage::deserializeAlarmTypeDesignator(start, end);

    return AlarmCommand(level, AlarmMessage(message), std::move(messageId), std::move(typeDesignator));
}

ProtocolMessage ProtocolMessage::deserialize(const std::array<char, ProtocolMessage::BUFFER_LENGTH> buffer)
{
    auto bufferIterator = buffer.cbegin();
    const auto bufferIteratorEnd = buffer.cend();

    // TODO: This should be wrapped in a try/catch if there isn't a 0th element
    auto commandType = static_cast<CommandType>(*bufferIterator);

    switch (commandType)
    {
    case CommandType::ALARM:
        return ProtocolMessage(ProtocolMessage::deserializeAlarmCommand(++bufferIterator, bufferIteratorEnd));
    case CommandType::INFO:
        return ProtocolMessage(InfoCommand());
    }

    // In the event the switch falls through, throw an error
    throw;
}
