// Ref documewntation https://github.com/hideakitai/ESP32SPISlave
// Editted by LEE and NAgham on 2025 06 27


#include <ESP32SPISlave.h>
#include "helper.h"
#define CONFIG_IDF_TARGET_ESP32  //in file ESP32Slave.h the VSPI is defined if "CONFIG_IDF_TARGET_ESP32" is defined in the code

// #define VSPI
ESP32SPISlave slave;
#define SS 5
#define SCK 18
#define MOSI 23
#define MISO 19
static constexpr size_t BUFFER_SIZE = 8;
static constexpr size_t QUEUE_SIZE = 1;
uint8_t tx_buf[BUFFER_SIZE]{ 1, 2, 3, 4, 5, 6, 7, 8 };
uint8_t rx_buf[BUFFER_SIZE]{ 0, 0, 0, 0, 0, 0, 0, 0 };

void setup() {
  Serial.begin(115200);

  delay(2000);

  slave.setDataMode(SPI_MODE0);    // default: SPI_MODE0
  slave.setQueueSize(QUEUE_SIZE);  // default: 1

  // begin() after setting
  //   slave.begin();  // default: HSPI (please refer README for pin assignments)

  //   /// @brief initialize SPI with HSPI/FSPI/VSPI, sck, miso, mosi, and ss pins
  //   bool begin(uint8_t spi_bus, int sck, int miso, int mosi, int ss);

  slave.begin(VSPI, SCK, MISO, MOSI, SS);

  //   slave.begin();


  Serial.println("start spi slave");
}

void loop() {
  // initialize tx/rx buffers
  initializeBuffers(tx_buf, rx_buf, BUFFER_SIZE);

  // start and wait to complete one BIG transaction (same data will be received from slave)
  const size_t received_bytes = slave.transfer(tx_buf, rx_buf, BUFFER_SIZE);

  // verify and dump difference with received data
  if (verifyAndDumpDifference("slave", tx_buf, BUFFER_SIZE, "master", rx_buf, received_bytes)) {
    Serial.println("successfully received expected data from master");
  } else {
    Serial.println("unexpected difference found between master/slave data");
  }
}
