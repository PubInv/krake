#include <unity.h>
#include <string>
#include <cstring>
#include <vector>

#ifndef PIO_UNIT_TESTING
#define PIO_UNIT_TESTING
#endif

#include "GPAPMessage.h"

#include "MockPrint.h"

class VectorMockPrint : public MockPrint
{
private:
    std::vector<uint8_t> contents;

public:
    std::size_t write(uint8_t character) override
    {
        this->contents.push_back(character);
        return 1;
    }
};

void test_alarm_message()
{
    using namespace gpap_message;

    static const char *alarmMessageStr = "a5Test message[444]{a4ab}";
    static const char *alarmContentStr = "Test Message";
    static const char *typeDesignatorStr = "444";
    static const char *messageIdStr = "a4ab";

    const auto gpapMessage = GPAPMessage::deserialize(alarmMessageStr, std::strlen(alarmMessageStr));

    TEST_ASSERT_EQUAL(MessageType::ALARM, gpapMessage.getMessageType());

    auto vectorMockPrint = VectorMockPrint();

    const alarm::AlarmMessage &alarmMessage = gpapMessage.getAlarmMessage();
    auto alarmContentBytesWritten = alarmMessage.getAlarmContent().printTo(vectorMockPrint);

    TEST_ASSERT_EQUAL(std::strlen(alarmContentStr) + 1, alarmContentBytesWritten);
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_alarm_message);

    UNITY_END();
}
