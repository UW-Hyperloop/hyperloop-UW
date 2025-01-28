// #include <Arduino.h>
// #include <ArduinoJson.h>

// void setupPins();
// void sysData();



// void mainJsonSetup() {
//   jsonPinSetup();
//   sysData();
// }


// void jsonPinSetup(){
//   Serial.begin(115200);

//   pinMode(RX, OUTPUT);
//   pinMode(TX, OUTPUT);
//   pinMode(ESTOPCTRL_PIN, OUTPUT);
//   pinMode(MOTORCTRL_PIN, OUTPUT);
//   pinMode(PUMPCTRL_PIN, OUTPUT);
//   pinMode(BENTCTRL_PIN, OUTPUT);

//   // 3. Configure sense pins as inputs
//   pinMode(MOTORSENSE_PIN, INPUT);
//   pinMode(PUMPSENSE_PIN, INPUT);
//   pinMode(ESTOPSENSE_PIN, INPUT);
//   pinMode(MOTOR_TEMP_PIN, INPUT);
//   pinMode(PUMP_TEMP_PIN, INPUT);
//   pinMode(FLOW_IN_PIN, INPUT);
//   pinMode(FLOW_OUT_PIN, INPUT);
// }

// void sysData(){
// //Initialize systemData with default/safe values
//   systemData.state = STOP;          // By default, let's say we're "stopped"
//   systemData.global_time = 0;       // Start global time at 0

//   // Now initialize each sensor. For example:
//   systemData.motor_temp.active       = true; 
//   systemData.motor_temp.value        = 0; 
//   systemData.motor_temp.timestamp    = 0;

//   systemData.flow_temp.active        = true; 
//   systemData.flow_temp.value         = 0; 
//   systemData.flow_temp.timestamp     = 0;

//   systemData.flow_in.active          = true; 
//   systemData.flow_in.value           = 0; 
//   systemData.flow_in.timestamp       = 0;

//   systemData.flow_out.active         = true; 
//   systemData.flow_out.value          = 0; 
//   systemData.flow_out.timestamp      = 0;

//   systemData.motor_power.active      = true; 
//   systemData.motor_power.value       = 0; 
//   systemData.motor_power.timestamp   = 0;

//   systemData.pump_power.active       = true; 
//   systemData.pump_power.value        = 0; 
//   systemData.pump_power.timestamp    = 0;

//   systemData.bentonite_power.active  = true;
//   systemData.bentonite_power.value   = 0; 
//   systemData.bentonite_power.timestamp = 0;

//   systemData.estop_button.active     = true; 
//   systemData.estop_button.value      = 0; 
//   systemData.estop_button.timestamp  = 0;
// }

// void readSensor(){
// // 1. Read motor temperature
//   int motorTempVal = analogRead(MOTOR_TEMP_PIN);
//   systemData.motor_temp.value     = motorTempVal; 
//   systemData.motor_temp.timestamp = millis();


//   //IMPORTANT: WHAT KIND OF OUTPUT ARE WE GETTING FROM THE SENSORS, IS IT CONVERTED TO NEEDED UNITS OR NOT
//   //ASK ANYONE WHOS WORKING WITH THE SENSOR CODE.

//   // 2. Read pump temperature (flow_temp in your struct)
//   int pumpTempVal = analogRead(PUMP_TEMP_PIN);
//   systemData.flow_temp.value      = pumpTempVal;
//   systemData.flow_temp.timestamp  = millis();

//   // 3. Read flow_in
//   int flowInVal = analogRead(FLOW_IN_PIN);
//   systemData.flow_in.value        = flowInVal;
//   systemData.flow_in.timestamp    = millis();

//   // 4. Read flow_out
//   int flowOutVal = analogRead(FLOW_OUT_PIN);
//   systemData.flow_out.value       = flowOutVal;
//   systemData.flow_out.timestamp   = millis();

//   // 5. Motor power sense
//   int motorPowerVal = digitalRead(MOTORSENSE_PIN);
//   systemData.motor_power.value    = motorPowerVal;
//   systemData.motor_power.timestamp = millis();

//   // 6. Pump power sense
//   int pumpPowerVal = digitalRead(PUMPSENSE_PIN);
//   systemData.pump_power.value     = pumpPowerVal;
//   systemData.pump_power.timestamp = millis();

//   // 7. Bentonite power sense
//   // If you have a dedicated sense pin for bentonite power, read it here:
//   // int bentoniteSenseVal = digitalRead(BENT_SENSE_PIN);
//   // systemData.bentonite_power.value = bentoniteSenseVal;
//   //
//   // If not, you may hard-code or use some logic here:
//   systemData.bentonite_power.value   = 0;
//   systemData.bentonite_power.timestamp = millis();

//   // 8. E-stop button sense
//   int eStopVal = digitalRead(ESTOPSENSE_PIN);
//   systemData.estop_button.value    = eStopVal;
//   systemData.estop_button.timestamp = millis();

//   // 9. Update global_time 
//   // (This could come from millis(), an RTC, etc.)
//   systemData.global_time = millis();
  
//   // Print debug info if desired
//   // Serial.printf("Motor Temp: %d\n", systemData.motor_temp.value);
// }


// void updateSystemState() {
//   // Check e-stop first
//   if (systemData.estop_button.value == 1) {
//     systemData.state = ERROR;
//     digitalWrite(ESTOPCTRL_PIN, HIGH); // e-stop hardware engaged
//     Serial.println("E-stop engaged. System ERROR.");
//     // Stop outputs
//     digitalWrite(MOTORCTRL_PIN, LOW);
//     digitalWrite(PUMPCTRL_PIN, LOW);
//     digitalWrite(BENTCTRL_PIN, LOW);
//     return;
//   }

//   // // Check motor temperature threshold
//   // if (systemData.motor_temp.value > 3000) {
//   //   systemData.state = ERROR;
//   //   digitalWrite(ESTOPCTRL_PIN, HIGH); // e-stop or error line
//   //   Serial.println("Motor temperature too high! System ERROR.");
//   //   // Stop outputs
//   //   digitalWrite(MOTORCTRL_PIN, LOW);
//   //   digitalWrite(PUMPCTRL_PIN, LOW);
//   //   digitalWrite(BENTCTRL_PIN, LOW);
//   //   return;
//   // }


// //Not sure if we need this, review required.^^^

//   // If no error, set RUNNING
//   systemData.state = RUNNING;
//   digitalWrite(ESTOPCTRL_PIN, LOW); // e-stop hardware off
//   // Start or keep running equipment
//   digitalWrite(MOTORCTRL_PIN, HIGH);
//   digitalWrite(PUMPCTRL_PIN, HIGH);
//   digitalWrite(BENTCTRL_PIN, HIGH);
//   Serial.println("System is RUNNING.");
// }

// String constructJsonPayload(){
//   StaticJsonDocument<512> doc;

//   doc["state"] = systemData.state;
//   doc["global_time"] = systemData.global_time;

//   // 3. Create nested JSON objects for each sensor
//   JsonObject motorTempObj = doc.createNestedObject("motor_temp");
//   motorTempObj["active"]    = systemData.motor_temp.active;
//   motorTempObj["value"]     = systemData.motor_temp.value;
//   motorTempObj["timestamp"] = systemData.motor_temp.timestamp;

//   JsonObject flowTempObj = doc.createNestedObject("flow_temp");
//   flowTempObj["active"]    = systemData.flow_temp.active;
//   flowTempObj["value"]     = systemData.flow_temp.value;
//   flowTempObj["timestamp"] = systemData.flow_temp.timestamp;

//   JsonObject flowInObj = doc.createNestedObject("flow_in");
//   flowInObj["active"]    = systemData.flow_in.active;
//   flowInObj["value"]     = systemData.flow_in.value;
//   flowInObj["timestamp"] = systemData.flow_in.timestamp;

//   JsonObject flowOutObj = doc.createNestedObject("flow_out");
//   flowOutObj["active"]    = systemData.flow_out.active;
//   flowOutObj["value"]     = systemData.flow_out.value;
//   flowOutObj["timestamp"] = systemData.flow_out.timestamp;

//   JsonObject motorPowerObj = doc.createNestedObject("motor_power");
//   motorPowerObj["active"]    = systemData.motor_power.active;
//   motorPowerObj["value"]     = systemData.motor_power.value;
//   motorPowerObj["timestamp"] = systemData.motor_power.timestamp;

//   JsonObject pumpPowerObj = doc.createNestedObject("pump_power");
//   pumpPowerObj["active"]    = systemData.pump_power.active;
//   pumpPowerObj["value"]     = systemData.pump_power.value;
//   pumpPowerObj["timestamp"] = systemData.pump_power.timestamp;

//   JsonObject bentonitePowerObj = doc.createNestedObject("bentonite_power");
//   bentonitePowerObj["active"]    = systemData.bentonite_power.active;
//   bentonitePowerObj["value"]     = systemData.bentonite_power.value;
//   bentonitePowerObj["timestamp"] = systemData.bentonite_power.timestamp;

//   JsonObject estopButtonObj = doc.createNestedObject("estop_button");
//   estopButtonObj["active"]    = systemData.estop_button.active;
//   estopButtonObj["value"]     = systemData.estop_button.value;
//   estopButtonObj["timestamp"] = systemData.estop_button.timestamp;

//   // 4. Serialize JSON to a String
//   String output;
//   serializeJson(doc, output);

//   // 5. Return the JSON string
//   return output;
// }


// void sendJsonPayload(){
//   //Dan: Work with him to integrate web socket.
// }

// void handleInputs(){
//   //Same as above.
// }

// void JSON_loop() {
//   // put your main code here, to run repeatedly:
//   void readSensor;
//   void updateSystemState;
//   String jsonPayload = constructJsonPayload();
//   // void sendJsonPayload();
//   // void handleInputs();
// }
