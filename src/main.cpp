#include <Arduino.h>
#include <ArduinoJson.h>
#include <string.h>
#include <tbm.h>

sys_json systemData;

using namespace std;
static TBMState STATE = STATE_CONFIG;

HardwareSerial CH9121(2);

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(CFG_PIN, OUTPUT);
  delay(200);

  if (STATE == STATE_CONFIG) {
    config();
  }

  CH9121.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
  Serial.println("CH9121 Normal Mode, 115200.");

  uint8_t* msg = tbm_init();

  CH9121.write(msg, MSG_SIZE);
  state_loop();
  eStop_loop();
  tbm_start_stop();
  state_loop();
}

void loop() {
  eStop_loop();
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
  readSensors();
  state_loop();
  uint8_t* msg = tbm_data();
  CH9121.write(msg, MSG_SIZE);
  delay(1000);
  tbm_start_stop();
  state_loop();
  eStop_loop();
}
 