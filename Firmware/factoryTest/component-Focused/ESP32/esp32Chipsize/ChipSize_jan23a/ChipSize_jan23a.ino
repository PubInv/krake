void setup() {
    Serial.begin(115200);
    while (!Serial);

    // Print flash size
    Serial.println("Flash Size:");
    Serial.println(ESP.getFlashChipSize() / (1024 * 1024));
}

void loop() {
    // Do nothing
}