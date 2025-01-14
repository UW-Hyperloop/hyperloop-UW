# include <Arduino.h>
// check states function returns the state - shd return a String 

const int thermocouplePin = 0; 
// defines system states 
enum State { 
  START, 
  RUNNING, 
  ERROR, 
  STOP
}

//sets current state to config 
State currentState = CONFIG; 

// temp threshold 
// amplifier module used: AD8495
const float maxTemp = 0;  

//ESP32 ADC ref voltage in voltage 
const float referenceVoltage = 0;
// ref resolution - 32 bits 
const float adcResolution = 4096;  

void setup() {
  Serial.begin(9600);
  pinMode(thermocouplePin, INPUT); 
  Serial.println("System initialized - Entering CONFIG state");
  int adcIn = analogRead(thermocouplePin); 
  float calcVoltage = (adcIn * referenceVoltage) / adcResolution;
  float temp = voltage / 0.005;  // converts to °C (5 mV/°C) - don't trust me on this 

  // start 
  switch (currentState) { 
    case CONFIG: 
      Serial.println("CONFIG: Configuring system"); 
      // allows time for ppl to read this
      delay(3000); 
      currentState = RUNNING; 
      break; 

    case RUNNING:
      Serial.println("RUNNING - Current temp is %.1f°C\n", temp); 

      if (temp > maxTemp) {  
        Serial.println("ERROR: Max Temp exceeded - Overheating detected"); 
        currentState = ERROR; 
      }
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
      // idk if we need this 
      Serial.println("Unknown state"); 
      currentState = CONFIG; 
      break; 


  }




