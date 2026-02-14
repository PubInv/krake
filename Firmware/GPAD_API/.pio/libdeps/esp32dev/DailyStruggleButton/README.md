# DailyStruggleButton

DailyStruggleButton is yet another Arduino library to debounce button (push tactile switch) and manage its events. Events supported include pressing, releasing, holding down, long pressing (hold button for x time) and multi-hitting (hit button x times in y  time). For simplicity, there is only one callback function for all events, that passes an identifier as a parameter to indicate the event that just happened. 

![DailyButtonSketch](extras/DailyStruggleButtonSketch.jpg)

Original [meme](https://jake-clark.tumblr.com/post/100946716432) by Jake Clark.



# Updates
- 0.5.1
    - Fixed spelling mistakes.
    - Example file to use <> instead of "".
    - Changed versioning.
- 0.5.0
    - First upload.

# Button events

For every instance of Dailybutton, there is only one callback function. A callback function is a user defined function that DailyStruggleButton will call when a button event occurs. This callback function takes a parameter _buttonEvent_ of _byte_ datatype.  It is essentially positive whole numbers from 0 to 255.

_buttonEvent_ is updated before the function is being called back. Thus, you can use `switch` /`case` or `if`/`else` to dictate what to do in each situation.

Numerical Equivalent | Definition | Explanation
:--------------------|:-----------|:-----------
0 | `neverPressed`|The button is never pressed. This is used internally only.
1 | `onPress` | The button is pushed.
2 | `onRelease` | The button is let go.
3 | `onHold` | The button is held down. Be careful as this will keep calling back multiple times until the button is let go.
4 | `onLongPress` | The button was held down for a `longPressTime` length of time.
5 | `onMultiHit` | The button was pushed and let go for `onMultiHitTarget` number of times within `onMultiHit` milliseconds.

Here is an example:
```
#include <DailyStruggleButton>

DailyStruggleButton myButton;

void setup(){
	myButton.set(0, myCallback);
}
void loop(){
	myButton.poll();
}
void myCallback(byte buttonEvent){
	switch (buttonEvent){
		case onPress:
			// Do something...
			break;
		case onRelease:
			// Do something...
			break;
	}
}
```


# Setting up for the example code

To set up the example code, you need a Arduino (we tested it on Pro Micro but it should work on most common ones too) and a push tactile button.

One end of the button is connected to Pin 4, while the other is connected to ground. 

The internal pull-up resistor will ensure Pin 4 does not float (change between `HIGH` and `LOW` sporadically}, rather stay on `HIGH` (hence pull-**up**) unless it is deliberately connected to ground. When it is connected to ground, it becomes `LOW`, thus the logic is inverted, where `LOW` is pressed and `HIGH` is released. You need not be bothered with the inverted logic as DailyStruggleButton will take care of it, as long as you set the pull-up/down right in `set()`, which defaults to internal pulled up if not specified.



![Example Circuit](extras/Example%20Circuit_bb.svg)

# Public Functions
## DailyStruggleButton()

Constructor, not used. 



##  _void_ set( _byte_ myPin, _void_ (*myCallback)(_byte_ buttonEvent) )

Initialises the button. _myPin_ sets the digital pin the button is connected to. _myCallback_ is a callback function that the instance will call when an event occur. This event will be pass as _buttonEvent_. 
```
...
void setup(){
	myButton.set(0, callback);
}
...
callback(byte myEvent){
...
} 
```



##  _void_ set( _byte_ myPin, _void_ (*myCallback)(_byte_ buttonEvent), _byte_ myPull )

This is similar to the other _set()_ function, but allows for a third parameter to set the pull-up/down of the button. You can use one of the following:

`INT_PULL_UP`
Uses the internal pull-up resistor, most common use case, selected by default is this parameter is not entered.    

`EXT_PULL_UP`
You are to use your own external pull-up resistor, internal one will not be used.

`EXT_PULL_DOWN`
You are to use your own external pull-down resistor, internal one will not be used.

Internal pull-down is not supported. It is important to select the right pull-up/down as the library will decided if the logic should be inverted. 



## _bool_ poll()

Leave this function inside _loop()_ so it checks on the button constantly. 
Returns 1 if polled successfully. Returns 0 if button debounce failed.
```
void loop(){
	myButton.poll();
	...
}
```


## _void_ setDebounceTime(_unsigned int_ myTime)

To debounce a button in software is to read the state of the button, wait for a while and make sure the button state remains during this time. This is to filter out mechanical bouncing in push tactile switches. 

_myTime_ sets the waiting time in milliseconds. By default 20ms is used and usually there is no need to call this function. 



## _void_ enableLongPress(_unsigned int_ myTime)
Turn on long press detection. You need to hold the button down for _myTime_ in milliseconds for the event to register.



## _void_ disableLongPress()
Turn off long press detection.



## _void_ enableMultiHit(_unsigned int_ myTime, _byte_ myTarget)
Turn on multi-hit detection. You need to press the button_myTarget_ number of times within _myTime_ in milliseconds for the event to register.



## _void_ disableMultiHit()
Turn off multi-hit detection.



# Flowchart
The following shows the flow chart that explains the internal workings should you be interested. Note that not all variable names in the source code correspond to the variable names in the flowchart.

![Flow Chart Page 1](extras/DailyStruggleButton.svg)

