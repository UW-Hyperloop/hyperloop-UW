#include <tbm.h>

using namespace std;

// ---------------------------------------------------------
//  Initialize all fields of systemData
// ---------------------------------------------------------
void initSystemData() {
  systemData.state         = STATE_STOP;
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
  JsonObject flowTempObj = doc["flow_temp"].to<JsonObject>();
  flowTempObj["active"]    = systemData.flow_temp.active;
  flowTempObj["value"]     = systemData.flow_temp.value;
  flowTempObj["timestamp"] = systemData.flow_temp.timestamp;

  // ... flow_in, flow_out, motor_power, pump_power, etc.

  // Done, serialize
  String output;
  serializeJson(doc, output);
  return output;
}