#include <Arduino.h>
#include "tbm.h"

// ---------------------------------------------------------
//  Setup for state machine
// ---------------------------------------------------------
void state_setup() {
  Serial.println("state_setup: Entering CONFIG");
  systemData.state = STATE_CONFIG;
}

// ---------------------------------------------------------
//  Check if all outputs are physically off
// ---------------------------------------------------------
bool checkStopped() {
  if (digitalRead(MOTORCTRL_PIN) != LOW)  return false;
  if (digitalRead(PUMPCTRL_PIN)  != LOW)  return false;
  if (digitalRead(BENTCTRL_PIN)  != LOW)  return false;
  return true;
}

// ---------------------------------------------------------
//  Finite state machine for TBM 
//  (Transitions between CONFIG, RUNNING, ERROR, STOP)
// ---------------------------------------------------------
void state_loop() {
  switch(systemData.state) {
    case STATE_CONFIG:
      Serial.println("STATE_CONFIG: Checking if system is stopped...");
      if (!checkStopped()) {
        systemData.state = STATE_ERROR;
      } else {
        systemData.state = STATE_RUNNING;
      }
      break;

    case STATE_RUNNING:
      digitalWrite(ESTOPCTRL_PIN, LOW);
      digitalWrite(MOTORCTRL_PIN, HIGH);
      digitalWrite(PUMPCTRL_PIN, HIGH);
      digitalWrite(BENTCTRL_PIN, HIGH);
      Serial.printf("RUNNING: Motor temp = %d C\n", systemData.motor_temp.value);
      break;

    case STATE_ERROR:

      if (systemData.motor_temp.value >= maxTemp || systemData.estop_button.value == 1) {
        systemData.state = STATE_ERROR;
        digitalWrite(ESTOPCTRL_PIN, HIGH); 
      }
      Serial.println("STATE_ERROR: Turning off everything.");
      digitalWrite(MOTORCTRL_PIN, LOW);
      digitalWrite(PUMPCTRL_PIN, LOW);
      digitalWrite(BENTCTRL_PIN, LOW);
      systemData.state = STATE_STOP;
      break;

    case STATE_STOP:
      Serial.println("STATE_STOP: Checking if physically off...");
      if (!checkStopped()) {
        systemData.state = STATE_ERROR;
      } else {
        // For demo, we can cycle back to CONFIG
        systemData.state = STATE_CONFIG;
      }
      break;
  }
}

