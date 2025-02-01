#include <Arduino.h>
#include "tbm.h"

// ---------------------------------------------------------
//  Helpers for ADC, temperature, flow, etc.
// ---------------------------------------------------------
float get_voltage(int raw_adc) {
  return (raw_adc * AREF) / ADC_RESOLUTION;
}

float get_temperature(float voltage) {
  // Example: 5 mV/°C, 1.25 V offset
  return (voltage - 1.25f) / 0.005f;
}

float get_flowRate(float voltage) {
  // Convert voltage across a shunt resistor to current
  float current_mA = (voltage / SHUNT_RESISTOR) * 1000.0f;
  // Example flow formula: 3.125 * (mA - 4)
  return 3.125f * (current_mA - 4.0f);
}

// ---------------------------------------------------------
//  Read all sensors -> update systemData fields
// ---------------------------------------------------------
void readSensors() {
  // 1. Motor temperature
  int   raw_motor       = analogRead(MOTOR_TEMP_PIN);
  float volt_motor      = get_voltage(raw_motor);
  float motorTempC      = get_temperature(volt_motor);

  systemData.motor_temp.value     = (int)motorTempC;
  systemData.motor_temp.timestamp = millis();

  // 2. Pump temperature => "flow_temp" in your struct
  int   raw_pump        = analogRead(PUMP_TEMP_PIN);
  float volt_pump       = get_voltage(raw_pump);
  float pumpTempC       = get_temperature(volt_pump);

  systemData.flow_temp.value      = (int)pumpTempC;
  systemData.flow_temp.timestamp  = millis();

  // 3. Flow in
  int   raw_flowIn      = analogRead(FLOW_IN_PIN);
  float volt_flowIn     = get_voltage(raw_flowIn);
  float flowRateIn      = get_flowRate(volt_flowIn);

  systemData.flow_in.value        = (int)flowRateIn;
  systemData.flow_in.timestamp    = millis();

  // 4. Flow out
  int   raw_flowOut     = analogRead(FLOW_OUT_PIN);
  float volt_flowOut    = get_voltage(raw_flowOut);
  float flowRateOut     = get_flowRate(volt_flowOut);

  systemData.flow_out.value       = (int)flowRateOut;
  systemData.flow_out.timestamp   = millis();

  // 5. Motor power sense (digital)
  int motorPow = digitalRead(MOTORSENSE_PIN);
  systemData.motor_power.value    = motorPow;
  systemData.motor_power.timestamp= millis();

  // 6. Pump power sense (digital)
  int pumpPow = digitalRead(PUMPSENSE_PIN);
  systemData.pump_power.value     = pumpPow;
  systemData.pump_power.timestamp = millis();

  // 7. Bentonite power sense (if no pin, set 0)
  systemData.bentonite_power.value     = 0;
  systemData.bentonite_power.timestamp = millis();

  // 8. E-stop sense
  int eStopVal = digitalRead(ESTOPSENSE_PIN);
  systemData.estop_button.value        = eStopVal;
  systemData.estop_button.timestamp    = millis();

  // 9. Update global_time
  systemData.global_time = millis();
}
