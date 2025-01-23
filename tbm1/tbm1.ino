#include "../headers/tbm.h"
# include <Arduino.h>

char *state;
sys_json SystemData;

enum State { 
  CONFIG,  
  RUNNING, 
  ERROR, 
  STOP
}; 

// "currentState" is used of state definition throughout file 
// idfk what "state" is used for 
State currentState = CONFIG; 
const float maxTemp = 0;  
const float adcResolution = 65536;  // 32-bit resolution 

void setup() {
  // put your setup code here, to run once:
  // output to transistors for supplying power
  pinMode(ESTOPCTRL_PIN, OUTPUT);
  pinMode(MOTORCTRL_PIN, OUTPUT);
  pinMode(PUMPCTRL_PIN, OUTPUT);
  pinMode(BENTCTRL_PIN, OUTPUT);

  // digital input pins for monitoring power
  pinMode(MOTORSENSE_PIN, INPUT);
  pinMode(PUMPSENSE_PIN, INPUT);
  pinMode(ESTOPSENSE_PIN, INPUT);

  // analog input pins for sensors
  pinMode(MOTOR_TEMP_PIN, INPUT);
  pinMode(PUMP_TEMP_PIN, INPUT);
  pinMode(FLOW_IN_PIN, INPUT);
  pinMode(FLOW_OUT_PIN, INPUT);

  // TODO set up uart communication to ethernet module

  TestFunction();
  state = START;
  Serial.println("System initialized - Entering CONFIG state");
}
  // start 
void loop(){ 
  switch (currentState) { 
    case CONFIG: 
      Serial.println("CONFIG: Configuring system"); // delay(3000) if needed 
      currentState = RUNNING; 
      break; 

    case RUNNING:
      Serial.println("RUNNING - Current temp is %.1f°C\n", SystemData.motor_temp.value); 
      if (SystemData.motor_temp.value >= maxTemp || SystemData.estop_button.value == 1) { 
        Serial.println("ERROR: Max Temp exceeded - Overheating detected"); 
        currentState = ERROR; 
      }
      // else if - condition for other sensors : flow meter, bentonite sensor   
      break; 

    case ERROR: 
      Serial.println("Stopping system"); 
      currentState = STOP; 
      break; 

    case STOP:
        // every time the system is stopped, it automatically resets. 
        Serial.println("System stopped. Resetting system. "); 
        currentState = CONFIG; 
      break;  

    default: 
      Serial.println("Unknown state"); 
      currentState = CONFIG; 
      break; 

  }
}
