#include <Arduino.h>
#include <tbm.h>
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
      Serial.println("CONFIG: Configuring system"); 
      if (!checkStopped()) { 
        currentState = ERROR; 
      }
      while (incomingMessage != TBM_START) { 
        currentState = CONFIG; 
      } else { 
        currentState = RUNNING; 
      }
      break; 

    case RUNNING:
      Serial.println("RUNNING - Current temp is %.1f°C\n", SystemData.motor_temp.value); 
      if (SystemData.motor_temp.value >= maxTemp || SystemData.estop_button.value == 1 || incomingMessage == TBM_STOP) { 
        Serial.println("ERROR: Stopping TBM"); 
        currentState = ERROR; 
      }
      break; 

    case ERROR: 
      Serial.println("Stopping system"); 
      digitalWrite(MOTORCTRL_PIN, LOW);     // stops motor
      digitalWrite(PUMPCTRL_PIN, LOW);      // stops water pump 
      digitalWrite(BENTCTRL_PIN, LOW);      // stops bentonite pump 
      currentState = STOP; 
      break; 

    case STOP:
        // every time the system is stopped, it automatically resets. 
        if (!checkStopped()) { 
          currentState = ERROR; 
        } else { 
          Serial.println("System stopped. Resetting system."); 
          while (incomingMessage != TBM_START) { 
            currentState = STOP; 
          }
          currentState = CONFIG; 
        }
      break;  

    default: 
      Serial.println("Unknown state"); 
      currentState = CONFIG; 
      break; 

  }
}

bool checkStopped() { 
  if (digitalRead(MOTORCTRL_PIN) != LOW) { 
    Serial.println("Motor not stopped!"); 
    return false; 
  } else if (digitalRead(PUMPCTRL_PIN) != LOW) { 
    Serial.println("Water pump not off!"); 
    return false; 
  } else if (digitalRead(BENTCTRL_PIN) != LOW) { 
    Serial.println("Bentonite pump not off! "); 
    return false; 
  } else { 
    return true; 
  }
}


  


  

