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
    std::vector<char> contents;

public:
    std::size_t write(uint8_t character) override
    {
        this->contents.push_back(static_cast<char>(character));
        this->contents.shrink_to_fit();
        return 1;
    }

    const char *data()
    {
        return this->contents.data();
    }

    void clear()
    {
        this->contents.clear();
    }
};

struct AlarmMessageData final
{
    const gpap_message::alarm::AlarmMessage::Level alarmLevel;
    const std::string alarmContentStr;
    const std::string typeDesignatorStr;
    const std::string messageIdStr;

    AlarmMessageData(
        const gpap_message::alarm::AlarmMessage::Level alarmLevel,
        const std::string alarmContentStr,
        const std::string typeDesignatorStr,
        const std::string messageIdStr)
        : alarmLevel(alarmLevel),
          alarmContentStr(alarmContentStr),
          typeDesignatorStr(typeDesignatorStr),
          messageIdStr(messageIdStr) {}
};

void alarm_test_helper(const AlarmMessageData &alarmMessageData, const std::string &fullMessageString)
{
    using namespace gpap_message;

    const auto gpapMessage = GPAPMessage::deserialize(fullMessageString.c_str(), fullMessageString.length());
    const alarm::AlarmMessage &alarmMessage = gpapMessage.getAlarmMessage();

    TEST_ASSERT_EQUAL(MessageType::ALARM, gpapMessage.getMessageType());
    TEST_ASSERT_EQUAL(alarmMessageData.alarmLevel, alarmMessage.getAlarmLevel());

    auto printedData = VectorMockPrint();
    auto alarmContentBytesWritten = alarmMessage.getAlarmContent().printTo(printedData);

    // Have to add an additional 1 since AlarmContent handles printing the null terminator as well
    TEST_ASSERT_EQUAL(alarmMessageData.alarmContentStr.size() + 1, alarmContentBytesWritten);
    TEST_ASSERT_EQUAL(strcmp(alarmMessageData.alarmContentStr.c_str(), printedData.data()), 0);
    printedData.clear();

    if (alarmMessage.getTypeDesignator().state == alarm::PossibleParameter<alarm::AlarmTypeDesignator>::State::Some)
    {
        auto typeDesignatorBytesWritten = alarmMessage.getTypeDesignator().contents.printTo(printedData);
        // Have to add an additional 1 since AlarmContent handles printing the null terminator as well
        TEST_ASSERT_EQUAL(alarmMessageData.typeDesignatorStr.size() + 1, typeDesignatorBytesWritten);
        TEST_ASSERT_EQUAL(strcmp(alarmMessageData.typeDesignatorStr.c_str(), printedData.data()), 0);
        printedData.clear();
    }
    else
    {
        TEST_ASSERT_TRUE(alarmMessageData.typeDesignatorStr.empty());
    }

    if (alarmMessage.getMessageId().state == alarm::PossibleParameter<alarm::AlarmMessageId>::State::Some)
    {
        auto messageIdBytesWritten = alarmMessage.getMessageId().contents.printTo(printedData);
        TEST_ASSERT_EQUAL(alarmMessageData.messageIdStr.size() + 1, messageIdBytesWritten);
        TEST_ASSERT_EQUAL(strcmp(alarmMessageData.messageIdStr.c_str(), printedData.data()), 0);
    }
    else
    {
        TEST_ASSERT_TRUE(alarmMessageData.messageIdStr.empty());
    }
}

void test_alarm_message_order_content_designator_id()
{
    static const std::string alarmMessageStr = "a5Test message[444]{a4ab}";
    static const std::string alarmContentStr = "Test message";
    static const std::string typeDesignatorStr = "444";
    static const std::string messageIdStr = "a4ab";

    const auto alarmData = AlarmMessageData(
        gpap_message::alarm::AlarmMessage::Level::Level5,
        alarmContentStr,
        typeDesignatorStr,
        messageIdStr);

    alarm_test_helper(alarmData, alarmMessageStr);
}

void test_alarm_message_order_designator_content_id()
{
    static const std::string alarmMessageStr = "a2[432]Testing more messages{abcdef}";
    static const std::string alarmContentStr = "Testing more messages";
    static const std::string typeDesignatorStr = "432";
    static const std::string messageIdStr = "abcdef";

    const auto alarmData = AlarmMessageData(
        gpap_message::alarm::AlarmMessage::Level::Level2,
        alarmContentStr,
        typeDesignatorStr,
        messageIdStr);

    alarm_test_helper(alarmData, alarmMessageStr);
}

void test_alarm_message_only_content()
{
    static const std::string alarmMessageStr = "a0Content only alarm";
    static const std::string alarmContentStr = "Content only alarm";
    static const std::string typeDesignatorStr = "";
    static const std::string messageIdStr = "";

    const auto alarmData = AlarmMessageData(
        gpap_message::alarm::AlarmMessage::Level::Level0,
        alarmContentStr,
        typeDesignatorStr,
        messageIdStr);

    alarm_test_helper(alarmData, alarmMessageStr);
}

void test_alarm_message_only_id()
{
    static const std::string alarmMessageStr = "a1{98765}";
    static const std::string alarmContentStr = "";
    static const std::string typeDesignatorStr = "";
    static const std::string messageIdStr = "98765";

    const auto alarmData = AlarmMessageData(
        gpap_message::alarm::AlarmMessage::Level::Level1,
        alarmContentStr,
        typeDesignatorStr,
        messageIdStr);

    alarm_test_helper(alarmData, alarmMessageStr);
}

void test_alarm_message_only_type_designator()
{
    static const std::string alarmMessageStr = "a3[000]";
    static const std::string alarmContentStr = "";
    static const std::string typeDesignatorStr = "000";
    static const std::string messageIdStr = "";

    const auto alarmData = AlarmMessageData(
        gpap_message::alarm::AlarmMessage::Level::Level3,
        alarmContentStr,
        typeDesignatorStr,
        messageIdStr);

    alarm_test_helper(alarmData, alarmMessageStr);
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_alarm_message_order_content_designator_id);
    RUN_TEST(test_alarm_message_order_designator_content_id);
    RUN_TEST(test_alarm_message_only_content);
    RUN_TEST(test_alarm_message_only_id);
    RUN_TEST(test_alarm_message_only_type_designator);

    UNITY_END();
}
