#include "DailyStruggleButton.h"

// Empty Constructor, we will use setup() instead;
DailyStruggleButton::DailyStruggleButton(){ }
  


// We use this set() inside setup() instead of a constructor so users don't have to declare the callback function
void DailyStruggleButton::set(byte myPin, void (*myCallback)(byte buttonEvent), byte myPull){
  pin=myPin;
  buttonPull=myPull; 
  
  switch (buttonPull){

    case EXT_PULL_UP:
      pinMode(pin, INPUT);
      flags.inverted=true;
      break;
      
    case EXT_PULL_DOWN:
      pinMode(pin, INPUT);
      flags.inverted=false;
      break;

      // Default to internal pull-up if user enters something weird
     default:
      pinMode(pin, INPUT_PULLUP);
      flags.inverted=true;
      break;
  }

  // Inverted is the default
  if (flags.inverted==true){
    flags.currentRawState = RELEASED;
    flags.lastRawState = RELEASED;
    flags.currentState = RELEASED;
    flags.lastState = RELEASED;
  }

  else{
    flags.currentRawState = PRESSED;
    flags.lastRawState = PRESSED;
    flags.currentState = PRESSED;
    flags.lastState = PRESSED;
  }

  flags.enableLongPress = false;
  flags.wasLongPressed = false;
  flags.enableMultiHit = false;

  callback = myCallback;
  
}



// Same as the other setup, but without and declaration of pull-up or down
// and default to internal pull-up.
void DailyStruggleButton::set(byte myPin, void (*myCallback)(byte buttonEvent)){
  set(myPin, myCallback, INT_PULL_UP);  
}



// Manually set the debounce time
void DailyStruggleButton::setDebounceTime(unsigned int myTime){
  debounceTime = myTime;
}



void DailyStruggleButton::enableLongPress(unsigned int myTime){
  longPressTime = myTime;
  flags.enableLongPress = true;
}



void DailyStruggleButton::disableLongPress(){
  flags.enableLongPress=false;
}



void DailyStruggleButton::enableMultiHit(unsigned int myTime, byte myTarget){
  multiHitTime = myTime;
  multiHitTarget = myTarget;
  flags.enableMultiHit = true;
}



void DailyStruggleButton::disableMultiHit(){
  flags.enableMultiHit= false;
}


bool DailyStruggleButton::debounce(){ //1FUNC

  flags.currentRawState = digitalRead(pin);

  // if not inverted, we make it inverted on purpose
  // becase the whole algo is based on inverted button logic.
  if (flags.inverted == false){ !flags.currentRawState; } 

  // If the button state did not change within debounce time,
  // then we considered it stable
  if (flags.currentRawState == flags.lastRawState){
    if ( (unsigned long)(millis()-lastChangeTime) >= debounceTime ){
      flags.currentState = flags.currentRawState;
      return 1;
    }
    else{ return 0; }
  }

  // However if it keeps changing, we keep resetting the debounce timer
  else{
    lastChangeTime = millis();
    flags.lastRawState = flags.currentRawState;
    return 0;
  }
 
}



bool DailyStruggleButton::poll(){ //1FUNC
  // Straight away end poll if debounce fails
  if (debounce()==0){ return 0; }

  /*======================================*/
  /* FIRST Scenario,PRESSED after PRESSED */
  /*======================================*/
  if (flags.currentState == PRESSED && flags.lastState == PRESSED){ //2I

    // Trigger onHold Event
    buttonEvent=onHold;
    callback(buttonEvent);

    // enableLongPress 1st Check: Is it enabled?
    if (flags.enableLongPress){ //3I
      
      // enableLongPress 2nd Check: Was the button held down long enough?
      if ( (unsigned long)(millis()-longPressSince) >= longPressTime ){ //4I

        // Trigger enableLongPress event
        buttonEvent=onLongPress;
        callback(buttonEvent);

        //reset timing for the next onLongPress
        longPressSince=millis(); 
        
        // You need this so the next onRelease will not trigger when user let go of the button
        flags.wasLongPressed=true; 
        
      } //4I
      
    } //3I

  } //2I
  
  /*=========================================*/
  /* SECOND Scenario, PRESSED after RELEASED */
  /*=========================================*/
  else if (flags.currentState == PRESSED && flags.lastState == RELEASED){ //2EI

    //Trigger onPress event
    longPressSince=millis(); // Reset since it was previously released
    buttonEvent=onPress;
    callback(buttonEvent); 
         
  } //2EI

  /*========================================*/
  /* THIRD Scenario, RELEASED after PRESSED */
  /*========================================*/
  else if (flags.currentState == RELEASED && flags.lastState == PRESSED){ //2EI
    
    if (flags.wasLongPressed == true){ //3I
      flags.wasLongPressed = false;
      // Do nothing here as we do not want to register 
      // the onRelease right after a longPressFor
    } //3I

    else{ //3E
      
      // Trigger onRelease event
      buttonEvent=onRelease;
      callback(buttonEvent); 

      // onMultiHit 1st check: Is it enabled?
      if (flags.enableMultiHit){ //4I
        // Register the hit
        multiHitCount++; 
        
        // onMultiHit 2nd check: Is it >1 hit? A multi-release seq needs >1 hits.
        if (multiHitCount == 1){//5I
          //Start timer if this is the first hit
          multiHitSince=millis();
        }//5I

        else{ //5E
          // onMultiHit 3rd check: Is it within time limit?
          if ( (unsigned long)(millis()-multiHitSince) <= multiHitTime ){//6I

            // onMultiHit 4th check: Did the user press enough times?
            if (multiHitCount >= multiHitTarget){//7I
              buttonEvent = onMultiHit;
              callback(buttonEvent);
              multiHitCount=0;
            }//7I
            
          }//6I

          // Reset to 1 and reset time if time limit is over
          else {//6E 
            multiHitCount=1;
            multiHitSince=millis();
          }//6E
          
        }//5E

      } //4I
      
    } //3E
    
  } //2EI

  // Record the current button state
  flags.lastState=flags.currentState; 
  return 1;
  
}// 1FUNC
