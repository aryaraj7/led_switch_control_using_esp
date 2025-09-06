#include <Arduino.h>

// Structure to hold LED control data
struct LedControl {
  const int ledPin;         // LED pin
  const int switchPin;      // Switch pin
  unsigned long lastPressTime;
  int pressCount;
  bool ledOn;
  bool ledBlink;
  unsigned long blinkTimer;
  bool lastButtonState;
};

// Initialize structure with values
LedControl ledCtrl = {
  2,       // LED pin (GPIO2 onboard LED)
  32,       // Switch pin (GPIO4 external button)
  0,       // lastPressTime
  0,       // pressCount
  false,   // ledOn
  false,   // ledBlink
  0,       // blinkTimer
  HIGH     // lastButtonState (button not pressed)
};

void setup() {
  pinMode(ledCtrl.ledPin, OUTPUT);
  pinMode(ledCtrl.switchPin, INPUT_PULLUP); // Button active LOW
  digitalWrite(ledCtrl.ledPin, LOW);
}

void loop() {
  bool buttonState = digitalRead(ledCtrl.switchPin);

  // Detect button press (falling edge)
  if (ledCtrl.lastButtonState == HIGH && buttonState == LOW) {
    unsigned long now = millis();

    if (now - ledCtrl.lastPressTime < 400) {  
      // Within 400ms → double press
      ledCtrl.pressCount++;
    } else {
      // New sequence
      ledCtrl.pressCount = 1;
    }

    ledCtrl.lastPressTime = now;
  }

  ledCtrl.lastButtonState = buttonState;

  // Check actions
  if (ledCtrl.pressCount == 1 && millis() - ledCtrl.lastPressTime > 400) {
    // Single press action
    if (!ledCtrl.ledOn && !ledCtrl.ledBlink) {
      // OFF → turn ON
      ledCtrl.ledOn = true;
      digitalWrite(ledCtrl.ledPin, HIGH);
    } 
    else if (ledCtrl.ledOn && !ledCtrl.ledBlink) {
      // ON → turn OFF
      ledCtrl.ledOn = false;
      digitalWrite(ledCtrl.ledPin, LOW);
    } 
    else if (ledCtrl.ledBlink) {
      // If blinking → stop and turn OFF
      ledCtrl.ledBlink = false;
      digitalWrite(ledCtrl.ledPin, LOW);
    }
    ledCtrl.pressCount = 0;
  }
  else if (ledCtrl.pressCount == 2) {
    // Double press action → Blink mode
    ledCtrl.ledOn = false;
    ledCtrl.ledBlink = true;
    ledCtrl.pressCount = 0;
  }

  // Handle blinking
  if (ledCtrl.ledBlink) {
    if (millis() - ledCtrl.blinkTimer > 500) { // 500ms interval
      ledCtrl.blinkTimer = millis();
      digitalWrite(ledCtrl.ledPin, !digitalRead(ledCtrl.ledPin));
    }
  }
}
