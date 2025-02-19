#include <Arduino.h>
#include <tbm.h>

#define DEBOUNCE_TIME 500 // in ms
int last_sample = 0; // time of last button sample

void eStop_loop() {
  int current_sample = millis();
  int buttonState = digitalRead(ESTOPSENSE_PIN);
  // Serial.println(buttonState);
  if (current_sample - last_sample > 500) {
    // Read the state of the emergency stop button
    int buttonState = digitalRead(ESTOPSENSE_PIN);

    if (buttonState == HIGH) {
      Serial.println("Emergency Stop Activated!");
      // TODO: add stop logic
      systemData.estop_button.value = 1;
      digitalWrite(ESTOPCTRL_PIN, HIGH);
      Serial.flush();
    } else {
      TBMState state = systemData.state;
      if (state == STATE_CONFIG) {
        systemData.estop_button.value = 0;
      } else if (state == STATE_STOP) {
        if (systemData.estop_button.value == 1) {        
          systemData.state = STATE_CONFIG;
          systemData.estop_button.value = 0;
          digitalWrite(ESTOPCTRL_PIN, LOW);
        } else {
          systemData.estop_button.value = 0;
        }
      } else {
        systemData.estop_button.value = 0;
      }

      //Serial.println("Estop button released. System Running Normally");
    }
  }
}
