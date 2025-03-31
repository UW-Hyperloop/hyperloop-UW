#include <tbm.h>

uint8_t* tbm_init() {
  uint8_t* msg = new uint8_t[515];
  msg[0] = 0x02;
  msg[514] = 0x03;
  msg[1] = TBM_INIT;
  initSystemData();
  String json = constructJsonPayload();

  for (int i = 2; i < MSG_SIZE - 1; i++) {
    if ((i-2) < json.length()) {
      msg[i] = static_cast<uint8_t>(json[i - 2]); // Convert char to ASCII and store in msg
    } else {
      msg[i] = 0x00;
    }
  }

  Serial.println(json.length());
  pinMode(POWCTRL_PIN, OUTPUT);
  pinMode(ESTOPSENSE_PIN, INPUT);
  systemData.estop_button.value = 0;

  // need to figure out logic for pump and motor controls when in config vs running. 
  // should be a difference given we're doing a start command. 
  // common sense - it shouldnt be high but it should be something else.

  // digitalWrite(MOTORCTRL_PIN, HIGH);
  // digitalWrite(PUMPCTRL_PIN, HIGH);
  
  return msg;
}

bool tbm_ack() {
  if (CH9121.available() < 3) {
    return false; 
  }

  uint8_t buffer[3];
  // Attempt to read exactly 3 bytes
  int readRet = CH9121.readBytes(buffer, 3);
  if (readRet != 3) {
    // Could not read all 3 bytes; just return (or handle as error).
    return false;
  }

  if (buffer[0] == MSG_START && buffer[2] == MSG_END) {
    if (buffer[1] == TBM_ACK) {
      return true;
    }
  }

  return false;
}

void tbm_start_stop() {
  // If fewer than 3 bytes are available, we can't parse a command yet.
  if (CH9121.available() < 3) {
    return; 
  }

  uint8_t buffer[3];
  // Attempt to read exactly 3 bytes
  int readRet = CH9121.readBytes(buffer, 3);
  if (readRet != 3) {
    // Could not read all 3 bytes; just return (or handle as error).
    return;
  }

  // Check the first and last byte
  if (buffer[0] == MSG_START && buffer[2] == MSG_END) {
    // Middle byte tells us which command
    if (buffer[1] == TBM_START) {
      if (!digitalRead(ESTOPSENSE_PIN)) {
        Serial.println("estop activated. Reject start");
        CH9121.flush();
        // send out as json as well if possible. separate logic chain for that :(
        return;
      }
      Serial.println("Received TBM_START command");
      digitalWrite(POWCTRL_PIN, HIGH);
      systemData.state = STATE_RUNNING;
      while (CH9121.available() > 0) {
          CH9121.read();  // dump all incoming bytes
      }
    } else if (buffer[1] == TBM_STOP) {
      Serial.println("Received TBM_STOP command");
      digitalWrite(POWCTRL_PIN, LOW);
      systemData.state = STATE_STOP;
      while (CH9121.available() > 0) {
          CH9121.read();  // dump all incoming bytes
      }
    } else if (buffer[1] == TBM_ACK) {
      ack = true;
      missCount = 0;
    } else {
      Serial.println("Received unknown TBM command");
    }
  } else {
    // If it’s not in the [start, command, end] format, decide how to handle it.
    // For example, you could forward these bytes to Serial for debugging:
    Serial.println("Received non-command bytes:");
    for (int i = 0; i < 3; i++) {
      Serial.print(buffer[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
}

uint8_t* tbm_data() {
  uint8_t* msg = new uint8_t[MSG_SIZE];
  msg[0] = 0x02;
  msg[514] = 0x03;
  msg[1] = TBM_DATA;
  String json = constructJsonPayload();
  Serial.println(json);
  
  for (int i = 2; i < MSG_SIZE - 1; i++) {
    if ((i-2) < json.length()) {
      msg[i] = static_cast<uint8_t>(json[i - 2]); // Convert char to ASCII and store in msg
    } else {
      msg[i] = 0x00;
    }
  }
  
  return msg;
}

uint8_t* tbm_estop() {
  uint8_t* msg = new uint8_t[3];
  msg[0] = 0x02;
  msg[2] = 0x03;
  msg[1] = TBM_ERROR;

  return msg;
}

uint8_t* tbm_estop_start() {
  uint8_t* msg = new uint8_t[3];
  msg[0] = 0x02;
  msg[2] = 0x03;
  msg[1] = TBM_START;

  return msg;
}