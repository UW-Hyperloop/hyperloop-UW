#include <Arduino.h>

static const int RX_PIN = 16; // CH9121 TXD1 -> ESP32 RX2
static const int TX_PIN = 17; // CH9121 RXD1 <- ESP32 TX2

HardwareSerial CH9121(2);

void setup() {
  Serial.begin(115200);  
  // Now the CH9121 is in data mode at 115200
  CH9121.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
  Serial.println("CH9121 Normal Mode, 115200. Trying to connect to 192.168.0.100:5001...");
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
}
