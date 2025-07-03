/*
DailyStruggleButton by cygig

DailyStruggleButton is yet another Arduino library to debounce button (push tactile switch) and manage its events. 
Events supported include pressing, releasing, holding down, long pressing (hold button for x time) and 
multi-hitting (hit button x times in y  time). For simplicity, there is only one callback function for all events, 
that passes an identifier as a parameter to indicate the event that just happened.
*/

#include <DailyStruggleButton.h>

// Time in ms you need to hold down the button to be considered a long press
unsigned int longPressTime = 1000;

// How many times you need to hit the button to be considered a multi-hit
byte multiHitTarget = 2;

// How fast you need to hit all buttons to be considered a multi-hit
unsigned int multiHitTime = 400;

// Create an instance of DailyStruggleButton
DailyStruggleButton myButton;
DailyStruggleButton SW1;
DailyStruggleButton SW2;
DailyStruggleButton SW3;
DailyStruggleButton SW4;



#define GPIO_SW1 36
#define GPIO_SW2 39
#define GPIO_SW3 34
#define GPIO_SW4 35
#define GPIO0 0

// // Customized this by changing these defines
// #define VERSION " V0.0.1 "
// #define MODEL_NAME "Model: bUTTOM"
// #define DEVICE_UNDER_TEST "SN: 00001"  //A Serial Number
// #define LICENSE "GNU Affero General Public License, version 3 "
// #define ORIGIN "LB"

// void splashserial(void) {
//   //Serial splash
//   Serial.println(F("==================================="));
//   Serial.println(VERSION);
//   Serial.println(MODEL_NAME);
//   Serial.println(DEVICE_UNDER_TEST);
//   Serial.print(F("Compiled at: "));
//   Serial.println(F(__DATE__ " " __TIME__));  //compile date that is used for a unique identifier
//   Serial.println(LICENSE);
//   Serial.println(F("==================================="));
//   Serial.println();
// }


void Buttonsetup() {


  // Use set(digital pin connected to button, a callback function, type of pull-up/down) to initialise the button
  // Choose between INT_PULL_UP, EXT_PULL_UP and EXT_PULL_DOWN
  myButton.set(GPIO0, buttonEvent, INT_PULL_UP);
  SW1.set(GPIO_SW1, morisCodeEvent, INT_PULL_UP);
  SW2.set(GPIO_SW2, RepeatCalculation, INT_PULL_UP);
  SW3.set(GPIO_SW3, muteFiveMin, INT_PULL_UP);
  SW4.set(GPIO_SW4, SendEmergMessage, INT_PULL_UP);


  // You can enable long press to use this feature
  myButton.enableLongPress(longPressTime);
  SW1.enableLongPress(longPressTime);
  SW2.enableLongPress(longPressTime);
  SW3.enableLongPress(longPressTime);
  SW4.enableLongPress(longPressTime);


  // You can enable multi-hit to use this feature
  SW1.enableMultiHit(multiHitTime, multiHitTarget);
  SW1.enableMultiHit(multiHitTime, multiHitTarget);
  SW2.enableMultiHit(multiHitTime, multiHitTarget);
  SW3.enableMultiHit(multiHitTime, multiHitTarget);
  SW4.enableMultiHit(multiHitTime, multiHitTarget);
}

// This function will be called whenever an event occurs.
// We pass the name of this callback function in set().
// It needs to take a parameter of the byte datatype.
// This byte will indicate the event.
// It needs to return void.
void buttonEvent(byte btnStatus) {

  // We can use switch/case to run through what happens for each event
  switch (btnStatus) {

    // onPress is indicated when the button is pushed down
    case onPress:
      Serial.println("BOOT Button Pressed");
      break;

    // onRelease is indicated when the button is let go
    case onRelease:
      //Serial.println("BOOT Button Released");
      break;

      // onHold is indicated whenever the button is held down.
      // It can be annoying so we will comment this out in this example.
      //  case onHold:
      //    Serial.println("Button Long Pressed");
      //    break;

    // onLongPress is indidcated when you hold onto the button
    // more than longPressTime in milliseconds
    case onLongPress:
      Serial.print("BOOT Buttong Long Pressed For ");
      Serial.print(longPressTime);
      Serial.println("ms");
      break;

    // onMultiHit is indicated when you hit the button
    // multiHitTarget times within multihitTime in milliseconds
    case onMultiHit:
      Serial.print("BOOT Button Pressed ");
      Serial.print(multiHitTarget);
      Serial.print(" times in ");
      Serial.print(multiHitTime);
      Serial.println("ms");
      break;
  }
}

// This function will be called whenever an event occurs.
// We pass the name of this callback function in set().
// It needs to take a parameter of the byte datatype.
// This byte will indicate the event.
// It needs to return void.
void morisCodeEvent(byte btnStatus) {

  // We can use switch/case to run through what happens for each event
  switch (btnStatus) {

    // onPress is indicated when the button is pushed down
    case onPress:
      Serial.println("SW1 Button Pressed");
      break;

    // onRelease is indicated when the button is let go
    case onRelease:
      //Serial.println("BOOT Button Released");
      break;

      // onHold is indicated whenever the button is held down.
      // It can be annoying so we will comment this out in this example.
      //  case onHold:
      //    Serial.println("Button Long Pressed");
      //    break;

    // onLongPress is indidcated when you hold onto the button
    // more than longPressTime in milliseconds
    case onLongPress:
      Serial.print("SW1 Buttong Long Pressed For ");
      Serial.print(longPressTime);
      Serial.println("ms");
      break;

    // onMultiHit is indicated when you hit the button
    // multiHitTarget times within multihitTime in milliseconds
    case onMultiHit:
      Serial.print("SW1 Button Pressed ");
      Serial.print(multiHitTarget);
      Serial.print(" times in ");
      Serial.print(multiHitTime);
      Serial.println("ms");
      break;
  }
}


void RepeatCalculation(byte btnStatus) {

  // We can use switch/case to run through what happens for each event
  switch (btnStatus) {

    // onPress is indicated when the button is pushed down
    case onPress:
      Serial.println("SW2 Button Pressed");
      break;

    // onRelease is indicated when the button is let go
    case onRelease:
      //Serial.println("BOOT Button Released");
      break;

      // onHold is indicated whenever the button is held down.
      // It can be annoying so we will comment this out in this example.
      //  case onHold:
      //    Serial.println("Button Long Pressed");
      //    break;

    // onLongPress is indidcated when you hold onto the button
    // more than longPressTime in milliseconds
    case onLongPress:
      Serial.print("SW2 Buttong Long Pressed For ");
      Serial.print(longPressTime);
      Serial.println("ms");
      break;

    // onMultiHit is indicated when you hit the button
    // multiHitTarget times within multihitTime in milliseconds
    case onMultiHit:
      Serial.print("SW2 Button Pressed ");
      Serial.print(multiHitTarget);
      Serial.print(" times in ");
      Serial.print(multiHitTime);
      Serial.println("ms");
      break;
  }
}

void muteFiveMin(byte btnStatus) {

  // We can use switch/case to run through what happens for each event
  switch (btnStatus) {

    // onPress is indicated when the button is pushed down
    case onPress:
      Serial.println("SW3 Button Pressed");
      break;

    // onRelease is indicated when the button is let go
    case onRelease:
      //Serial.println("BOOT Button Released");
      break;

      // onHold is indicated whenever the button is held down.
      // It can be annoying so we will comment this out in this example.
      //  case onHold:
      //    Serial.println("Button Long Pressed");
      //    break;

    // onLongPress is indidcated when you hold onto the button
    // more than longPressTime in milliseconds
    case onLongPress:
      Serial.print("SW3 Buttong Long Pressed For ");
      Serial.print(longPressTime);
      Serial.println("ms");
      break;

    // onMultiHit is indicated when you hit the button
    // multiHitTarget times within multihitTime in milliseconds
    case onMultiHit:
      Serial.print("SW3 Button Pressed ");
      Serial.print(multiHitTarget);
      Serial.print(" times in ");
      Serial.print(multiHitTime);
      Serial.println("ms");
      break;
  }
}

void SendEmergMessage(byte btnStatus) {

  // We can use switch/case to run through what happens for each event
  switch (btnStatus) {

    // onPress is indicated when the button is pushed down
    case onPress:
      Serial.println("SW4 Button Pressed");
      break;

    // onRelease is indicated when the button is let go
    case onRelease:
      //Serial.println("BOOT Button Released");
      break;

      // onHold is indicated whenever the button is held down.
      // It can be annoying so we will comment this out in this example.
      //  case onHold:
      //    Serial.println("Button Long Pressed");
      //    break;

    // onLongPress is indidcated when you hold onto the button
    // more than longPressTime in milliseconds
    case onLongPress:
      Serial.print("SW4 Buttong Long Pressed For ");
      Serial.print(longPressTime);
      Serial.println("ms");
      break;

    // onMultiHit is indicated when you hit the button
    // multiHitTarget times within multihitTime in milliseconds
    case onMultiHit:
      Serial.print("SW4 Button Pressed ");
      Serial.print(multiHitTarget);
      Serial.print(" times in ");
      Serial.print(multiHitTime);
      Serial.println("ms");
      break;
  }
}



// End of Functions

// void setup() {
//   Serial.begin(115200);

//   splashserial();
//   Buttonsetup();
// }

// void loop() {
//   // This is needed to poll the button constantly
//   myButton.poll();
//   SW1.poll();
//   SW2.poll();
//   SW3.poll();
//   SW4.poll();
// }


