#include <HardwareSerial.h>
#include <Arduino.h>
#include "tbm.h"

// Declare global variables for storing received values
double receivedTemp = 0.0;
int receivedGas = 0;
HardwareSerial mySerial(1);  // Use UART1

void receivingESP_setup() {
    Serial.begin(9600);
    mySerial.begin(115200, SERIAL_8N1, 17, 16); // RX = 16, TX = 17
    Serial.println("ESP32 UART Receiver Started");
}   

void receivingESP_loop() {
    if (mySerial.available()) {
        String received = mySerial.readStringUntil('\n'); // Read until newline
        Serial.print("Received: ");
        Serial.println(received);

        // Convert String to C-string for parsing
        char receivedChars[received.length() + 1];
        received.toCharArray(receivedChars, sizeof(receivedChars));

        // Split using strtok()
        char *token = strtok(receivedChars, ",");
        if (token != NULL) {
            receivedTemp = atof(token);  // Convert first token to double
            token = strtok(NULL, ",");   // Get the next token
            if (token != NULL) {
                receivedGas = atoi(token); // Convert second token to int
            }
        }

        // Print parsed values
        Serial.print("Parsed Temp: ");
        Serial.println(receivedTemp);
        Serial.print("Parsed Gas: ");
        Serial.println(receivedGas);
    }
}

// Function implementations
double getReceivedTemp() {
    return receivedTemp;
}

int getReceivedGas() {
    return receivedGas;
}
