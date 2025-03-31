#include <tbm.h>
using namespace std;

// ---------------------------------------------------------
//  Initialize all fields of systemData
// ---------------------------------------------------------
void initSystemData() {
  systemData.state         = STATE_CONFIG;
  systemData.global_time   = 0;

  systemData.motor_temp    = { true, 0, 0 };
  systemData.flow_temp     = { true, 0, 0 };
  systemData.flow_in       = { true, 0, 0 };
  systemData.flow_out      = { true, 0, 0 };
  systemData.motor_power   = { true, 0, 0 };
  systemData.pump_power    = { true, 0, 0 };
  systemData.bentonite_power  = { true, 0, 0 };
  systemData.estop_button  = { true, 0, 0 };
}

// ---------------------------------------------------------
//  Build JSON string from systemData
// ---------------------------------------------------------
String constructJsonPayload() {
  JsonDocument doc;

  // Use helper to convert enum to string
  doc["state"]       = stateToString(systemData.state);
  doc["global_time"] = systemData.global_time;

  JsonObject motorTempObj = doc["motor_temp"].to<JsonObject>();
  motorTempObj["active"]    = systemData.motor_temp.active;
  motorTempObj["value"]     = systemData.motor_temp.value;
  motorTempObj["timestamp"] = systemData.motor_temp.timestamp;

  // Repeat pattern for other sensors:
  // JsonObject pumpTempObj = doc["pump_temp"].to<JsonObject>();
  // pumpTempObj["active"]    = systemData.pump_temp.active;
  // pumpTempObj["value"]     = systemData.pump_temp.value;
  // pumpTempObj["timestamp"] = systemData.pump_temp.timestamp;

  // ... flow_in, flow_out, motor_power, pump_power, etc.
  JsonObject gasSensorObj = doc["gas_sensor"].to<JsonObject>();
  gasSensorObj["active"] = systemData.gas_sensor.active; 
  gasSensorObj["value"] = systemData.gas_sensor.value; 
  gasSensorObj["timestamp"] = systemData.gas_sensor.timestamp; 

  // Done, serialize
  String output;
  serializeJson(doc, output);
  return output;
}

// ---------------------------------------------------------
//  Example send function (currently just prints to Serial)
// ---------------------------------------------------------
void sendJsonPayload(const String &payload) {
  Serial.println(payload);
}

// ---------------------------------------------------------
//  JSON_loop: read the constructed JSON and send it out
// ---------------------------------------------------------
void JSON_loop() {
  // Typically you'd do readSensors() & updateSystemState() 
  // in your main loop or state loop. 
  // But if you want to handle them here, you can — be consistent.

  String payload = constructJsonPayload();
  sendJsonPayload(payload);
}