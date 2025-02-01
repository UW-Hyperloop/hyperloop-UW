#include <Arduino.h>
#include "tbm.h"

// ---------------------------------------------------------
//  Setup for state machine
// ---------------------------------------------------------
void state_setup() {
  Serial.println("state_setup: Entering CONFIG");
  currentState = STATE_CONFIG;
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
//  Update systemData.state based on sensor data, e-stop, etc.
//  This is separate from the finite state machine transitions.
// ---------------------------------------------------------
void updateSystemState() {
  // Example: if E-stop is pressed, switch to ERROR
  if (systemData.estop_button.value == 1) {
    systemData.state = STATE_ERROR;
    digitalWrite(ESTOPCTRL_PIN, HIGH); 
    digitalWrite(MOTORCTRL_PIN, LOW);
    digitalWrite(PUMPCTRL_PIN, LOW);
    digitalWrite(BENTCTRL_PIN, LOW);
    return;
  }

  // Example: check motor temp
  if (systemData.motor_temp.value >= maxTemp) {
    systemData.state = STATE_ERROR;
    digitalWrite(ESTOPCTRL_PIN, HIGH); 
    digitalWrite(MOTORCTRL_PIN, LOW);
    digitalWrite(PUMPCTRL_PIN, LOW);
    digitalWrite(BENTCTRL_PIN, LOW);
    return;
  }

  // Otherwise, let it run
  systemData.state = STATE_RUNNING;
  digitalWrite(ESTOPCTRL_PIN, LOW);
  digitalWrite(MOTORCTRL_PIN, HIGH);
  digitalWrite(PUMPCTRL_PIN, HIGH);
  digitalWrite(BENTCTRL_PIN, HIGH);
}

// ---------------------------------------------------------
//  Finite state machine for TBM 
//  (Transitions between CONFIG, RUNNING, ERROR, STOP)
// ---------------------------------------------------------
void state_loop() {
  switch(currentState) {
    case STATE_CONFIG:
      Serial.println("STATE_CONFIG: Checking if system is stopped...");
      if (!checkStopped()) {
        currentState = STATE_ERROR;
      } else {
        currentState = STATE_RUNNING;
      }
      break;

    case STATE_RUNNING:
      // Simple example: if systemData says "ERROR", jump to error
      if (systemData.state == STATE_ERROR) {
        currentState = STATE_ERROR;
      } else {
        // remain in RUNNING
        Serial.printf("RUNNING: Motor temp = %d C\n", systemData.motor_temp.value);
      }
      break;

    case STATE_ERROR:
      Serial.println("STATE_ERROR: Turning off everything.");
      digitalWrite(MOTORCTRL_PIN, LOW);
      digitalWrite(PUMPCTRL_PIN, LOW);
      digitalWrite(BENTCTRL_PIN, LOW);
      currentState = STATE_STOP;
      break;

    case STATE_STOP:
      Serial.println("STATE_STOP: Checking if physically off...");
      if (!checkStopped()) {
        currentState = STATE_ERROR;
      } else {
        // For demo, we can cycle back to CONFIG
        currentState = STATE_CONFIG;
      }
      break;
  }
}
