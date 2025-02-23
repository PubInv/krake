// Author: Nagham Kheir
// Date: 20250223
// LICENSE "GNU Affero General Public License, version 3 "


byte I2C_address = 0x3F;



//functions defined below
void testI2cAdress(void) {
  Wire.begin();
  Wire.beginTransmission(I2C_address);
  byte error;
  error = Wire.endTransmission();

  if (error == 0) {
    Serial.print("I2C device found at address 0x");
    if (I2C_address < 16) {
      Serial.print("0");
    }
    Serial.println(I2C_address, HEX);
  } else {
    Serial.print("I2C device not found");
  }
}