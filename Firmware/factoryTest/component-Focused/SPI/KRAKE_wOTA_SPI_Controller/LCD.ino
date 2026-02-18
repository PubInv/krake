#include <LiquidCrystal_I2C.h>
#include <Wire.h>

int lcdColumns = 20;
int lcdRows = 4;
uint8_t foundAddress = 0x00;

// Declare a pointer, will be assigned after detecting address
LiquidCrystal_I2C* lcd;

void LCD_setup() {
  Wire.begin();
  Serial.begin(115200);
  Serial.println("Scanning for I2C devices...");

  for (uint8_t address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.print("I2C device found at 0x");
      Serial.println(address, HEX);
      foundAddress = address;
      break;
    }
  }

  if (foundAddress == 0) {
    Serial.println("No I2C LCD found.");
    return; // exit setup
  }

  // Dynamically create the LCD object
  lcd = new LiquidCrystal_I2C(foundAddress, lcdColumns, lcdRows);
  lcd->init();
  lcd->backlight();
  lcd->setCursor(0, 0);
  lcd->print("LCD Ready");
}

void LCD_Splash(void) {
  if (!lcd) return; // check if lcd was initialized

  lcd->clear();
  lcd->setCursor(0, 0);
  lcd->print(PROG_NAME);
  lcd->print(VERSION);

  IPAddress myIP = WiFi.localIP();
  lcd->setCursor(0, 1);
  lcd->print(myIP);

  lcd->setCursor(0, 2);
  lcd->print(MODEL_NAME);
  lcd->print(DEVICE_UNDER_TEST);

  lcd->setCursor(0, 3);
  lcd->print(F(__DATE__ " " __TIME__));
}
