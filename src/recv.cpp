#include <HardwareSerial.h>
#include <Arduino.h>
#include <tbm.h>

HardwareSerial mySerial(0);  // Use UART1
volatile int temperature = 0.0;
volatile int gasValue = 0;
static const unsigned long DATA_TIMEOUT = 3000;

void receivingESP_setup() {
    mySerial.begin(115200, SERIAL_8N1, 3, 1); // RX on GPIO16, no TX needed

    Serial.println("ESP32 UART Receiver Started");
} 

void receivingESP_loop() {
    while (mySerial.available() > 0) {
        uint8_t rec = mySerial.read();
        uint8_t curr = rec >> 7; // top bit indicates "temp" (1) or "gas" (0)

        if (curr == 1) {
        // If top bit=1 => lower 6 bits are the temperature
        systemData.motor_temp.value = (rec & 0x3F);
        systemData.motor_temp.timestamp = millis();
        } else {
        // If top bit=0 => this is the gas value in lower 7 bits, if needed
        systemData.gas_sensor.value = (rec & 0x7F);
        systemData.gas_sensor.timestamp = millis();
        }
        sensorCount = 0;
    }

    unsigned long now = millis();
    if ((now - systemData.motor_temp.timestamp) > DATA_TIMEOUT) {
        systemData.motor_temp.value = -1;
    }
    if ((now - systemData.gas_sensor.timestamp) > DATA_TIMEOUT) {
        systemData.gas_sensor.value = 0; // or -1, your choice
    }
}