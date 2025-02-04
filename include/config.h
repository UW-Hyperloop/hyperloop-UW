#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

static const int RX_PIN = 16; // CH9121 TXD1 -> ESP32 RX2
static const int TX_PIN = 17; // CH9121 RXD1 <- ESP32 TX2
static const int CFG_PIN = 25;

extern HardwareSerial CH9121;

void config();
bool sendConfigCommand();

#endif