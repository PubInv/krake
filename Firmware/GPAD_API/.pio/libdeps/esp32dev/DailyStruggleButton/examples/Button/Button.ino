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

void setup(){
  Serial.begin(9600);

  // Use set(digital pin connected to button, a callback function, type of pull-up/down) to initialise the button
  // Choose between INT_PULL_UP, EXT_PULL_UP and EXT_PULL_DOWN
  myButton.set(4, buttonEvent, INT_PULL_UP);

  // You can enable long press to use this feature
  myButton.enableLongPress(longPressTime);

  // You can enable multi-hit to use this feature
  myButton.enableMultiHit(multiHitTime, multiHitTarget);
}

void loop(){
  // This is needed to poll the button constantly
  myButton.poll();
}

// This function will be called whenever an event occurs.
// We pass the name of this callback function in set().
// It needs to take a parameter of the byte datatype.
// This byte will indicate the event.
// It needs to return void.
void buttonEvent(byte btnStatus){

  // We can use switch/case to run through what happens for each event
  switch (btnStatus){

    // onPress is indicated when the button is pushed down
    case onPress:
      Serial.println("Button Pressed");
      break;

    // onRelease is indicated when the button is let go
    case onRelease:
      Serial.println("Button Released");
      break;

    // onHold is indicated whenever the button is held down.
    // It can be annoying so we will comment this out in this example.
//  case onHold:
//    Serial.println("Button Long Pressed");
//    break;

    // onLongPress is indidcated when you hold onto the button 
    // more than longPressTime in milliseconds
    case onLongPress:
      Serial.print("Buttong Long Pressed For ");
      Serial.print(longPressTime);
      Serial.println("ms");
      break;

    // onMultiHit is indicated when you hit the button
    // multiHitTarget times within multihitTime in milliseconds
    case onMultiHit:
      Serial.print("Button Pressed ");
      Serial.print(multiHitTarget);
      Serial.print(" times in ");
      Serial.print(multiHitTime);
      Serial.println("ms");
      break;
  }
}
