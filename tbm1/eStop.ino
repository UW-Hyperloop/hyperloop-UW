#include <Arduino.h>
#include "tbm.h"


void eStop_setup() {
 
  pinMode(ESTOP_PIN, INPUT_PULLUP);
}


void eStop_loop() {
    // Read the state of the emergency stop button
    int buttonState = digitalRead(ESTOPCTRL_PIN);
    
    if (buttonState == HIGH) {
        Serial.println("Emergency Stop Activated!");
        // TODO: add stop logic
        SystemData.estop_button.value = 1;
    } else {
        Serial.println("System Running Normally");
    }
    
    delay(100); // Small delay for stability
}
