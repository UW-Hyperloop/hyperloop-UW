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
  pinMode(MOTORCTRL_PIN, OUTPUT);
  pinMode(PUMPCTRL_PIN, OUTPUT);
  pinMode(ESTOPSENSE_PIN, INPUT);
  pinMode(ESTOPCTRL_PIN, OUTPUT);
  systemData.estop_button.value = 0;

  // need to figure out logic for pump and motor controls when in config vs running. 
  // should be a difference given we're doing a start command. 
  // common sense - it shouldnt be high but it should be something else.

  // digitalWrite(MOTORCTRL_PIN, HIGH);
  // digitalWrite(PUMPCTRL_PIN, HIGH);
  
  return msg;
}

// void tbm_start_stop() {
//   // if (!CH9121.available()) {
//   //   delay(1000);
//   //   if (!CH9121.available()) {
//   //     Serial.println("no incoming message");
//   //     return;
//   //   }
//   // }

//   uint8_t* msg = new uint8_t[3];
//   CH9121.flush();
//   if (CH9121.available() < 3) {
//     // Not enough data yet, return and try again on the next loop iteration
//     return;
//   }
//   Serial.println("we through");
//   uint8_t readRet = CH9121.readBytes(msg, 3);
//   Serial.println(readRet);
//   if (readRet != 3) {
//     // error condition
//     return;
//   }

//   Serial.println("valid msg");
//   Serial.println(static_cast<String>(msg[0]));
//   Serial.println(static_cast<String>(msg[2]));
//   Serial.println("over");
//   if (msg[0] == MSG_START && msg[2] == MSG_END) {
//     if (msg[1] == TBM_START) {   // message id must match tbm_start
//       digitalWrite(MOTORCTRL_PIN, HIGH);
//       digitalWrite(PUMPCTRL_PIN, HIGH);
//       systemData.state = STATE_RUNNING;
//     } else if (msg[1] == TBM_STOP) {
//       digitalWrite(MOTORCTRL_PIN, LOW);
//       digitalWrite(PUMPCTRL_PIN, LOW);
//       systemData.state = STATE_STOP;
//     } else {
//         // error condition
//     }
//   }
// }

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
      if (digitalRead(ESTOPSENSE_PIN)) {
        Serial.println("estop activated. Reject start");
        // send out as json as well if possible. separate logic chain for that :(
        return;
      }
      Serial.println("Received TBM_START command");
      digitalWrite(MOTORCTRL_PIN, HIGH);
      digitalWrite(PUMPCTRL_PIN, HIGH);
      systemData.state = STATE_RUNNING;
    } else if (buffer[1] == TBM_STOP) {
      Serial.println("Received TBM_STOP command");
      digitalWrite(MOTORCTRL_PIN, LOW);
      digitalWrite(PUMPCTRL_PIN, LOW);
      systemData.state = STATE_STOP;
    } else {
      Serial.println("Received unknown TBM command");
      // Optional: handle error
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

uint8_t* tbm_error() {
  uint8_t* msg = new uint8_t[MSG_SIZE];
  msg[0] = 0x02;
  msg[514] = 0x03;
  msg[1] = TBM_ERROR;
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