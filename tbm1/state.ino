# include <Arduino.h>
// check states function returns the state - shd return a String 

const int thermocouplePin = 0; 
// defines system states 
enum State { 
  CONFIG,  
  RUNNING, 
  ERROR, 
  STOP
};

//sets current state to config 
State currentState = CONFIG; 

// temp threshold 
// amplifier module used: AD8495
const float maxTemp = 0;  

//ESP32 ADC ref voltage in voltage 
const float referenceVoltage = 3.3;
// ref resolution - 32 bits 
const float adcResolution = 65536;  

void setup() {
  Serial.begin(9600);
  pinMode(thermocouplePin, INPUT); 
  Serial.printf("System initialized - Entering CONFIG state");
}
  // start 
void loop(){ 
  int adcIn = analogRead(thermocouplePin); 
  float calcVoltage = (adcIn * referenceVoltage) / adcResolution;
  float temp = (calcVoltage - 1.25) / 0.005;  // converts to °C (5 mV/°C) 
  switch (currentState) { 
    case CONFIG: 
      Serial.printf("CONFIG: Configuring system"); // delay(3000) if needed 
      currentState = RUNNING; 
      break; 

    case RUNNING:
      Serial.printf("RUNNING - Current temp is %.1f°C\n", temp); 
      if (temp >= maxTemp) {
        Serial.printf("ERROR: Max Temp exceeded - Overheating detected"); 
        currentState = ERROR; 
      } 
      break; 

    case ERROR: 
      Serial.printf("Stopping system"); 
      currentState = STOP; 
      break; 

    case STOP:
        // every time the system is stopped, it automatically resets. 
        Serial.printf("System stopped. Resetting system. "); 
        currentState = CONFIG; 
      break;  

    default: 
      Serial.printf("Unknown state"); 
      currentState = CONFIG; 
      break; 
  }
}
  
