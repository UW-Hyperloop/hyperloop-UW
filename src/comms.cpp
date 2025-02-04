#include <tbm.h>

uint8_t* tbm_init() {
  uint8_t* msg = new uint8_t[515];
  msg[0] = 0x02;
  msg[514] = 0x03;
  msg[1] = 0x05;
  initSystemData();
  String json = constructJsonPayload();

  for (int i = 0; i < json.length(); i++) {
    msg[i + 2] = static_cast<uint8_t>(json[i]); // Convert char to ASCII and store in msg
  }

  Serial.println(json.length());
  
  return msg;
}

void tbm_start_stop() {
  if (!CH9121.available()) {
    delay(1000);
    if (!CH9121.available()) {
      Serial.println("no incoming message");
      return;
    }
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
    pinMode(MOTORCTRL_PIN, OUTPUT);
    pinMode(PUMPCTRL_PIN, OUTPUT);
    if (msg[1] == TBM_START) {   // message id must match tbm_start
      digitalWrite(MOTORCTRL_PIN, HIGH);
      digitalWrite(PUMPCTRL_PIN, HIGH);
    } else if (msg[1] == TBM_STOP) {
      digitalWrite(MOTORCTRL_PIN, LOW);
      digitalWrite(PUMPCTRL_PIN, LOW);
    } else {
        // error condition
    }
  }
}

uint8_t* tbm_data() {
  readSensors();
  uint8_t* msg = new uint8_t[MSG_SIZE];
  msg[0] = 0x02;
  msg[514] = 0x03;
  msg[1] = 0x05;
  String json = constructJsonPayload();
  Serial.println(json);
  
  for (int i = 0; i < json.length(); i++) {
    msg[i + 2] = static_cast<uint8_t>(json[i]); // Convert char to ASCII and store in msg
  }

  return msg;
}