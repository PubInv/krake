#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// set the LCD number of columns and rows
int lcdColumns = 20;
int lcdRows = 4;
uint8_t foundAddress = 0x00;
// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch


// LiquidCrystal_I2C lcd(LCD_ADDRESS, 20, 4);
LiquidCrystal_I2C lcd(foundAddress, lcdColumns, lcdRows);

void LCD_setup() {
  Wire.begin();
  lcd.init();  // initialize the lcd
  // Print a message to the LCD.
  lcd.backlight();


// scannin for ip address

  for (uint8_t address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.print("I2C device found at 0x");
      Serial.println(address, HEX);
      foundAddress = address;
      break;  // Optional: stop at first device found
    }
  }

  if (foundAddress == 0) {
    Serial.println("No I2C devices found.");
  } else {
   LiquidCrystal_I2C lcd(foundAddress, lcdColumns, lcdRows);
  }



  // lcd.setCursor(0, 0);
  // lcd.print("Hello, Public Invention!qwertyuiopasdfghjklzxcvbnmqwertyuiopasdfghjklzxcvbnmqwertyuiopasdfghjklzxcvbnmqwer");
  // lcd.print("Hello, Public Invention!qwertyuiopasdfghjklzxcvbnmqwertyuiopasdfghjklzxcvbnmqwertyuiopasdfghjklzxcvbnmqwer0123456789");
  // lcd.print("wwwwwwwww/xxxxxxxxx/yyyyyyyyy/zzzzzzzzz/wwwwwwwww/xxxxxxxxx/yyyyyyyyy/zzzzzzzzz/");
  // lcd.print("The quick brown fox jumps over the lazy dog near the river at sunset today!testlcd");
  // lcd.print("The quick brown fox jumps over the lazy dog near the river at sunset today!test0");
  // delay(1000);
  // lcd.print("123456789");
}

void LCD_Splash(void) {
  // lcd.print("wwwwwwwww/xxxxxxxxx/yyyyyyyyy/zzzzzzzzz/wwwwwwwww/xxxxxxxxx/yyyyyyyyy/zzzzzzzzz/");
  lcd.clear();
  lcd.setCursor(0, 0);  // home cursor
  lcd.print(PROG_NAME);
  lcd.print(VERSION);
  IPAddress myIP = WiFi.localIP();
  lcd.setCursor(0, 1);
  lcd.print( myIP);
  // delay(5000);
  lcd.setCursor(0, 2);  //set cursor (culomn,raw)
  lcd.print(MODEL_NAME);
  lcd.print(DEVICE_UNDER_TEST);
  lcd.print(F("Compiled at: "));
  lcd.setCursor(0, 3);                 //set cursor (culomn,raw)
  lcd.print(F(__DATE__ "" __TIME__));  //compile date that is used for a unique identifier
  // lcd.print(LICENSE);
}