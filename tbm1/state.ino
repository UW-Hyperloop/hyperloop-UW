#include <Arduino.h>
// option for code once we add more sensors: check states function returns the state - shd return a String 

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

// ref resolution - 32 bits 
const float adcResolution = 65536;  

void state_setup() {
  Serial.begin(9600);
  Serial.println("System initialized - Entering CONFIG state");
}
  // start 
void state_loop(){ 
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

  


  

