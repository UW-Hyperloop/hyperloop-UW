#include <Arduino.h>
#include <tbm.h>

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

void state_loop(){
  // TBM_DATA 
  switch (currentState) { 
    case CONFIG: 
      Serial.println("CONFIG: Configuring system"); 
      // Send: TBM_CONFIG
      if (!checkStopped()) { 
        currentState = ERROR; 
        // Send: TBM_ERROR
      } else if (incomingMessage != TBM_START) { 
        currentState = CONFIG; 
      } else { 
        currentState = RUNNING; 
      }
      break; 

    case RUNNING:
      Serial.println("RUNNING - Current temp is %.1f°C\n", SystemData.motor_temp.value); 
      if (SystemData.motor_temp.value >= maxTemp || SystemData.estop_button.value == 1 ) { 
        Serial.println("ERROR: Stopping TBM"); 
        currentState = ERROR; 
        // TBM_ERROR
      } else if (incomingMessage == TBM_STOP) {
        Serial.println("TBM_STOP received - Stopping TBM"); 
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
          if (incomingMessage != TBM_START) { 
            currentState = STOP; 
          } else { 
            currentState = CONFIG; 
          }  
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


  


  

