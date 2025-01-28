#include <config.h>

bool sendConfigCommand(const uint8_t *cmd, size_t len) {
  while (CH9121.available()) {
    CH9121.read(); // clear old data
  }
  CH9121.write(cmd, len);
  CH9121.flush();
  delay(50);
  while (CH9121.available()) {
    uint8_t r = CH9121.read();
    if (r == 0xAA) {
      return true;
    }
  }
  return false;
}


void config() {
  CH9121.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
    digitalWrite(CFG_PIN, LOW);
    delay(200);

    {
      uint8_t cmd[] = {0x57, 0xAB, 0x10, 0x01};
      bool ok = sendConfigCommand(cmd, sizeof(cmd));
      Serial.println(ok ? "Port1 = TCP Server (OK)" : "Port1=TCP Server (FAILED)");
    }

    // 2) Set IP = 192.168.0.10  (command=0x11)
    {
      uint8_t cmd[] = {0x57, 0xAB, 0x11, 0xC0, 0xA8, 0x00, 0x0A};
      bool ok = sendConfigCommand(cmd, sizeof(cmd));
      Serial.println(ok ? "IP=192.168.0.10 (OK)" : "Set IP (FAILED)");
    }

    // 3) Set subnet mask = 255.255.255.0  (command=0x12)
    {
      uint8_t cmd[] = {0x57, 0xAB, 0x12, 0xFF, 0xFF, 0xFF, 0x00};
      bool ok = sendConfigCommand(cmd, sizeof(cmd));
      Serial.println(ok ? "Mask=255.255.255.0 (OK)" : "Set Mask (FAILED)");
    }

    // 4) Set gateway = 192.168.0.1 (command=0x13)
    {
      uint8_t cmd[] = {0x57, 0xAB, 0x13, 0xC0, 0xA8, 0x00, 0x01};
      bool ok = sendConfigCommand(cmd, sizeof(cmd));
      Serial.println(ok ? "Gateway=192.168.0.1 (OK)" : "Set Gateway (FAILED)");
    }

    // 5) Set local port = 2000  (command=0x14 => param= 0xD0,0x07)
    {
      uint8_t cmd[] = {0x57, 0xAB, 0x14, 0xD0, 0x07};
      bool ok = sendConfigCommand(cmd, sizeof(cmd));
      Serial.println(ok ? "Local port=2000 (OK)" : "Local port (FAILED)");
    }


    // 2) Set destination IP = 192.168.0.111  (command=0x15)
    {
      uint8_t cmd[] = {0x57, 0xAB, 0x15, 0xC0, 0xA8, 0x00, 0x6F};
      bool ok = sendConfigCommand(cmd, sizeof(cmd));
      Serial.println(ok ? "IP=192.168.0.10 (OK)" : "Set IP (FAILED)");
    }

    // 3) Set destination port = 12345 (command = 0x16)
    {
      uint8_t cmd[] = {0x57, 0xAB, 0x16, 0x39, 0x30};
      bool ok = sendConfigCommand(cmd, sizeof(cmd));
      Serial.println(ok ? "IP=192.168.0.10 (OK)" : "Set IP (FAILED)");
    }

    // 6) Set baud rate = 115200  (command=0x21 => param=0x00,0xC2,0x01,0x00)
    {
      uint8_t cmd[] = {0x57, 0xAB, 0x21, 0x00, 0xC2, 0x01, 0x00};
      bool ok = sendConfigCommand(cmd, sizeof(cmd));
      Serial.println(ok ? "Baud=115200 (OK)" : "Baud=115200 (FAILED)");
    }

    // 7) Save to EEPROM => 0x0D
    {
      uint8_t cmd[] = {0x57, 0xAB, 0x0D};
      bool ok = sendConfigCommand(cmd, sizeof(cmd));
      Serial.println(ok ? "Save (OK)" : "Save (FAILED)");
    }

    // 8) Reset => 0x0E
    {
      uint8_t cmd[] = {0x57, 0xAB, 0x0E};
      bool ok = sendConfigCommand(cmd, sizeof(cmd));
      Serial.println(ok ? "Reset CH9121 (OK)" : "Reset CH9121 (FAILED)");
    }

    Serial.println("=== Done. Remove CFG0 from GND and power-cycle ===");
    digitalWrite(CFG_PIN, HIGH);
    delay(200);
}
