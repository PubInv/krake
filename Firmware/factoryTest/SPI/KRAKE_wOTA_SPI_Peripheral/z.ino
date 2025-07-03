void setup() {

  // pinMode(SW1, INPUT);  // sets the digital pin 36 as input
  // pinMode(SW2, INPUT);  // sets the digital pin 39 as input
  // pinMode(SW3, INPUT);  // sets the digital pin 34 as input
  // pinMode(SW4, INPUT);  // sets the digital pin 35 as input

  //setting GPIO to initial values
  pinMode(WiFiLed, OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);     // set the LED pin mode
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  pinMode(LED_1, OUTPUT);           // set the LED pin mode
  digitalWrite(LED_1, HIGH);        // turn the LED on (HIGH is the voltage level)
  pinMode(LED_2, OUTPUT);           // set the LED pin mode
  digitalWrite(LED_2, HIGH);        // turn the LED on (HIGH is the voltage level)
  pinMode(LED_3, OUTPUT);           // set the LED pin mode
  digitalWrite(LED_3, HIGH);        // turn the LED on (HIGH is the voltage level)
  pinMode(LED_4, OUTPUT);           // set the LED pin mode
  digitalWrite(LED_4, HIGH);        // turn the LED on (HIGH is the voltage level)
  pinMode(LED_5, OUTPUT);           // set the LED pin mode
  digitalWrite(LED_5, HIGH);        // turn the LED on (HIGH is the voltage level)

  Serial.begin(BAUDRATE);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB
  }
  WiFiMan();
  // blink();
  splashserial();
  Wire.begin();
  // initOLED();
  // splashOLED();
  initDFP();
  splashDFPlayer();
  SPIsetup();
  LCD_setup();
  // More setup code here

  digitalWrite(LED_1, LOW);        //Make built in LED low at end of setup.
  digitalWrite(LED_2, LOW);        //Make built in LED low at end of setup.
  digitalWrite(LED_3, LOW);        //Make built in LED low at end of setup.
  digitalWrite(LED_4, LOW);        //Make built in LED low at end of setup.
  digitalWrite(LED_5, LOW);        //Make built in LED low at end of setup.
  digitalWrite(LED_BUILTIN, LOW);  //Make built in LED low at end of setup.

  //Elegant OTA Setup

  initWiFi();
  initLittleFS();


  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html", false, processor);
  });

  server.serveStatic("/", LittleFS, "/");

  // Route to control LEDs using parameters
  server.on("/control", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("led") && request->hasParam("state")) {
      int ledIndex = request->getParam("led")->value().toInt();
      String state = request->getParam("state")->value();

      if (ledIndex >= 1 && ledIndex <= LED_COUNT) {
        digitalWrite(LED_PINS[ledIndex - 1], (state == "on") ? HIGH : LOW);
        request->send(200, "text/plain", "OK");
      } else {
        request->send(400, "text/plain", "Invalid LED index");
      }
    } else {
      request->send(400, "text/plain", "Missing parameters");
    }
  });

  // // Initialize switch pins as inputs with pull-ups
  // for (int i = 0; i < SWITCH_COUNT; i++) {
  //   pinMode(SWITCH_PINS[i], INPUT_PULLUP);
  //   attachInterrupt(SWITCH_PINS[i], switchISR, FALLING);
  // }

  // Start server
  server.begin();
  ElegantOTA.begin(&server);  // Start ElegantOTA

  // End of ELegant OTA Setup
  Buttonsetup();
}
//end setup()

void loop() {
  // if (availableDFPLAYER) {
  //   if (!digitalRead(SW1)) {
  //     dfPlayer.play(1);
  //     Serial.print(digitalRead(SW1));
  //     Serial.println(", playing track 1");
  //   }
  //   if (!digitalRead(SW2)) {
  //     dfPlayer.play(2);
  //     Serial.print(digitalRead(SW2));
  //     Serial.println(", playing track 2");
  //   }
  //   if (!digitalRead(SW3)) {
  //     dfPlayer.play(3);
  //     Serial.print(digitalRead(SW3));
  //     Serial.println(", playing track 3");
  //   }
  //   if (!digitalRead(SW4)) {
  //     dfPlayer.play(5);
  //     Serial.print(digitalRead(SW4));
  //     Serial.println(", playing track 5");
  //   }
  // }
  // displayAlarm(); // check OLED i2cTest code and fix// OLED is not displaying the alarm message when the SW of emergency is trigerred

  // switchISR();
  wink();  // Heart beat aka activity indicator LED function.
           // led1.Update();
           // led2.Update();
           // led3.Update();
           // led4.Update();
           // led5.Update();

  // This is needed to poll the button constantly
  // myButton.poll();
  // SW1.poll();
  // SW2.poll();
  // SW3.poll();
  // SW4.poll();
  SPIloop();

  LCD_Splash();


}  //end loop()
