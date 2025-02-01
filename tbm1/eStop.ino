#include <Arduino.h>
#include "tbm.h"


void eStop_setup() {
 
  pinMode(ESTOP_PIN, INPUT_PULLUP);
}


void eStop_loop() {
  int buttonState = digitalRead(ESTOP_PIN);
  if (buttonState == HIGH) {

    Serial.println("Emergency Stop Activated!");

  } else {
    // Normal operation
    // ...
  }
  // Add any additional E-stop logic you need
}
