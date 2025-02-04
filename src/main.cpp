#include <Arduino.h>
#include <ArduinoJson.h>
#include <string.h>
#include <tbm.h>
#include <config.h>

sys_json systemData;

using namespace std;
static const int MSG_START = 0x02;
static const int MSG_END = 0x03;
static string STATE = "CONFIG";

HardwareSerial CH9121(2);

void sysData() {
// Initialize systemData with default/safe values
  systemData.state = STOP;          // By default, let's say we're "stopped"
  systemData.global_time = 0;       // Start global time at 0

  // Now initialize each sensor. For example:
  systemData.motor_temp.active       = true; 
  systemData.motor_temp.value        = 0; 
  systemData.motor_temp.timestamp    = 0;

  systemData.flow_temp.active        = true; 
  systemData.flow_temp.value         = 0; 
  systemData.flow_temp.timestamp     = 0;

  systemData.flow_in.active          = true; 
  systemData.flow_in.value           = 0; 
  systemData.flow_in.timestamp       = 0;

  systemData.flow_out.active         = true; 
  systemData.flow_out.value          = 0; 
  systemData.flow_out.timestamp      = 0;

  systemData.motor_power.active      = true; 
  systemData.motor_power.value       = 0; 
  systemData.motor_power.timestamp   = 0;

  systemData.pump_power.active       = true; 
  systemData.pump_power.value        = 0; 
  systemData.pump_power.timestamp    = 0;

  systemData.bentonite_power.active  = true;
  systemData.bentonite_power.value   = 0; 
  systemData.bentonite_power.timestamp = 0;

  systemData.estop_button.active     = true; 
  systemData.estop_button.value      = 0; 
  systemData.estop_button.timestamp  = 0;
}

string constructJsonPayload() {
  StaticJsonDocument<512> doc;

  doc["state"] = systemData.state;
  doc["global_time"] = systemData.global_time;

  // 3. Create nested JSON objects for each sensor
  JsonObject motorTempObj = doc.createNestedObject("motor_temp");
  motorTempObj["active"]    = systemData.motor_temp.active;
  motorTempObj["value"]     = systemData.motor_temp.value;
  motorTempObj["timestamp"] = systemData.motor_temp.timestamp;

  JsonObject flowTempObj = doc.createNestedObject("flow_temp");
  flowTempObj["active"]    = systemData.flow_temp.active;
  flowTempObj["value"]     = systemData.flow_temp.value;
  flowTempObj["timestamp"] = systemData.flow_temp.timestamp;

  JsonObject flowInObj = doc.createNestedObject("flow_in");
  flowInObj["active"]    = systemData.flow_in.active;
  flowInObj["value"]     = systemData.flow_in.value;
  flowInObj["timestamp"] = systemData.flow_in.timestamp;

  JsonObject flowOutObj = doc.createNestedObject("flow_out");
  flowOutObj["active"]    = systemData.flow_out.active;
  flowOutObj["value"]     = systemData.flow_out.value;
  flowOutObj["timestamp"] = systemData.flow_out.timestamp;

  JsonObject motorPowerObj = doc.createNestedObject("motor_power");
  motorPowerObj["active"]    = systemData.motor_power.active;
  motorPowerObj["value"]     = systemData.motor_power.value;
  motorPowerObj["timestamp"] = systemData.motor_power.timestamp;

  JsonObject pumpPowerObj = doc.createNestedObject("pump_power");
  pumpPowerObj["active"]    = systemData.pump_power.active;
  pumpPowerObj["value"]     = systemData.pump_power.value;
  pumpPowerObj["timestamp"] = systemData.pump_power.timestamp;

  JsonObject bentonitePowerObj = doc.createNestedObject("bentonite_power");
  bentonitePowerObj["active"]    = systemData.bentonite_power.active;
  bentonitePowerObj["value"]     = systemData.bentonite_power.value;
  bentonitePowerObj["timestamp"] = systemData.bentonite_power.timestamp;

  JsonObject estopButtonObj = doc.createNestedObject("estop_button");
  estopButtonObj["active"]    = systemData.estop_button.active;
  estopButtonObj["value"]     = systemData.estop_button.value;
  estopButtonObj["timestamp"] = systemData.estop_button.timestamp;

  // 4. Serialize JSON to a String
  string output;
  serializeJson(doc, output);

  // 5. Return the JSON string
  return output;
}

uint8_t* tbm_init() {
  uint8_t* msg = new uint8_t[515];
  msg[0] = 0x02;
  msg[514] = 0x03;
  msg[1] = 0x05;
  sysData();
  string json = constructJsonPayload();

  for (int i = 0; i < json.length(); i++) {
    msg[i + 2] = static_cast<uint8_t>(json[i]); // Convert char to ASCII and store in msg
  }

  Serial.println(json.length());
  
  return msg;
}

uint8_t* tbm_data() {
  uint8_t* msg = new uint8_t[515];
  msg[0] = 0x02;
  msg[514] = 0x03;
  msg[1] = 0x05;
  string json = constructJsonPayload();

  for (int i = 0; i < json.length(); i++) {
    msg[i + 2] = static_cast<uint8_t>(json[i]); // Convert char to ASCII and store in msg
  }

  return msg;
}

void tbm_start_stop(int msg_id, int cmd) {
  while (!CH9121.available()) {
    continue;
  }

  uint8_t* msg = new uint8_t[3];
  uint8_t readRet;
  if (CH9121.available()) {
    readRet = CH9121.readBytes(msg, 3);
  }

  if (readRet != 3) {
    // error condition
  }

  if (msg[0] == MSG_START && msg[2] == MSG_END) {
    if (msg[1] == msg_id) {   // message id must match tbm_start
      pinMode(MOTORCTRL_PIN, OUTPUT);
      pinMode(PUMPCTRL_PIN, OUTPUT);

      digitalWrite(MOTORCTRL_PIN, cmd);
      digitalWrite(PUMPCTRL_PIN, cmd);
    }
  }
}

void tbm_start() {
  tbm_start_stop(1);
}

void tbm_stop() {
  tbm_start_stop(0);
}


void setup() {
  Serial.begin(115200);
  delay(1000);
  // Now the CH9121 is in data mode at 115200
  pinMode(CFG_PIN, OUTPUT);
  delay(200);

  if (STATE == "CONFIG") {
    config();
  }

  CH9121.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
  Serial.println("CH9121 Normal Mode, 115200.");

  sysData();

  uint8_t* msg = tbm_init();
  size_t msg_size = 515;

  CH9121.write(msg, msg_size);
  bool flag = false;

  while (!flag) {
    if (CH9121.available()) {
      break;
    }
    flag = true;
  }
}

void loop() {
  // Pass CH9121 -> Serial Monitor
  if (CH9121.available()) {
    int b = CH9121.read();
    Serial.write(b);
  }
 
  // Pass Serial Monitor -> CH9121
  if (Serial.available()) {
    int b = Serial.read();
    CH9121.write(b);
  }

  delay(3000);
  uint8_t* msg = tbm_data();
  size_t msg_size = 515;

  CH9121.write(msg, msg_size);
}
 