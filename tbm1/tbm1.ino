#include <Arduino.h>
#include "tbm.h"


sys_json systemData;       
TBMState currentState = STATE_CONFIG; 


void setup() {
  Serial.begin(115200);
  
  pinMode(ESTOPCTRL_PIN, OUTPUT);
  pinMode(MOTORCTRL_PIN, OUTPUT);
  pinMode(PUMPCTRL_PIN, OUTPUT);
  pinMode(BENTCTRL_PIN, OUTPUT);

  pinMode(MOTORSENSE_PIN, INPUT);
  pinMode(PUMPSENSE_PIN, INPUT);
  pinMode(ESTOPSENSE_PIN, INPUT);
  pinMode(MOTOR_TEMP_PIN, INPUT);
  pinMode(PUMP_TEMP_PIN, INPUT);
  //pinMode(FLOW_IN_PIN,    INPUT);
  //pinMode(FLOW_OUT_PIN,   INPUT);



 
  initSystemData();


  state_setup();
}

void loop(){

  eStop_loop();


  readSensors();


  updateSystemState();

  state_loop();


  JSON_loop();

  delay(1000); 
}
