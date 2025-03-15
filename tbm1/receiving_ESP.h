#ifndef RECEIVING_ESP_H
#define RECEIVING_ESP_H

// Function declarations (they are defined in `receivingESP.cpp`)
void receivingESP_setup();
void receivingESP_loop();
float getReceivedTemp();
int getReceivedGas();

#endif
