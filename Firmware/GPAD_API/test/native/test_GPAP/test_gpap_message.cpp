#include <gtest/gtest.h>

#include "GPAPMessage.h"

namespace gpap_message
{
    class GPAPMessageTest : public testing::Test
    {
    };

    TEST_F(GPAPMessageTest, EmptyTestFunc)
    {
        static const char *alarmMessageStr = "a5Test message[444]{a4ab}";

        const auto gpapMessage = GPAPMessage::deserialize(alarmMessageStr, strlen(alarmMessageStr));

        EXPECT_EQ(gpapMessage.messageType, MessageType::ALARM);
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
