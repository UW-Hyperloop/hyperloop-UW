#include <Arduino.h>
#include <tbm.h>

#define DEBOUNCE_TIME 500 // in ms
int last_sample = 0; // time of last button sample
bool onflag = false;
bool offlag = false;

void eStop_loop() {
  int current_sample = millis();
  int buttonState = digitalRead(ESTOPSENSE_PIN);
  // Serial.println(buttonState);
  if (current_sample - last_sample > 500) {
    // Read the state of the emergency stop button
    int buttonState = digitalRead(ESTOPSENSE_PIN);

    if (buttonState == LOW) {
      Serial.println("Emergency Stop Activated!");
      // TODO: add stop logic
      systemData.estop_button.value = 1;
      if (!onflag) {
        while (CH9121.available() > 0) {
          CH9121.read();  // dump all incoming bytes
        }
        uint8_t* msg = tbm_estop();
        CH9121.write(msg, 3);
        Serial.flush();
        delete[] msg;
        Serial.flush();
      }
      onflag = true;
      offlag = false;
      
    } else {
      TBMState state = systemData.state;
      if (state == STATE_CONFIG) {
        systemData.estop_button.value = 0;
      } else if (state == STATE_STOP) {
        if (systemData.estop_button.value == 1) {        
          systemData.state = STATE_CONFIG;
        }
        systemData.estop_button.value = 0;

      } else {
        systemData.estop_button.value = 0;
      }

      if (!offlag) {
        while (CH9121.available() > 0) {
          CH9121.read();  // dump all incoming bytes
        }
        uint8_t* msg = tbm_estop_start();
        CH9121.write(msg, 3);
        Serial.flush();
        delete[] msg;
        Serial.flush();
      }

      offlag = true;
      onflag = false;
      //Serial.println("Estop button released. System Running Normally");
    }
  }
}
