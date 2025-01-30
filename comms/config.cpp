#include <Arduino.h>

// Same wiring as before:
//   ESP32 GPIO17 (TX2) -> CH9121 RXD1
//   ESP32 GPIO16 (RX2) -> CH9121 TXD1
//   CFG0 -> GND on power-up to enter config mode

static const int RX_PIN = 16;  // CH9121 TXD1 -> ESP32 RX2
static const int TX_PIN = 17;  // CH9121 RXD1 <- ESP32 TX2
HardwareSerial CH9121Serial(2);

bool sendConfigCommand(const uint8_t *cmd, size_t len) {
  while (CH9121Serial.available()) {
    CH9121Serial.read(); // clear old data
  }
  CH9121Serial.write(cmd, len);
  CH9121Serial.flush();
  delay(50);
  while (CH9121Serial.available()) {
    uint8_t r = CH9121Serial.read();
    if (r == 0xAA) {
      return true;
    }
  }
  return false;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== Reconfiguring CH9121 for TCP SERVER mode ===");

  // CH9121 config mode at 9600 bps
  CH9121Serial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  delay(200);

  // 1) Set port 1 = TCP Server (0x10, param=0x00)
  {
    uint8_t cmd[] = {0x57, 0xAB, 0x10, 0x00};
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
}

void loop() {
  // no-op
}
