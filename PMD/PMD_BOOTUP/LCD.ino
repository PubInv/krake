// #include <LiquidCrystal_I2C.h>
// #include <Wire.h>

// set the LCD number of columns and rows
// int lcdColumns = 20;
// int lcdRows = 4;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch


//LiquidCrystal_I2C lcd(LCD_ADDRESS, 20, 4);
// LiquidCrystal_I2C lcd(0x3F, lcdColumns, lcdRows);

void LCD_setup() {
  lcd.init();  // initialize the lcd
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0, 0);
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
  // delay(5000);
  lcd.setCursor(0, 2);  //set cursor (culomn,raw)
  // lcd.print(MODEL_NAME);
  // lcd.print(DEVICE_UNDER_TEST);
  lcd.print(F("Compiled at: "));
  lcd.setCursor(0, 3);  //set cursor (culomn,raw)
  lcd.print(F(__DATE__ "" __TIME__));  //compile date that is used for a unique identifier
  // lcd.print(LICENSE);
}