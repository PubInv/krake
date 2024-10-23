// Define the GPIO pins for CLK, DT, and SW
const int CLK = 26;   // Connected to the CLK pin of the encoder
const int DT = 14;    // Connected to the DT pin of the encoder
const int SW = 25;    // Connected to the SW (Switch) pin of the encoder

int currentStateCLK;
int previousStateCLK;
int encoderPos = 0;
bool isButtonPressed = false;

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);

  // Set CLK and DT as inputs
  pinMode(CLK, INPUT );
  pinMode(DT, INPUT );
  
  // Set SW (Switch) as input with internal pull-up resistor
  pinMode(SW, INPUT_PULLUP);
  
  // Read the initial state of CLK
  previousStateCLK = digitalRead(CLK);
}

void loop() {
  // Read the current state of the CLK pin
  currentStateCLK = digitalRead(CLK);

  // If the current state of CLK is different from the last state
  // then the encoder has been rotated
  if (currentStateCLK != previousStateCLK) {
    
    // If the DT state is different than the CLK state, the encoder
    // is rotating clockwise, otherwise it's counterclockwise
    if (digitalRead(DT) != currentStateCLK) {
      encoderPos++;
      Serial.println("Rotating clockwise");
    } else {
      encoderPos--;
      Serial.println("Rotating counterclockwise");
    }

    Serial.print("Encoder position: ");
    Serial.println(encoderPos);
  }

  // Update the previous state of the CLK with the current state
  previousStateCLK = currentStateCLK;

  // Check if the switch is pressed
  if (digitalRead(SW) == LOW) {
    if (!isButtonPressed) {
      isButtonPressed = true;
      Serial.println("Button Pressed");
    }
  } else {
    if (isButtonPressed) {
      isButtonPressed = false;
      Serial.println("Button Released");
    }
  }

  // Small delay to debounce the switch and encoder
  delay(10);
}
