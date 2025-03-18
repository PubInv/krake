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


void I2cScanner(void) {
  Wire.begin();
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
      nDevices++;
    } else if (false) {
      //  else if (error == 4) {
      Serial.print("Unknow error at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  } else {
    Serial.println("done\n");
  }
  Serial.print("Number of Devices found: ");
  Serial.println(nDevices);

}
