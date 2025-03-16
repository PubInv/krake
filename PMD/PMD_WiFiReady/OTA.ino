//// Upload and run  
// The device will appear in your Wi-Fi discoverable devices with the name "ESP32-Setup".  
// Connect to it.  
// Copy the IP address from the Serial Monitor.  
// Paste the IP address and set up your Wi-Fi local network.  




#include <WiFiManager.h>   // WiFi Manager for ESP32
#include <ArduinoOTA.h>    // OTA library

#define LED_PIN 23          // Built-in LED on ESP32

void OTA() {
    pinMode(LED_PIN, OUTPUT);

    // WiFiManager to handle WiFi setup
    WiFiManager wifiManager;
    if (!wifiManager.autoConnect("ESP32-Setup")) {
        Serial.println("Failed to connect. Restarting...");
        ESP.restart();
    }
    Serial.println("Connected to WiFi!");

    // OTA Setup
    ArduinoOTA.setHostname("ESP32-OTA");
    ArduinoOTA.begin();
    
    Serial.println("OTA Ready!");
}
// this blinking is effecting the rate of LEDs blinking 
void blink() {
    ArduinoOTA.handle();  // Handle OTA updates

    // Blink LED to show ESP32 is running
    digitalWrite(LED_PIN, HIGH);
   
}
