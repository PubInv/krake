/*********
here we added Serial Prints to detect the bug 
and on the hardware I tried to fix the contrast of the LCD by tilting the nob in the back of the chip. 
*********/

#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// set the LCD number of columns and rows
int lcdColumns = 20;
int lcdRows = 4;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x3F, lcdColumns, lcdRows);  

void setup(){
 
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nI2C Scanner");
  Wire.begin();
  // initialize LCD
  lcd.init();
  Serial.print("LCD init success");
  // turn on LCD backlight                      
  // lcd.backlight();
  // Serial.print("Did you fail yet?");
}

void loop(){
  // set cursor to first column, first row
  lcd.setCursor(1, 1);
    Serial.println("Hello, World!");
  // print message
  lcd.print("Hello, World!");
  
  delay(1000);
  // clears the display to print new message
  // lcd.clear();
  // set cursor to first column, second row
  lcd.setCursor(2,2);
  Serial.print("Hello, World! AGAIN!\n");
  lcd.print("Hello, World!");

  delay(1000);
  // lcd.clear(); 
}