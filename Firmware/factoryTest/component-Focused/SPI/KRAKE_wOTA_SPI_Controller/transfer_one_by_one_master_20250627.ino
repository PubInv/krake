#include <SPI.h>
#include "helper.h"

SPIClass master(VSPI);
#ifdef CONFIG_IDF_TARGET_ESP32
static constexpr uint8_t PIN_SS = 15;
#else
static constexpr uint8_t PIN_SS = SS;
#endif

static constexpr size_t BUFFER_SIZE = 8;
uint8_t tx_buf[BUFFER_SIZE] {1, 2, 3, 4, 5, 6, 7, 8};
uint8_t rx_buf[BUFFER_SIZE] {0, 0, 0, 0, 0, 0, 0, 0};
const int LED04 = 25;

void SPIsetup()
{
    pinMode(LED04, OUTPUT);
    pinMode(PIN_SS, OUTPUT);
    digitalWrite(PIN_SS, HIGH);
    master.begin(SCK, MISO, MOSI, PIN_SS);

    delay(2000);

    Serial.println("start spi master");
}

void SPIloop()
{
    // initialize tx/rx buffers
    initializeBuffers(tx_buf, rx_buf, BUFFER_SIZE);

    master.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    digitalWrite(PIN_SS, LOW);
    master.transferBytes(tx_buf, rx_buf, BUFFER_SIZE);
    digitalWrite(PIN_SS, HIGH);
    master.endTransaction();

    // verify and dump difference with received data
    if (verifyAndDumpDifference("master", tx_buf, BUFFER_SIZE, "slave", rx_buf, BUFFER_SIZE)) {
        Serial.println("successfully received expected data from slave");
        digitalWrite(LED04, HIGH);
    } else {
        Serial.println("unexpected difference found between master/slave data");
        digitalWrite(LED04, LOW);
    }

    delay(2000);
}
