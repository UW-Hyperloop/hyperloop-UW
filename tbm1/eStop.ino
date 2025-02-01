#include <Arduino.h>
#include "tbm.h"



void eStop_loop() {
    // Read the state of the emergency stop button
    int buttonState = digitalRead(ESTOPCTRL_PIN);
    
    if (buttonState == HIGH) {
        Serial.println("Emergency Stop Activated!");
        // TODO: add stop logic
        systemData.estop_button.value = 1;
    } else {
        //Serial.println("System Running Normally");
    }
    
}
