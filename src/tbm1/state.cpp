// #include <Arduino.h>
// #include <tbm.h>
// #include <string.h>

// // defines system states 
// enum State { 
//   STATE_CONFIG,  
//   STATE_RUNNING, 
//   STATE_ERROR, 
//   STATE_STOP
// };

// //sets current state to config 
// State currentState = STATE_CONFIG; 

// // temp threshold 
// // amplifier module used: AD8495
// const float maxTemp = 0;  

// // ref resolution - 32 bits 
// const float adcResolution = 65536;  

// void state_setup() {
//   Serial.begin(9600);
//   Serial.println("System initialized - Entering CONFIG state");
// }

// void state_loop(){
//   switch (currentState) { 
//     case STATE_CONFIG: 
//       Serial.println("CONFIG: Configuring system"); 
//       // Send: TBM_CONFIG
//       if (!checkStopped()) { 
//         currentState = STATE_ERROR; 
//         // Send: TBM_ERROR
//       } else if (incomingMessage != TBM_START) { 
//         currentState = STATE_CONFIG; 
//       } else { 
//         currentState = STATE_RUNNING; 
//       }
//       break; 

//     case STATE_RUNNING:
//       Serial.println("RUNNING - Current temp is %.1f°C\n", SystemData.motor_temp.value); 
//       if (SystemData.motor_temp.value >= maxTemp || SystemData.estop_button.value == 1 || incomingMessage == TBM_STOP) { 
//         Serial.println("ERROR: Stopping TBM"); 
//         currentState = ERROR; 
//       }
//       break; 

//     case STATE_ERROR:
//       //Send TMB_ERROR
//       Serial.println("Stopping system"); 
//       digitalWrite(MOTORCTRL_PIN, LOW);     // stops motor
//       digitalWrite(PUMPCTRL_PIN, LOW);      // stops water pump 
//       digitalWrite(BENTCTRL_PIN, LOW);      // stops bentonite pump 
//       // send message to server 
//       currentState = STOP; 
//       break; 

//     case STATE_STOP:
//         // every time the system is stopped, it automatically resets. 
//         if (!checkStopped()) { 
//           currentState = ERROR; 
//         } else { 
//           Serial.println("System stopped. Resetting system."); 
//           if (incomingMessage != TBM_START) { 
//             currentState = STATE_STOP; 
//           } else { 
//             currentState = STATE_CONFIG; 
//           }  
//         }
//       break;  

//     default: 
//       Serial.println("Unknown state"); 
//       currentState = CONFIG; 
//       break; 

//   }
// }

// bool checkStopped() { 
//   if (digitalRead(MOTORCTRL_PIN) != LOW) { 
//     Serial.println("Motor not stopped!"); 
//     return false; 
//   } else if (digitalRead(PUMPCTRL_PIN) != LOW) { 
//     Serial.println("Water pump not off!"); 
//     return false; 
//   } else if (digitalRead(BENTCTRL_PIN) != LOW) { 
//     Serial.println("Bentonite pump not off! "); 
//     return false; 
//   } else { 
//     return true; 
//   }
// }
