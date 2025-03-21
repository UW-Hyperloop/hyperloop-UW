#include <HardwareSerial.h>
#include <Arduino.h>
#include <tbm.h>

HardwareSerial mySerial(0);  // Use UART1
volatile int temperature = 0.0;
volatile int gasValue = 0;

void receivingESP_setup() {
    mySerial.begin(115200, SERIAL_8N1, 3, 1); // RX on GPIO16, no TX needed

    Serial.println("ESP32 UART Receiver Started");
} 

void receivingESP_loop() {
    if (mySerial.available() >= 3) {
        uint8_t buf[3];
        size_t received = mySerial.readBytes(buf, 3);
        if (received == 3) {
            uint8_t temp = buf[0];
            uint16_t gas = buf[1] | (buf[2] << 8);

            temperature = temp;
            gasValue    = gas;
            systemData.motor_temp.value   = temperature;
            systemData.motor_temp.timestamp = millis();
            systemData.gas_sensor.value   = gasValue;
            systemData.gas_sensor.timestamp = millis();
        }
    }
    else {
        Serial.println("not received");
    }
}