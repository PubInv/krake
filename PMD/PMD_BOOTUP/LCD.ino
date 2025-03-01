// #include <LiquidCrystal_I2C.h>
// #include <Wire.h>

// set the LCD number of columns and rows
// int lcdColumns = 20;
// int lcdRows = 4;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch


//LiquidCrystal_I2C lcd(LCD_ADDRESS, 20, 4);
// LiquidCrystal_I2C lcd(0x3F, lcdColumns, lcdRows);  

void LCD_setup()
{
  lcd.init();                      // initialize the lcd 
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Hello, world!");
  lcd.setCursor(2,1);
  lcd.print("Ywrobot Arduino!");
   lcd.setCursor(0,2);
  lcd.print("Arduino LCM IIC 2004");
   lcd.setCursor(2,3);
  lcd.print("Power By Ec-yuan!");
}