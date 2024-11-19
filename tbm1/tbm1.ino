#include "./headers/tbm.h"

char *state;

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
}

void loop() {
  // put your main code here, to run repeatedly:

}
