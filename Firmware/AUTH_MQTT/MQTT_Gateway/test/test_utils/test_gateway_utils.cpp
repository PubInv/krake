#include <unity.h>
#include "gateway_utils.h"

void setUp(void) {
}

void tearDown(void) {
}

void test_hex_to_bytes() {
    const char* hex = "25";
    uint8_t result = 0;
    gw_hex_to_bytes(hex, &result,sizeof(hex));
    TEST_ASSERT_EQUAL_INT(0x25, result);
}
void test_hex_to_bytes_30() {
    const char* hex = "30";
    uint8_t result = 0;
    gw_hex_to_bytes(hex, &result,sizeof(hex));
    TEST_ASSERT_EQUAL_INT(0x30, result);
}

int main(void) {
    UNITY_BEGIN();
 //run test
    RUN_TEST(test_hex_to_bytes);
    RUN_TEST(test_hex_to_bytes_30);
    UNITY_END();
    return 0;
}