#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <SPI.h> //enables communication between ESP32 and MAX31855 amp
#include <Adafruit_MAX31855.h> // need to install this package, depends on above as well

// -------------------------------------------------------------------
//    PIN DEFINITIONS 
// -------------------------------------------------------------------

#define POWCTRL_PIN     21
#define BENTCTRL_PIN     26

// #define MOTORSENSE_PIN 22
// #define PUMPSENSE_PIN 23
#define ESTOPSENSE_PIN 33
#define MOTOR_TEMP_PIN 34
#define PUMP_TEMP_PIN 35
//#define FLOW_IN_PIN 12
//#define FLOW_OUT_PIN 13

// SPI for thermocouple
#define DO   18 
#define CS   19
#define CLK  5

// -------------------------------------------------------------------
//    CONSTANTS/THRESHOLDS
// -------------------------------------------------------------------
static const float AREF           = 3.3f;
static const int   ADC_RESOLUTION = 4095;
static const float SHUNT_RESISTOR = 150.0f;
static const float maxTemp        = 3000.0f; // Example threshold

struct Sensor {
  bool         active;
  int          value;
  unsigned long timestamp;
};

struct sys_json;
extern sys_json systemData;

// Sensors
void readSensors();
float get_voltage(int raw_adc);
float get_temperature(float voltage);
float get_flowRate(float voltage);

#endif