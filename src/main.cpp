#include <Arduino.h>
#include <ArduinoJson.h>
#include <string.h>
#include <tbm.h>

sys_json systemData;

using namespace std;

HardwareSerial CH9121(2);

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(CFG_PIN, OUTPUT);
  delay(200);

  if (systemData.state == STATE_CONFIG) {
    config();
  }

  CH9121.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
  Serial.println("CH9121 Normal Mode, 115200.");

  uint8_t* msg = tbm_init();

  CH9121.write(msg, MSG_SIZE);
  CH9121.flush();
  delete[] msg;
  state_loop();
  tbm_start_stop();
  eStop_loop();
  state_loop();
}

void loop() {
  eStop_loop();
  state_loop();
  tbm_start_stop();
  state_loop();

  if (systemData.state == STATE_RUNNING) {
    readSensors();
    uint8_t* msg = tbm_data();
    CH9121.write(msg, MSG_SIZE);
    CH9121.flush();
    delete[] msg;
    delay(500);
  } else {
    readSensors();
  }

  tbm_start_stop();
  state_loop();
  eStop_loop();
}
 