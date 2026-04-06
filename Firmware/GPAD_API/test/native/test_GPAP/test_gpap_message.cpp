#include <gtest/gtest.h>
#include <string>
#include <sstream>

#include "GPAPMessage.h"

namespace gpap_message
{
    class GPAPMessageTest : public testing::Test
    {
    };

    TEST_F(GPAPMessageTest, TestFunc)
    {
        static const char *alarmMessageStr = "a5Test message[444]{a4ab}";

        const auto gpapMessage = GPAPMessage::deserialize(alarmMessageStr, strlen(alarmMessageStr));

        EXPECT_EQ(gpapMessage.getMessageType(), MessageType::ALARM);

        const std::string expectedString = "Test message";
        std::ostringstream receivedString;
        receivedString << gpapMessage.getAlarmMessage().getAlarmContent();

        std::cout << expectedString << std::endl;
        std::cout << receivedString.str() << std::endl;

        EXPECT_EQ(receivedString.str().compare(expectedString), 0);
    }
};

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    // if you plan to use GMock, replace the line above with
    // ::testing::InitGoogleMock(&argc, argv);

    if (RUN_ALL_TESTS())
        ;

    // Always return zero-code and allow PlatformIO to parse results
    return 0;
}
