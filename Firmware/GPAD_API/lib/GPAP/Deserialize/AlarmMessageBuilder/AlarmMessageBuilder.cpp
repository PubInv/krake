#include <Arduino.h>

#include "AlarmMessageBuilder.h"

using namespace gpap_message::alarm;
using namespace gpap_message::deserialize;

AlarmMessageBuilder::AlarmMessageBuilder()
    : level(AlarmMessage::Level::Level1), idLength(0), idBuffer({}),
      designatorLength(0), designatorBuffer({}), messageLength(0),
      messageBuffer({})
{
}

size_t AlarmMessageBuilder::deserializeLevelBytes(const char *const buffer,
                                                  const size_t numBytes)
{
    if (numBytes == 0)
    {
        throw;
    }

    Serial.printf("The alarm level: %c\n", buffer[0]);

    this->level = static_cast<AlarmMessage::Level>(buffer[0]);

    Serial.println("Inside ::deserializeLevelBytes()");
    switch (this->level)
    {
    case AlarmMessage::Level::Level1:
        Serial.println("Level 1");
        break;
    case AlarmMessage::Level::Level2:
        Serial.println("Level 2");
        break;
    case AlarmMessage::Level::Level3:
        Serial.println("Level 3");
        break;
    case AlarmMessage::Level::Level4:
        Serial.println("Level 4");
        break;
    case AlarmMessage::Level::Level5:
        Serial.println("Level 5");
        break;
    }
    Serial.println("Finish ::deserializeLevelBytes()");

    return 1;
}

size_t AlarmMessageBuilder::deserializeIdBytes(const char *const buffer,
                                               const size_t numBytes)
{
    if (numBytes == 0 || (buffer[0] != AlarmMessageBuilder::ID_START_CHARACTER))
    {
        return 0;
    }

    auto idLength = 0;
    auto foundEnd = false;

    // Going 1 more than the max since the end character, ], can be after the max
    // length of the ID
    while ((idLength < (AlarmMessageId::MAX_LENGTH + 1)) &&
           (idLength < numBytes) && !foundEnd)
    {
        // Need to offset the index by 1 since we have to account for the starting
        // character, [
        auto bufferIndex = idLength + 1;
        if (buffer[bufferIndex] == AlarmMessageBuilder::ID_END_CHARACTER)
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

        // Add 2 to the index value since that will account for the deliminating
        // characters, '[' and ']'
        return idLength + 2;
    }
}

size_t
AlarmMessageBuilder::deserializeTypeDesignatorBytes(const char *const buffer,
                                                    const size_t numBytes)
{
    if ((numBytes == 0) ||
        buffer[0] != AlarmMessageBuilder::DESIGNATOR_START_CHARACTER)
    {
        return 0;
    }

    auto designatorLength = 0;
    auto foundEnd = false;
    while ((designatorLength < (AlarmTypeDesignator::DESIGNATOR_LENGTH + 1)) &&
           (designatorLength < numBytes) && !foundEnd)
    {
        // Need to offset the index by 1 since we have to account for the starting
        // character, {
        auto bufferIndex = designatorLength + 1;
        if (buffer[bufferIndex] == AlarmMessageBuilder::DESIGNATOR_END_CHARACTER)
        {
            foundEnd = true;
        }
        else if (designatorLength < AlarmTypeDesignator::DESIGNATOR_LENGTH)
        {
            this->designatorBuffer.at(designatorLength) = buffer[bufferIndex];
            ++designatorLength;
        }
    }

    if (!foundEnd ||
        (designatorLength != AlarmTypeDesignator::DESIGNATOR_LENGTH &&
         designatorLength != 0))
    {
        throw;
    }
    else
    {
        this->designatorLength = designatorLength;

        // Add 2 to the index value since that will account for the deliminating
        // characters, '{' and '}'
        return designatorLength + 2;
    }
}

size_t AlarmMessageBuilder::deserializeMessageBytes(const char *const buffer,
                                                    const size_t numBytes)
{
    auto messageLength = 0;
    for (; messageLength < numBytes; ++messageLength)
    {
        this->messageBuffer.at(messageLength) = buffer[messageLength];
    }

    return messageLength;
}

AlarmMessage AlarmMessageBuilder::buildAlarmMessage(const char *const buffer,
                                                    const size_t numBytes)
{
    AlarmMessageBuilder builder = AlarmMessageBuilder();

    auto totalBytes = builder.deserializeLevelBytes(buffer, numBytes);

    // Iterate through buffer multiple times to find the elements/components
    // remove Bytes from the function names

    if ((numBytes - totalBytes) > 0)
    {
        totalBytes +=
            builder.deserializeIdBytes(buffer + totalBytes, numBytes - totalBytes);
    }

    if ((numBytes - totalBytes) > 0)
    {
        totalBytes += builder.deserializeTypeDesignatorBytes(buffer + totalBytes,
                                                             numBytes - totalBytes);
    }

    if ((numBytes - totalBytes) > 0)
    {
        totalBytes += builder.deserializeMessageBytes(buffer + totalBytes,
                                                      numBytes - totalBytes);
    }

    const auto messageId =
        AlarmMessageId(builder.idLength, std::move(builder.idBuffer));
    const auto typeDesignator = AlarmTypeDesignator(
        std::move(builder.designatorBuffer), (builder.designatorLength == 0));
    const auto content =
        AlarmContent(builder.messageLength, std::move(builder.messageBuffer));

    auto alarmMessage = AlarmMessage(builder.level, std::move(content), std::move(messageId),
                                     std::move(typeDesignator));

    Serial.println("Inside ::buildAlarmMessage()");
    switch (alarmMessage.level)
    {
    case AlarmMessage::Level::Level1:
        Serial.println("Level 1");
        break;
    case AlarmMessage::Level::Level2:
        Serial.println("Level 2");
        break;
    case AlarmMessage::Level::Level3:
        Serial.println("Level 3");
        break;
    case AlarmMessage::Level::Level4:
        Serial.println("Level 4");
        break;
    case AlarmMessage::Level::Level5:
        Serial.println("Level 5");
        break;
    }
    Serial.println("Finish ::buildAlarmMessage()");

    return alarmMessage;
}
