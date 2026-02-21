/*
 * Balanced RGB Control with Rotary Encoder
 * Features: Debouncing, Adjustable Sensitivity, and Reset Button
 */

// Encoder Pins
const int clkPin = 3;
const int dtPin = 4;
const int swPin = 2;

// RGB Module Pins
const int redPin = 6;
const int greenPin = 5;
const int bluePin = 9;

// Variables for logic
int counter = 0;
int lastStateCLK;
unsigned long lastTurnTime = 0;    // For rotation debouncing
unsigned long lastButtonPress = 0; // For button debouncing

// --- ADJUST THESE ---
const int sensitivity = 8;     // Lower = more precise; Higher = faster
const bool commonAnode = false; // Set to true if LED logic is reversed
// --------------------

void setup() {
  pinMode(clkPin, INPUT);
  pinMode(dtPin, INPUT);
  pinMode(swPin, INPUT_PULLUP);
  
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  Serial.begin(9600);
  lastStateCLK = digitalRead(clkPin);
  
  updateColor(counter);
  Serial.println("System Ready. Sensitivity set to 8.");
}

void loop() {
  // 1. READ ROTATION
  int currentStateCLK = digitalRead(clkPin);

  // Check if knob moved
  if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {
    
    // Software Debounce: Ignore pulses faster than 5 milliseconds
    if (millis() - lastTurnTime > 5) {
      
      if (digitalRead(dtPin) != currentStateCLK) {
        counter += sensitivity; // Clockwise
      } else {
        counter -= sensitivity; // Counter-clockwise
      }

      counter = constrain(counter, 0, 255);
      
      Serial.print("Color Value: ");
      Serial.println(counter);
      updateColor(counter);
      
      lastTurnTime = millis(); // Update debounce timer
    }
  }
  lastStateCLK = currentStateCLK;

  // 2. READ BUTTON PRESS
  // Only triggers if button is pressed AND 200ms has passed since last press
  if (digitalRead(swPin) == LOW && (millis() - lastButtonPress > 200)) {
    Serial.println("Resetting Color...");
    counter = 0;
    updateColor(counter);
    lastButtonPress = millis();
  }
}

void updateColor(int val) {
  int r = val;           // Red increases with counter
  int b = 255 - val;     // Blue decreases with counter
  int g = 0;             // Keep Green off for a clean Red-to-Blue sweep

  if (commonAnode) {
    analogWrite(redPin, 255 - r);
    analogWrite(greenPin, 255 - g);
    analogWrite(bluePin, 255 - b);
  } else {
    analogWrite(redPin, r);
    analogWrite(greenPin, g);
    analogWrite(bluePin, b);
  }
}
