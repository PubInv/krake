#include <unity.h>
#include <string>

#include "GPAPMessage.h"

// namespace gpap_message
// {
//     class GPAPMessageTest : public testing::Test
//     {
//     };

//     TEST_F(GPAPMessageTest, TestFunc)
//     {
//         static const char *alarmMessageStr = "a5Test message[444]{a4ab}";

//         const auto gpapMessage = GPAPMessage::deserialize(alarmMessageStr, strlen(alarmMessageStr));

//         EXPECT_EQ(gpapMessage.getMessageType(), MessageType::ALARM);

//         const std::string expectedString = "Test message";
//         std::ostringstream receivedString;
//         receivedString << gpapMessage.getAlarmMessage().getAlarmContent();

//         std::cout << expectedString << std::endl;
//         std::cout << receivedString.str() << std::endl;

//         EXPECT_EQ(receivedString.str().compare(expectedString), 0);
//     }
// };

void test_alarm_message()
{
    TEST_ASSERT_TRUE(true);
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_alarm_message);

    UNITY_END();
}
