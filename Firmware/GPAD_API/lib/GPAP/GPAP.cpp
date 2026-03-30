#include "GPAP.h"
#include "Deserialize/AlarmMessageBuilder/AlarmMessageBuilder.h"

using namespace gpap_message;
GPAPMessage GPAPMessage::deserialize(const char *const buffer, const size_t numBytes)
{
    // Can't determined the message type if there are no bytes
    if (numBytes == 0)
    {
        throw;
    }

    // TODO: This should be wrapped in a try/catch if there isn't a 0th element
    auto messageType = static_cast<MessageType>(buffer[0]);

    switch (messageType)
    {
    case MessageType::ALARM:
        if ((numBytes - 1) == 0)
        {
            throw;
        }
        return std::move(GPAPMessage(deserialize::AlarmMessageBuilder::buildAlarmMessage(buffer + 1, numBytes - 1)));

        // case MessageType::INFO:
        //     return ProtocolMessage(InfoCommand());

        // case MessageType::MUTE:
        //     return ProtocolMessage(MuteCommand());

        // case MessageType::UNMUTE:
        //     return ProtocolMessage(UnmuteCommand());

        // case MessageType::HELP:
        //     return ProtocolMessage(HelpCommand());
        // }
    }
}