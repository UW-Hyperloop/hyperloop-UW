#include <Arduino.h>
#include <ArduinoJson.h>
#include <string.h>
#include <tbm.h>

sys_json systemData;

using namespace std;

uint8_t missCount = 0;
bool ack = false;
int sensorCount = 0;

HardwareSerial CH9121(2);

static const unsigned long SENSOR_READ_INTERVAL = 250;
static const unsigned long DATA_SEND_INTERVAL   = 500;

static unsigned long lastSensorReadTime = 0;
static unsigned long lastDataSendTime   = 0;

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
  receivingESP_setup();
}

void loop() {
  eStop_loop();
  state_loop();
  tbm_start_stop();
  state_loop();
  unsigned long now = millis();
  if (now - lastSensorReadTime >= SENSOR_READ_INTERVAL) {
    lastSensorReadTime = now;
    readSensors();
  }

  if (systemData.state == STATE_RUNNING) {
    if (now - lastDataSendTime >= DATA_SEND_INTERVAL) {
      lastDataSendTime = now;

      uint8_t* msg = tbm_data();
      CH9121.write(msg, MSG_SIZE);
      CH9121.flush();
      delete[] msg;

      if (!ack) {
        // If we keep missing ACK, stop the TBM
        if (missCount > 5) {
          systemData.state = STATE_STOP;
          stoppingTBM();
          Serial.println("Too many missed ACKs -> STOP");
        }
      }
      ack = false;
      missCount++;
    }
  } else if (systemData.state == STATE_STOP && systemData.estop_button.value == 0) {
    if (now - lastDataSendTime >= 1000) {
      lastDataSendTime = now;
      uint8_t* msg = tbm_data();
      CH9121.write(msg, MSG_SIZE);
      CH9121.flush();
      delete[] msg;
    }
  } else {
    readSensors();
  }

  state_loop();
  eStop_loop();
}
