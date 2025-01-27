#include "../headers/tbm.h"
#include <Arduino.h>
#include "state.ino"
#include <tbm.h>
#include <sensors.ino>
#include <jsonSerialization.ino>
char *state;
sys_json SystemData;

enum State { 
  CONFIG,  
  RUNNING, 
  ERROR, 
  STOP
}; 

//sets current state to config 
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

  // start adding stuff from here 
  state_setup(); 
  eStop_setup(); 
  sensorPinSetup();
  sensorDataReadLoop();
  mainJsonSetup();
  JSON_loop();

}
  // start 
void loop(){ 
  state_loop(); 
  eStop_loop(); 

}
